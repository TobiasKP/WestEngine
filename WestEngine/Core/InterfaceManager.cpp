#include "../CoreHeaders/InterfaceManager.h"

#include "Utils/InputUtils/MouseCallbacks.h"

#include <format>

using namespace WestInterface;

InterfaceManager::InterfaceManager() : IManager(nullptr)
{
  setName(CoreConstants::INTERFACE_MANAGER);
  _facade           = nullptr;
  _cachedInterfaces = _currentX = _currentY = 0;
#ifdef DEBUG
  _demoId = 0;
#endif
};

InterfaceManager::InterfaceManager(WestLogger* logger, WindowManager* manager) : IManager(logger)
{
  setName(CoreConstants::INTERFACE_MANAGER);
  _facade           = nullptr;
  _cachedInterfaces = 0;
  _windowManager    = manager;
#ifdef DEBUG
  _demoId = 0;
#endif
};

InterfaceManager::~InterfaceManager() {}

std::int32_t InterfaceManager::startup()
{
  _currentX = Config::GeneralConfig.WIDTH;
  _currentY = Config::GeneralConfig.HEIGHT;
  _facade   = &WestInterfaceFacade::getInterfaceInstance();
  assert(_facade != nullptr);
  return 0;
}

void InterfaceManager::shutdown()
{
  _facade->shutdown();
}

std::int32_t InterfaceManager::init()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  std::int32_t result = 0;
  result              = _facade->init();

#ifdef DEBUG
  _demoId    = buildTechDemoFooter();
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(std::format("{} ### {} init time: {} ms.\n", getName(), getName(), res));
#endif
  return result;
}

void InterfaceManager::update()
{
  _facade->updateRenderData();
  bool dirty = Config::INTERNAL_UI_COUNT != _cachedInterfaces;
  if (_currentX != Config::GeneralConfig.WIDTH && _currentY != Config::GeneralConfig.HEIGHT)
  {
#ifdef DEBUG
    refreshTechDemoFooter();
#endif
    _facade->resolutionChange(Config::GeneralConfig.WIDTH, Config::GeneralConfig.HEIGHT);
    _facade->updateRenderData();
    _currentX = Config::GeneralConfig.WIDTH;
    _currentY = Config::GeneralConfig.HEIGHT;
    dirty     = true;
  }
  if (dirty)
  {
    std::vector<ElementBounds*> result = _facade->getShownElementsBoundaries();
    MouseCallbacks::setElementBounds(result);
    _cachedInterfaces = Config::INTERNAL_UI_COUNT;
  }
}

#ifdef DEBUG

void InterfaceManager::refreshTechDemoFooter()
{
  _facade->destroyInterface(_demoId);
  _demoId = 0;
  _demoId = buildTechDemoFooter();
  assert(_demoId != 0);
}

std::uint8_t InterfaceManager::buildTechDemoFooter()
{
  assert(_facade != nullptr);
  std::vector<ElementProxy*> elements;

  ElementProxy* redQuad   = new ElementProxy();
  redQuad->type           = DEBUG_ELEMENT;
  redQuad->elementId      = Config::incUiId();
  redQuad->xPosition      = 0.0f;
  redQuad->yPosition      = 0.0f;
  redQuad->colorR         = 215.0f;
  redQuad->colorG         = 207.0f;
  redQuad->colorB         = 196.0f;
  redQuad->colorA         = 1.0f;
  redQuad->row            = 0;
  redQuad->column         = 0;
  redQuad->columnElements = 9;
  redQuad->text           = "Tech Demo";
  elements.push_back(redQuad);

  ElementProxy* quitButton   = new ElementProxy();
  quitButton->type           = BUTTON;
  quitButton->elementId      = Config::incUiId();
  quitButton->xPosition      = Config::GeneralConfig.WIDTH - 100;
  quitButton->yPosition      = 0.0f;
  quitButton->colorR         = 215.0f;
  quitButton->colorG         = 207.0f;
  quitButton->colorB         = 196.0f;
  quitButton->colorA         = 1.0f;
  quitButton->row            = 0;
  quitButton->column         = 0;
  quitButton->columnElements = 4;
  quitButton->givenFlags     = 0x0040;
  quitButton->text           = "Quit";
  quitButton->eventHandler   = [this]()
  {
    assert(_windowManager != nullptr);
    _windowManager->setWindowShouldClose();
  };

  elements.push_back(quitButton);

  assert(elements.size() > 0);
  logDebug(std::format("{} ### Creating Tech Demo interface footer\n", getName()));
  Container* c       = new Container(elements);
  float stretchX     = (float)Config::GeneralConfig.WIDTH / SIZE_E;
  c->xScreenPosition = 0.0f;
  c->yScreenPosition = 0.0f;
  c->stretchX        = stretchX;
  c->stretchY        = 1.0f;
  c->colorR          = 59.0f;
  c->colorG          = 58.0f;
  c->colorB          = 54.0f;
  c->colorA          = 1.0f;
  c->rows            = 1;
  c->columns         = 1;
  c->hiddenContainer = false;

  std::uint8_t footerId = _facade->createNewInterface(c);
  logDebug(std::format("{} ### Created Tech Demo footer -> {}\n", getName(), footerId));
  return footerId;
}

#endif
