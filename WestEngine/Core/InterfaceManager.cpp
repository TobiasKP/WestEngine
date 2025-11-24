#include "../CoreHeaders/InterfaceManager.h"

#include "Utils/InputUtils/MouseCallbacks.h"

InterfaceManager::InterfaceManager() : IManager(nullptr)
{
  setName(CoreConstants::INTERFACE_MANAGER);
  _facade           = nullptr;
  _interfaces       = 0;
  _cachedInterfaces = 0;
};

InterfaceManager::InterfaceManager(WestLogger* logger, WindowManager* manager) : IManager(logger)
{
  setName(CoreConstants::INTERFACE_MANAGER);
  _facade           = nullptr;
  _interfaces       = 0;
  _cachedInterfaces = 0;
  _windowManager    = manager;
};

InterfaceManager::~InterfaceManager() {}

std::int32_t InterfaceManager::startup()
{
  _facade = &WestInterfaceFacade::getInterfaceInstance();
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
  _facade->init();
  _interfaces++;

#ifdef DEBUG
  result     = buildTechDemoFooter();
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(std::format("{} ### {} init time: {} ms.\n", getName(), getName(), res));
#endif
  return result;
}

void InterfaceManager::update()
{
  _facade->updateRenderData();
  if (_interfaces != _cachedInterfaces)
  {
    std::vector<ElementBounds*> result = _facade->getShownElementsBoundaries();
    MouseCallbacks::setElementBounds(result);
    _cachedInterfaces = result.size();
  }
}

#ifdef DEBUG

std::int32_t InterfaceManager::buildTechDemoFooter()
{
  assert(_facade != nullptr);
  std::vector<ElementProxy*> elements;

  ElementProxy* redQuad   = new ElementProxy();
  redQuad->type           = DEBUG_ELEMENT;
  redQuad->elementId      = Config::INTERNAL_UI_ID++;
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
  quitButton->elementId      = Config::INTERNAL_UI_ID++;
  quitButton->xPosition      = 700.0f;
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
  Container* c = new Container(elements);

  c->xScreenPosition = 0.0f;
  c->yScreenPosition = 0.0f;
  c->stretchX        = 20.0f;
  c->stretchY        = 1.0f;
  c->colorR          = 59.0f;
  c->colorG          = 58.0f;
  c->colorB          = 54.0f;
  c->rows            = 1;
  c->columns         = 1;
  c->hiddenContainer = false;

  std::uint8_t footerId = _facade->createNewInterface(c);
  _interfaces++;
  logDebug(std::format("{} ### Created Tech Demo footer -> {}\n", getName(), footerId));
  if (footerId >= 1)
  {
    return 0;
  }
  else
  {
    return -1;
  }
}

#endif
