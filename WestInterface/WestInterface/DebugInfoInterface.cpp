#include "DebugInfoInterface.hpp"

#include "../WestInterfaceFacade.h"

using namespace WestInterface;
DebugInfoInterface::DebugInfoInterface(InterfaceBuilder& interfaceBuilder) : _interfaceBuilder(&interfaceBuilder) {}

DebugInfoInterface::~DebugInfoInterface() {};

ContainerElement* DebugInfoInterface::init()
{
  std::vector<ElementProxy*> elements;
  Container* c = createElementInfo(elements);
  _interfaceBuilder->createNewInterface(c);
  assert(c->elements.size() > 0);
  for (auto* element : c->elements)
  {
    _interfaceBuilder->addElement(element);
  }
  ContainerElement* result = _interfaceBuilder->build();
  assert(result->id > 0);
  Config::GeneralInterfaces.INFO_ID.store(result->id);
  return result;
};

Container* DebugInfoInterface::createElementInfo(std::vector<ElementProxy*>& elements)
{
  ElementProxy* infoLineOne   = new ElementProxy();
  infoLineOne->type           = VLABEL;
  infoLineOne->elementId      = Config::incUiId();
  infoLineOne->colorR         = 59.0f;
  infoLineOne->colorG         = 58.0f;
  infoLineOne->colorB         = 54.0f;
  infoLineOne->colorA         = 1.0f;
  infoLineOne->text           = std::format("Entities in Scene: {}", Config::GeneralInfo.TOTAL_ENTITIES);
  infoLineOne->row            = 0;
  infoLineOne->column         = 0;
  infoLineOne->columnElements = infoLineOne->text.length() + 1;
  infoLineOne->xPosition      = 0;
  infoLineOne->yPosition      = Config::GeneralConfig.HEIGHT - 2 * SIZE_E;
  infoLineOne->stretchX       = 0.8f;
  infoLineOne->stretchY       = 0.75f;
  infoLineOne->zIndex         = 10;
  infoLineOne->givenFlags     = 0x0008;
  Config::GeneralInterfaces.TE_ID.store(infoLineOne->elementId);
  elements.push_back(infoLineOne);

  ElementProxy* infoLineTwo   = new ElementProxy();
  infoLineTwo->type           = VLABEL;
  infoLineTwo->elementId      = Config::incUiId();
  infoLineTwo->colorR         = 59.0f;
  infoLineTwo->colorG         = 58.0f;
  infoLineTwo->colorB         = 54.0f;
  infoLineTwo->colorA         = 1.0f;
  infoLineTwo->text           = std::format("Culled Entities: {}", Config::GeneralInfo.CULLED_ENTITIES);
  infoLineTwo->row            = 1;
  infoLineTwo->column         = 0;
  infoLineTwo->columnElements = infoLineTwo->text.length() + 1;
  infoLineTwo->xPosition      = 0;
  infoLineTwo->yPosition      = Config::GeneralConfig.HEIGHT - 2 * SIZE_E;
  infoLineTwo->stretchX       = 0.8f;
  infoLineTwo->stretchY       = 0.75f;
  infoLineTwo->zIndex         = 10;
  infoLineTwo->givenFlags     = 0x0008;
  Config::GeneralInterfaces.CE_ID.store(infoLineTwo->elementId);
  elements.push_back(infoLineTwo);


  Container* c       = new Container(elements);
  c->xScreenPosition = 0;
  c->yScreenPosition = Config::GeneralConfig.HEIGHT - 2 * SIZE_E;
  c->colorR          = 59.0f;
  c->colorG          = 58.0f;
  c->colorB          = 54.0f;
  c->colorA          = 1.0f;
  c->stretchX        = 1.0f;
  c->stretchY        = 1.0f;
  c->rows            = 2;
  c->columns         = 30;
  c->hiddenContainer = true;
  _logger.log(Level::Info, "@@@ Instructions for info text created.\n");
  return c;
}

void DebugInfoInterface::refreshInterface()
{
  if (Config::GeneralInterfaces.INFO_ID.load() == -1)
  {
    return;
  }
#ifdef DEBUG
  _logger.log(Level::Info, "@@@ Refreshing info Interfaces.\n");
#endif
  WestInterfaceFacade& facade = WestInterfaceFacade::getInterfaceInstance();
  facade.destroyInterface(Config::GeneralInterfaces.INFO_ID.load());
  std::vector<ElementProxy*> elements;
  Config::GeneralInterfaces.INFO_ID.store(-1);
  Config::GeneralInterfaces.TE_ID.store(-1);
  Config::GeneralInterfaces.CE_ID.store(-1);
  Config::GeneralInterfaces.INFO_ID.store(facade.createNewInterface(createElementInfo(elements)));
  assert(Config::GeneralInterfaces.INFO_ID.load() != -1);
}
