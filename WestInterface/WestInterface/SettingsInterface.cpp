#include "SettingsInterface.h"

#include "../WestInterfaceFacade.h"

#include <chrono>
#include <thread>


SettingsInterface::SettingsInterface(InterfaceBuilder& interfaceBuilder) : _interfaceBuilder(&interfaceBuilder)
{
  _id = 0;
}

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
  settingsButton->eventHandler   = [this]()
  {
    createSettingInterface();
    std::thread(
      []()
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        // Config::PAUSE.exchange(true);
      })
      .detach();
  };

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
  c->colorA          = 0.9f;
  c->stretchX        = 1.0f;
  c->stretchY        = 1.0f;
  c->rows            = 1;
  c->columns         = 1;
  c->hiddenContainer = false;
  c->givenFlags      = 0x0010;
  _logger.log(Level::Info, "@@@ Instructions for basic settings menu created.\n");
  return c;
}

void SettingsInterface::createSettingInterface()
{
  std::vector<ElementProxy*> e;
  e.push_back(createQuitSettingsButton());
  Container* c                = new Container(e);
  WestInterfaceFacade& facade = WestInterfaceFacade::getInterfaceInstance();
  c->xScreenPosition          = Config::GeneralConfig.WIDTH / 3;
  c->yScreenPosition          = Config::GeneralConfig.HEIGHT - (Config::GeneralConfig.HEIGHT / 3);
  c->colorR                   = 59.0f;
  c->colorG                   = 58.0f;
  c->colorB                   = 54.0f;
  c->colorA                   = 0.9f;
  c->hiddenContainer          = false;
  c->stretchX                 = 1.0f;
  c->stretchY                 = 1.0f;
  c->rows                     = 4;
  c->columns                  = 5;
  c->hiddenContainer          = false;
  assert(_id == 0);
  _id = facade.createNewInterface(c);
}

ElementProxy* SettingsInterface::createQuitSettingsButton()
{
  ElementProxy* quitButton   = new ElementProxy();
  quitButton->type           = BUTTON;
  quitButton->elementId      = Config::INTERNAL_UI_ID++;
  float x                    = Config::GeneralConfig.WIDTH / 3;
  float y                    = Config::GeneralConfig.HEIGHT - (Config::GeneralConfig.HEIGHT / 3) - 1;
  quitButton->xPosition      = x;
  quitButton->yPosition      = y;
  quitButton->colorR         = 215.0f;
  quitButton->colorG         = 207.0f;
  quitButton->colorB         = 196.0f;
  quitButton->colorA         = 1.0f;
  quitButton->row            = 3;
  quitButton->column         = 4;
  quitButton->columnElements = 1;
  quitButton->givenFlags     = 0x040 | 0x0008;
  quitButton->text           = "X";
  quitButton->eventHandler   = [this]()
  {
    WestInterfaceFacade& facade = WestInterfaceFacade::getInterfaceInstance();
    facade.destroyInterface(_id);
    _id = 0;
  };

  /*
  TextureInformation* tex = new TextureInformation();
  tex->path               = "assets/Textures/gear.png";
  tex->wrapping_x         = GL_CLAMP_TO_BORDER;
  tex->wrapping_y         = GL_CLAMP_TO_BORDER;
  quitButton->texture     = tex;*/

  return quitButton;
}

ElementProxy* SettingsInterface::createResolutionSetting()
{
  return nullptr;
}
