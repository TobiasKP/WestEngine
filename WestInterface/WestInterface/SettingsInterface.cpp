#include "SettingsInterface.h"

#include "../WestInterfaceFacade.h"

#include <chrono>
#include <thread>


SettingsInterface::SettingsInterface(InterfaceBuilder& interfaceBuilder) : _interfaceBuilder(&interfaceBuilder)
{
  _id = _resolutionId = 0;
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
  settingsButton->elementId      = Config::incUiId();
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
    if (_id != 0)
    {
      return;
    }
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
  e.push_back(createResolutionSetting());
  Container* c                = new Container(e);
  WestInterfaceFacade& facade = WestInterfaceFacade::getInterfaceInstance();
  c->xScreenPosition          = Config::GeneralConfig.WIDTH / 4;
  c->yScreenPosition          = 75;
  c->colorR                   = 59.0f;
  c->colorG                   = 58.0f;
  c->colorB                   = 54.0f;
  c->colorA                   = 0.7f;
  c->hiddenContainer          = false;
  c->stretchX                 = 1.0f;
  c->stretchY                 = 1.0f;
  c->rows                     = 10;
  c->columns                  = 10;
  c->hiddenContainer          = false;
  assert(_id == 0);
  _id = facade.createNewInterface(c);
}

ElementProxy* SettingsInterface::createQuitSettingsButton()
{
  ElementProxy* quitButton   = new ElementProxy();
  quitButton->type           = BUTTON;
  quitButton->elementId      = Config::incUiId();
  float x                    = (float)Config::GeneralConfig.WIDTH / 4 - 10;
  float y                    = 65;
  quitButton->xPosition      = x;
  quitButton->yPosition      = y;
  quitButton->colorR         = 215.0f;
  quitButton->colorG         = 207.0f;
  quitButton->colorB         = 196.0f;
  quitButton->colorA         = 1.0f;
  quitButton->row            = 9;
  quitButton->column         = 9;
  quitButton->columnElements = 1;
  quitButton->givenFlags     = 0x0040;
  quitButton->text           = "X";
  quitButton->eventHandler   = [this]()
  {
    WestInterfaceFacade& facade = WestInterfaceFacade::getInterfaceInstance();
    facade.destroyInterface(_id);
    _id = 0;
  };

  /*
   * TODO: Add proper UI Component with UI Bitmap
  TextureInformation* tex = new TextureInformation();
  tex->path               = "assets/Textures/gear.png";
  tex->wrapping_x         = GL_CLAMP_TO_BORDER;
  tex->wrapping_y         = GL_CLAMP_TO_BORDER;
  quitButton->texture     = tex;*/

  return quitButton;
}

ElementProxy* SettingsInterface::createResolutionSetting()
{
  ElementProxy* resolution   = new ElementProxy();
  resolution->type           = DROPDOWN;
  resolution->elementId      = Config::incUiId();
  float x                    = (float)Config::GeneralConfig.WIDTH / 4 - 1;
  float y                    = 75;
  resolution->stretchX       = 0.8f;
  resolution->stretchY       = 0.8f;
  resolution->xPosition      = x * 1.1;
  resolution->yPosition      = y * 1.5;
  resolution->colorR         = 215.0f;
  resolution->colorG         = 207.0f;
  resolution->colorB         = 196.0f;
  resolution->colorA         = 1.0f;
  resolution->text           = "Screen Resolution";
  resolution->row            = 9;
  resolution->column         = 1;
  resolution->zIndex         = 5;
  resolution->columnElements = resolution->text.length() + 1;
  resolution->givenFlags     = 0x0080;
  resolution->eventHandler   = [this]()
  {
    if (_resolutionId != 0)
    {
      return;
    }
    createResolutionOptions();
  };
  /*
   * TODO: Add proper UI Component with UI Bitmap
  TextureInformation* tex = new TextureInformation();
  tex->path               = "assets/Textures/gear.png";
  tex->wrapping_x         = GL_CLAMP_TO_BORDER;
  tex->wrapping_y         = GL_CLAMP_TO_BORDER;
  quitButton->texture     = tex;*/
  return resolution;
}

void SettingsInterface::createResolutionOptions()
{
  std::vector<ElementProxy*> e;
}
