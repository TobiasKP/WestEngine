#include "../CoreHeaders/InterfaceManager.h"

#include "Utils/InputUtils/MouseCallbacks.h"

InterfaceManager::InterfaceManager() : IManager(nullptr) {
  setName(CoreConstants::INTERFACE_MANAGER);
  _facade = nullptr;
  _interfaces = 0;
  _cachedInterfaces = 0;
};

InterfaceManager::InterfaceManager(WestLogger *logger, WindowManager *manager)
    : IManager(logger) {
  setName(CoreConstants::INTERFACE_MANAGER);
  _facade = nullptr;
  _interfaces = 0;
  _cachedInterfaces = 0;
  _windowManager = manager;
};

InterfaceManager::~InterfaceManager() {}

std::int32_t InterfaceManager::startup() {
  _facade = &WestInterfaceFacade::getInterfaceInstance();
  assert(_facade != nullptr);
  return 0;
}

void InterfaceManager::shutdown() { _facade->shutdown(); }

std::int32_t InterfaceManager::init() {
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  std::int32_t result = 0;
  _facade->init();

#ifdef DEBUG
  result = buildTechDemoFooter();
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(
      std::format("{} ### {} init time: {} ms.\n", getName(), getName(), res));
#endif
  return result;
}

void InterfaceManager::update() {
  _facade->updateRenderData();
  if (_interfaces != _cachedInterfaces) {
    std::vector<ElementBounds *> result = _facade->getShownElementsBoundaries();
    MouseCallbacks::setElementBounds(result);
    _cachedInterfaces = result.size();
  }
}

#ifdef DEBUG

std::int32_t InterfaceManager::buildTechDemoFooter() {
  assert(_facade != nullptr);
  std::vector<ElementProxy *> elements;

  ElementProxy *redQuad = new ElementProxy();
  redQuad->type = DEBUG_ELEMENT;
  redQuad->elementId = 1;
  redQuad->xPosition = 0.0f;
  redQuad->yPosition = 0.0f;
  redQuad->row = 0;
  redQuad->column = 0;
  redQuad->columnElements = 9;
  redQuad->text = "Tech Demo";
  elements.push_back(redQuad);

  ElementProxy *quitButton = new ElementProxy();
  quitButton->type = BUTTON;
  quitButton->elementId = 2;
  quitButton->xPosition = 480.0f;
  quitButton->yPosition = 0.0f;
  quitButton->row = 0;
  quitButton->column = 0;
  quitButton->columnElements = 4;
  quitButton->text = "Quit";
  quitButton->eventHandler = [this]() {
    assert(_windowManager != nullptr);
    _windowManager->setWindowShouldClose();
  };

  elements.push_back(quitButton);

  assert(elements.size() > 0);
  logDebug(
      std::format("{} ### Creating Tech Demo interface footer\n", getName()));
  std::uint8_t footerId = _facade->createNewInterface(0.0f, 0.0f, 16.0f, 1.0f,
                                                      1, 1, false, elements);
  _interfaces++;
  logDebug(std::format("{} ### Created Tech Demo footer -> {}\n", getName(),
                       footerId));
  delete redQuad;
  if (footerId >= 1)
    return 0;
  else
    return 1;
}

#endif
