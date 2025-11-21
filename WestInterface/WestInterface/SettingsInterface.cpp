#include "SettingsInterface.h"


SettingsInterface::SettingsInterface(InterfaceBuilder& interfaceBuilder) : _interfaceBuilder(&interfaceBuilder) {}

SettingsInterface::~SettingsInterface() {}

ContainerElement* SettingsInterface::init()
{
  _interfaceBuilder->createNewInterface(createSettingButton());
  return _interfaceBuilder->build();
}

Container* SettingsInterface::createSettingButton()
{
  std::vector<ElementProxy*> elements;
  Container* c       = new Container(elements);
  c->xScreenPosition = Config::GeneralConfig.WIDTH - SIZE_E;
  c->yScreenPosition = Config::GeneralConfig.HEIGHT - SIZE_E;
  c->colorR          = 59.0f;
  c->colorG          = 58.0f;
  c->colorB          = 54.0f;
  c->stretchX        = 1.0f;
  c->stretchY        = 1.0f;
  c->rows            = 1;
  c->columns         = 1;
  c->hiddenContainer = false;
  return c;
}

IElement* SettingsInterface::createSettingInterface()
{
  return nullptr;
}

IElement* SettingsInterface::createResolutionSetting()
{
  return nullptr;
}
