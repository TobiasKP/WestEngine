#include "../CoreHeaders/RenderManager.h"

#include "../CoreHeaders/Utils/Math/PositionCalculation.h"

#include <Config.h>
#include <format>
#include <glm/ext/matrix_clip_space.hpp>
#include <UniformConstants.hpp>
#include <UniformParams.hpp>
#include <WestAssetFacade.hpp>

RenderManager::RenderManager() : IManager(nullptr)
{
  setName(CoreConstants::RENDER_MANAGER);
  _facade = nullptr;
  _scene  = nullptr;
}

RenderManager::RenderManager(WestLogger* logger, const std::shared_ptr<Scene>& s) : IManager(logger)
{
  setName(CoreConstants::RENDER_MANAGER);
  _facade = nullptr;
  _scene  = s;
}

RenderManager::~RenderManager() {}

std::int32_t RenderManager::startup()
{
  return 0;
}

void RenderManager::shutdown()
{
#ifdef DEBUG
  logDebug(std::format("{} ### Shutting down {}...\n", getName(), getName()));
#endif
}

std::int32_t RenderManager::init()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  _facade = &WestRenderer::WestRendererFacade::getRendererFacade();
  assert(_scene != nullptr && _facade != nullptr);

#ifdef DEBUG
  _debugDrawSystem = std::make_unique<DebugDrawSystem>(_scene, _facade);
  _debugDrawSystem->init();
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(std::format("{} ### RenderManager init time: {} ms.\n", getName(), res));
#endif

  return 0;
}

void RenderManager::update()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif
  std::shared_ptr<World> world      = _scene->getWorld();
  WorldUniformParams params         = {world->getFlagData(), world->getGridSize(), world->getOrigin()};
  std::atomic<std::int32_t> skipped = 0;
  if (!Config::PAUSE)
  {
    _scene->getCamera()->update();
  }
  _facade->clearColor();
  _facade->renderWorld(world->getModelGuid(), world->getId(), world->getShaderId(), world->isDirty(), &params);
  _entitiesToRender.assign(_scene->getEntities().size(), 0);
  const Frustum& frustum      = _scene->getCamera()->getFrustum();
  ComponentRegistry* registry = _scene->getRegistry().get();
  std::vector<std::future<void>> futures;

  for (size_t i = 0; i < _scene->getEntities().size(); i += 20)
  {
    futures.emplace_back(Config::THREADPOOL->enqueue(
      [this, i, &skipped, &frustum, registry, entities = &_scene->getEntities()]
      {
        for (size_t j = 0; j < 20; j++)
        {
          if (i + j >= entities->size())
          {
            return;
          }
          const Entity& entity = entities->at(i + j);


          _entitiesToRender[i + j] = AABBcheck(entity, frustum, registry);
          if (!_entitiesToRender[i + j])
          {
            skipped++;
#ifdef DEBUG
            logCycle(
              std::format("{} ### skipped rendering entity: {}, did not pass AABB.\n", getName(), entity.getId()));
#endif
          }
        }
      }));
  }

  for (auto& f : futures)
  {
    f.wait();
  }


  for (size_t i = 0; i < _scene->getEntities().size(); i++)
  {
    if (!_entitiesToRender[i])
    {
      continue;
    }
    const Entity& entity   = _scene->getEntities().at(i);
    Position* p            = _scene->getRegistry()->getComponent<Position>(entity.getId());
    Appearance* appearance = _scene->getRegistry()->getComponent<Appearance>(entity.getId());
    const Model* model     = WestData::WestAssetFacade::getAssetFacade().requestModelFromScene(entity.getModelGuid());
    EntityUniformParams params = {};
    params.transform           = PositionCalculation::createTransformationMatrix(p->position, p->rotation, p->scale);
    if (model)
    {
      params.diffuseColor  = model->getMeshes().front().material.diffuseColor;
      params.emissiveColor = model->getMeshes().front().material.emissiveColor;
      if (appearance)
      {
        params.diffuseColor  += appearance->diffuseOverride;
        params.emissiveColor += appearance->emissiveOverride;
      }

      _facade->renderEntity(entity.getModelGuid(), entity.getId(), entity.getShaderId(), &params, false);
    }
  }

  Config::GeneralInfo.TOTAL_ENTITIES  = _scene->getEntities().size();
  Config::GeneralInfo.CULLED_ENTITIES = skipped;
  _facade->renderInterface();

#ifdef DEBUG
  _debugDrawSystem->update();
  renderDebugEntities();
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logCycle(std::format("{} ### render time for all entites in scene: {} ms.\n", getName(), res));
#endif
}

bool RenderManager::AABBcheck(const Entity& e, const Frustum& f, ComponentRegistry* reg)
{
  const Model* model = WestData::WestAssetFacade::getAssetFacade().requestModelFromScene(e.getModelGuid());
  if (model == nullptr)
  {
    return false;
  }
  Position* p = reg->getComponent<Position>(e.getId());
  for (const Mesh& m : model->getMeshes())
  {
    const AABB aabb = m.aabb; 
    glm::vec3 min = aabb.min * p->scale + p->position;
    glm::vec3 max = aabb.max * p->scale + p->position;

    bool res =
      f.near.getSignedDistance(glm::vec3(
        f.near.normal.x > 0 ? max.x : min.x, f.near.normal.y > 0 ? max.y : min.y, f.near.normal.z > 0 ? max.z : min.z))
        >= 0
      && f.bottom.getSignedDistance(glm::vec3(f.bottom.normal.x > 0 ? max.x : min.x,
                                              f.bottom.normal.y > 0 ? max.y : min.y,
                                              f.bottom.normal.z > 0 ? max.z : min.z))
           >= 0
      &&

      f.far.getSignedDistance(glm::vec3(
        f.far.normal.x > 0 ? max.x : min.x, f.far.normal.y > 0 ? max.y : min.y, f.far.normal.z > 0 ? max.z : min.z))
        >= 0
      && f.left.getSignedDistance(glm::vec3(f.left.normal.x > 0 ? max.x : min.x,
                                            f.left.normal.y > 0 ? max.y : min.y,
                                            f.left.normal.z > 0 ? max.z : min.z))
           >= 0
      && f.right.getSignedDistance(glm::vec3(f.right.normal.x > 0 ? max.x : min.x,
                                             f.right.normal.y > 0 ? max.y : min.y,
                                             f.right.normal.z > 0 ? max.z : min.z))
           >= 0
      && f.top.getSignedDistance(glm::vec3(
           f.top.normal.x > 0 ? max.x : min.x, f.top.normal.y > 0 ? max.y : min.y, f.top.normal.z > 0 ? max.z : min.z))
           >= 0;
    if (!res)
    {
      return res;
    }
  }
  return true;
}

#ifdef DEBUG

void RenderManager::renderDebugEntities()
{
  std::vector<std::tuple<std::string, std::uint32_t, GLuint, EntityUniformParams>> debugData;
  for (const Entity& entity : _scene->getDebugEntities())
  {
    EntityUniformParams params = {};
    params.diffuseColor        = glm::vec3(1.0f, 0.0f, 0.0f);
    debugData.emplace_back(entity.getModelGuid(), entity.getId(), entity.getShaderId(), params);
  }
  _facade->renderDebugEntities(debugData);
}

#endif
