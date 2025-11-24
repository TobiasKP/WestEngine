#include "SettingsInterface.h"


SettingsInterface::SettingsInterface(InterfaceBuilder& interfaceBuilder) : _interfaceBuilder(&interfaceBuilder) {}

SettingsInterface::~SettingsInterface()
{
  _elements.clear();
}

ContainerElement* SettingsInterface::init()
{
  Container* c = createSettingButton();
  _interfaceBuilder->createNewInterface(c);
  assert(c->elements.size() > 0);
  for (auto* element : c->elements)
  {
    _interfaceBuilder->addElement(element);
  }
  return _interfaceBuilder->build();
}

Container* SettingsInterface::createSettingButton()
{
  ElementProxy* settingsButton   = new ElementProxy();
  settingsButton->type           = BUTTON;
  settingsButton->elementId      = Config::INTERNAL_UI_ID++;
  settingsButton->xPosition      = Config::GeneralConfig.WIDTH - SIZE_E;
  settingsButton->yPosition      = Config::GeneralConfig.HEIGHT - SIZE_E;
  settingsButton->colorR         = 59.0f;
  settingsButton->colorG         = 58.0f;
  settingsButton->colorB         = 54.0f;
  settingsButton->colorA         = 1.0f;
  settingsButton->row            = 0;
  settingsButton->column         = 0;
  settingsButton->columnElements = 1; 
  settingsButton->givenFlags     = 0x0100 | 0x0004;
  settingsButton->eventHandler   = [this]() {};

  TextureInformation* tex = new TextureInformation();
  tex->path               = "assets/Textures/gear.png";
  tex->wrapping_x         = GL_CLAMP_TO_BORDER;
  tex->wrapping_y         = GL_CLAMP_TO_BORDER;
  settingsButton->texture = tex;

  _elements.push_back(settingsButton);

  Container* c       = new Container(_elements);
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
  c->givenFlags      = 0x0010;
  _logger.log(Level::Info, "@@@ Instructions for basic settings menu created.\n");
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
