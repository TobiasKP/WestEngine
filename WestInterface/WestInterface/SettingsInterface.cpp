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
  c->colorA                   = 0.8f;
  c->hiddenContainer          = false;
  c->stretchX                 = 1.0f;
  c->stretchY                 = 1.0f;
  c->rows                     = 10;
  c->columns                  = 11;
  c->hiddenContainer          = false;
  assert(_id == 0);

  /*
   * TODO: Add proper UI Component with UI Bitmap
  TextureInformation* tex = new TextureInformation();
  tex->path               = "assets/Textures/gear.png";
  tex->wrapping_x         = GL_CLAMP_TO_BORDER;
  tex->wrapping_y         = GL_CLAMP_TO_BORDER;
  quitButton->texture     = tex;*/


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
  quitButton->column         = 10;
  quitButton->columnElements = 1;
  quitButton->givenFlags     = 0x0040;
  quitButton->text           = "X";
  quitButton->eventHandler   = [this]()
  {
    WestInterfaceFacade& facade = WestInterfaceFacade::getInterfaceInstance();
    facade.destroyInterface(_id);
    if (_resolutionId != 0)
    {
      facade.destroyInterface(_resolutionId);
      _resolutionId = 0;
    }
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
  resolution->stretchX       = 0.8f;
  resolution->stretchY       = 0.75f;
  resolution->xPosition      = (float)Config::GeneralConfig.WIDTH / 4 - 30;
  resolution->yPosition      = 35;
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
      WestInterfaceFacade& facade = WestInterfaceFacade::getInterfaceInstance();
      facade.destroyInterface(_resolutionId);
      _resolutionId = 0;
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
  std::uint8_t row = 0;
  for (auto res : _supportedResolutions)
  {
    e.push_back(resolutionOption(std::get<0>(res), std::get<1>(res), row));
    row++;
  }
  Container* c                 = new Container(e);
  WestInterfaceFacade& facade  = WestInterfaceFacade::getInterfaceInstance();
  c->xScreenPosition           = (Config::GeneralConfig.WIDTH / 4 - 1) + 6 * SIZE_E;
  c->yScreenPosition           = 8 * SIZE_E;
  c->colorR                    = 86.0f;
  c->colorG                    = 81.0f;
  c->colorB                    = 71.0f;
  c->colorA                    = 1.0f;
  c->hiddenContainer           = false;
  c->stretchX                  = 4.0f;
  c->stretchY                  = 2.0f;
  c->rows                      = 1;
  c->columns                   = 1;
  c->zIndex                    = 9;
  c->hiddenContainer           = false;
  c->givenFlags               |= 0x0010;
  assert(_resolutionId == 0);

  /*
   * TODO: Add proper UI Component with UI Bitmap
  TextureInformation* tex = new TextureInformation();
  tex->path               = "assets/Textures/gear.png";
  tex->wrapping_x         = GL_CLAMP_TO_BORDER;
  tex->wrapping_y         = GL_CLAMP_TO_BORDER;
  quitButton->texture     = tex;*/


  _resolutionId = facade.createNewInterface(c);
}

ElementProxy* SettingsInterface::resolutionOption(std::uint32_t x, std::uint32_t y, std::uint8_t row)
{
  ElementProxy* button   = new ElementProxy();
  button->type           = BUTTON;
  button->elementId      = Config::incUiId();
  button->xPosition      = (Config::GeneralConfig.WIDTH / 4 - 1) + 6 * SIZE_E;
  button->yPosition      = 8 * SIZE_E;
  button->stretchY       = 0.8f;
  button->stretchX       = 0.8f;
  button->colorR         = 215.0f;
  button->colorG         = 207.0f;
  button->colorB         = 196.0f;
  button->colorA         = 1.0f;
  button->row            = row;
  button->column         = 0;
  button->givenFlags     = 0x0080;
  button->text           = std::format("{} x {}", x, y);
  button->columnElements = button->text.length() + 1;
  button->eventHandler   = [this, x, y]()
  {
    Config::requestedWidth      = x;
    Config::requestedHeight     = y;
    WestInterfaceFacade& facade = WestInterfaceFacade::getInterfaceInstance();
    facade.destroyInterface(_resolutionId);
    _resolutionId = 0;
  };

  /*
   * TODO: Add proper UI Component with UI Bitmap
  TextureInformation* tex = new TextureInformation();
  tex->path               = "assets/Textures/gear.png";
  tex->wrapping_x         = GL_CLAMP_TO_BORDER;
  tex->wrapping_y         = GL_CLAMP_TO_BORDER;
  quitButton->texture     = tex;*/

  return button;
}
