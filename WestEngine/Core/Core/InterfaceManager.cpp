#include "../CoreHeaders/InterfaceManager.h"

InterfaceManager::InterfaceManager() : IManager(nullptr) {
  setName(CoreConstants::INTERFACE_MANAGER);
  _facade = nullptr;
};

InterfaceManager::InterfaceManager(WestLogger *logger) : IManager(logger) {
  setName(CoreConstants::INTERFACE_MANAGER);
  _facade = nullptr;
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

#ifdef DEBUG
  result = buildTechDemoFooter();
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(
      std::format("{} ### {} init time: {} ms.\n", getName(), getName(), res));
#endif
  return result;
}

void InterfaceManager::update() { _facade->updateRenderData(); }

#ifdef DEBUG

std::int32_t InterfaceManager::buildTechDemoFooter() {
  assert(_facade != nullptr);
  std::vector<ElementProxy *> elements;

  ElementProxy *redQuad = new ElementProxy();
  redQuad->type = DEBUG_ELEMENT;
  redQuad->elementId = 1;
  redQuad->colorR = 1.0f;
  redQuad->colorG = 0.0f;
  redQuad->colorB = 0.0f;
  redQuad->colorA = 1.0f;
  redQuad->xPosition = 0.0f;
  redQuad->yPosition = 0.0f;
  redQuad->scale = 1.0f;
  redQuad->column = 0;
  redQuad->row = 0;

  elements.push_back(redQuad);
  assert(elements.size() > 0);

  logDebug(
      std::format("{} ### Creating Tech Demo interface footer\n", getName()));
  std::uint8_t footerId =
      _facade->createNewInterface(0.0f, 0.0f, 1.0f, 1.0f, false, elements);
  logDebug(std::format("{} ### Created Tech Demo footer -> {}\n", getName(),
                       footerId));
  delete redQuad;
  if (footerId >= 1)
    return 0;
  else
    return 1;
}

#endif
