#ifdef DEBUG

#include "../../CoreHeaders/Systems/DebugDrawSystem.h"

#include <Config.h>
#include <UniformConstants.hpp>

DebugDrawSystem::DebugDrawSystem(std::shared_ptr<Scene> scene, WestRenderer::WestRendererFacade* renderer)
  : _scene(scene), _renderer(renderer)
{
}

void DebugDrawSystem::init()
{
  _debugShaderId = _renderer->registerShader("/shader/Debug/DebugVShader.vs", "/shader/Debug/DebugFShader.fs");
}

void DebugDrawSystem::update()
{
  std::shared_ptr<ComponentRegistry> reg              = _scene->getRegistry();
  std::shared_ptr<ComponentArray<Movement>> movements = reg->getComponentArray<Movement>();
  size_t size = movements->getSize();

  for (size_t i = 0; i < size; i++)
  {
    Movement* mov    = movements->getComponentByIdx(i);
    std::uint32_t id = movements->getEntityIdByIdx(i);

    if (mov->removeDebugInfo)
    {
      removeDebugEntity(mov->debugEntity);
      mov->removeDebugInfo    = false;
      mov->debugInfoDisplayed = false;
      mov->debugEntity        = 0;
    }
    else if (!mov->debugInfoDisplayed && mov->destination.has_value())
    {
      Position* pos = reg->getComponent<Position>(id);
      std::vector<glm::vec3> points{pos->position, *mov->destination};
      points.insert(points.end(), mov->path.begin(), mov->path.end());
      mov->debugEntity        = createDebugLine(points);
      mov->debugInfoDisplayed = true;
    }
  }

  // Projectile debug lines: recreated each frame since projectiles move
  for (std::uint32_t id : _projectileDebugEntities)
  {
    removeDebugEntity(id);
  }
  _projectileDebugEntities.clear();

  std::shared_ptr<ComponentArray<Projectile>> projectiles = reg->getComponentArray<Projectile>();
  size_t projSize = projectiles->getSize();
  for (size_t i = 0; i < projSize; i++)
  {
    Projectile* proj = projectiles->getComponentByIdx(i);
    std::uint32_t id = projectiles->getEntityIdByIdx(i);
    Position* projPos = reg->getComponent<Position>(id);
    Position* targetPos = reg->getComponent<Position>(proj->destination);
    if (projPos && targetPos)
    {
      _projectileDebugEntities.push_back(createDebugLine({projPos->position, targetPos->position}));
    }
  }
}

std::uint32_t DebugDrawSystem::createDebugLine(const std::vector<glm::vec3>& points)
{
  std::uint32_t entityId = Config::incEntityId();
  std::string guid       = "debug-line-" + std::to_string(entityId);

  std::vector<Vertex> verts;
  std::vector<std::uint32_t> indices;
  AABB aabb = {points.front(), points.front()};
  for (std::uint32_t i = 0; i < points.size(); i++)
  {
    verts.push_back({points[i], {0, 1, 0}, {0, 0}});
    aabb = {glm::min(aabb.min, points[i]), glm::max(aabb.max, points[i])};
    if (i > 0)
    {
      indices.insert(indices.end(), {i - 1, i});
    }
  }
  std::vector<Texture> textures;
  Mesh mesh(guid, verts, indices, textures, aabb);
  mesh.material.diffuseColor = glm::vec3(1.0f, 0.0f, 0.0f);

  Model model;
  model.setGuid(guid);
  model.setName(guid);
  model.addMesh(mesh);

  const std::string& actualGuid = WestData::WestAssetFacade::getAssetFacade().addModelToScene(model);

  Entity entity(entityId);
  entity.setModelGuid(actualGuid);
  entity.setShaderId(_debugShaderId);
  entity.debugEntity();
  entity.initialize();

  _renderer->createUniform(UniformConstants::DCOLOR, _debugShaderId, entityId);

  _scene->addDebugEntity(std::move(entity));
  return entityId;
}

void DebugDrawSystem::removeDebugEntity(std::uint32_t entityId)
{
  Entity* e = _scene->getEntityById(entityId);
  if (e == nullptr)
  {
    return;
  }
  const std::string& guid = e->getModelGuid();
  _renderer->cleanupModel(guid);
  WestData::WestAssetFacade::getAssetFacade().deleteModelFromScene(guid);
  _scene->removeEntity(*e);
}

#endif
