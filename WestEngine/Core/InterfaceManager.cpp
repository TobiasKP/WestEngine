#include "../CoreHeaders/InterfaceManager.h"

#include "../Constants/LuaAPI.hpp"
#include "../Core/Scripting/LuaFacade.hpp"
#include "InputManager.h"

#include <format>

using namespace WestInterface;

InterfaceManager::InterfaceManager() : IManager(nullptr)
{
  setName(CoreConstants::INTERFACE_MANAGER);
  _facade           = nullptr;
  _dispatcher       = nullptr;
  _cachedInterfaces = _currentX = _currentY = 0;
  _currentPos                               = glm::vec2(0);
  _currentHover                             = std::make_tuple(-1, false);
#ifdef DEBUG
  _demoId = 0;
#endif
};

InterfaceManager::InterfaceManager(WestLogger* logger, WindowManager* manager, std::shared_ptr<EventDispatcher> d)
  : IManager(logger)
{
  setName(CoreConstants::INTERFACE_MANAGER);
  _facade           = nullptr;
  _cachedInterfaces = 0;
  _windowManager    = manager;
  _dispatcher       = d;
  _currentPos       = glm::vec2(0);
  _currentHover     = std::make_tuple(-1, false);
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
  LuaFacade::getLuaFacadeInstance().registerCFunction(registerInterface, LuaAPI::C_CREATE_INTERFACE.data(), this);
  LuaFacade::getLuaFacadeInstance().registerCFunction(destroyInterface, LuaAPI::C_DESTROY_INTERFACE.data(), this);
  LuaFacade::getLuaFacadeInstance().registerCFunction(updateInterfaceValue, LuaAPI::C_UPDATE_INTERFACE.data(), this);
  _dispatcher->subscribe(EventIdentifiers::MOUSE_MOVE,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
  _dispatcher->subscribe(EventIdentifiers::MOUSE_LCLICK,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
  _dispatcher->subscribe(EventIdentifiers::KEY,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
  result = _facade->init();

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
  pollEvents();
  _facade->updateRenderData();
  bool dirty = Config::INTERNAL_UI_COUNT != _cachedInterfaces;
  if (_currentTE != Config::GeneralInfo.TOTAL_ENTITIES && Config::GeneralInterfaces.TE_ID.load() != -1)
  {
    _facade->notify(Config::GeneralInterfaces.TE_ID.load(),
                    0x08,
                    std::format("Entities in Scene: {}", Config::GeneralInfo.TOTAL_ENTITIES));
    _currentTE = Config::GeneralInfo.TOTAL_ENTITIES;
  }
  if (_currentCE != Config::GeneralInfo.CULLED_ENTITIES && Config::GeneralInterfaces.CE_ID.load() != -1)
  {
    _facade->notify(Config::GeneralInterfaces.CE_ID.load(),
                    0x08,
                    std::format("Culled Entities: {}", Config::GeneralInfo.CULLED_ENTITIES));
    _currentCE = Config::GeneralInfo.CULLED_ENTITIES;
  }
  if (_currentX != Config::GeneralConfig.WIDTH && _currentY != Config::GeneralConfig.HEIGHT)
  {
#ifdef DEBUG
    refreshTechDemoFooter();
#endif
    _facade->resolutionChange(Config::GeneralConfig.WIDTH, Config::GeneralConfig.HEIGHT);
    _facade->updateRenderData();
    refreshGameInterfaces();
    _currentX = Config::GeneralConfig.WIDTH;
    _currentY = Config::GeneralConfig.HEIGHT;
    dirty     = true;
  }
  if (dirty)
  {
    _elements         = _facade->getShownElementsBoundaries();
    _cachedInterfaces = Config::INTERNAL_UI_COUNT;
  }
}

void InterfaceManager::pollEvents()
{
  std::vector<std::tuple<EventIdentifiers, EventPayload>> events = _eventQueue.drain();

  for (std::tuple<EventIdentifiers, EventPayload> event : events)
  {
    switch (std::get<0>(event))
    {
      case EventIdentifiers::MOUSE_MOVE:
      {
        MousePayload* p = std::get_if<MousePayload>(&std::get<1>(event));
        {
          std::lock_guard lock(_m);
          _currentPos.x = p->x;
          _currentPos.y = p->y;
        }
        std::tuple<std::int16_t, bool> hover = isInterfaceHovered();

        if (std::get<0>(hover) != std::get<0>(_currentHover))
        {
          if (std::get<0>(_currentHover) != -1 && std::get<1>(_currentHover))
          {
            _facade->notify(std::get<0>(_currentHover), 0x02, -1, -1);
          }
          if (std::get<0>(hover) != -1 && std::get<1>(hover))
          {
            _facade->notify(std::get<0>(hover), 0x01, -1, -1);
          }
        }
        _currentHover = hover;
        break;
      }
      case EventIdentifiers::MOUSE_LCLICK:
        if (std::get<0>(_currentHover) != -1 && std::get<1>(_currentHover))
        {
          _facade->notify(std::get<0>(_currentHover), 0x04, -1, -1);
        }
        break;
      case EventIdentifiers::KEY:
      {
        KeyboardPayload* k = std::get_if<KeyboardPayload>(&std::get<1>(event));
        if (k->action != GLFW_PRESS || !InputManager::getInputMap().contains(k->key))
        {
          break;
        }
        std::string action = InputManager::getInputMap()[k->key];
        if (action.compare("OpenMenu") == 0)
        {
          _facade->notify(Config::GeneralInterfaces.SETTING_ID.load(), 0x04, -1, -1);
        }
        else if (action.compare("Info") == 0)
        {
          if (Config::GeneralInterfaces.INFO_ID.load() == -1)
          {
            _facade->createNewInterface("info");
          }
          else
          {
            _facade->destroyInterface(Config::GeneralInterfaces.INFO_ID.load());
            Config::GeneralInterfaces.INFO_ID.store(-1);
          }
        }
        break;
      }
      default:
        break;
    }
  }
}


std::tuple<std::int16_t, bool> InterfaceManager::isInterfaceHovered()
{
  for (ElementBounds* eb : _elements)
  {
    if (_currentPos.x > eb->xLeft && _currentPos.x < eb->xRight && _currentPos.y < eb->yBottom
        && _currentPos.y > eb->yTop)
    {
      if (eb->isContainer)
      {
        continue;
      }
      return std::make_tuple(eb->id, eb->eventDriven);
    }
  }
  return std::make_tuple(-1, false);
}

int InterfaceManager::registerInterface(lua_State* L)
{
  InterfaceManager* me          = (InterfaceManager*)lua_touserdata(L, lua_upvalueindex(1));
  std::vector<ElementProxy*> el = fillInfo(L);
  Container* c                  = new Container(el);
  c->colorR                     = 59.0f;
  c->colorG                     = 58.0f;
  c->colorB                     = 54.0f;
  c->xScreenPosition            = lua_tonumber(L, 2);
  c->yScreenPosition            = lua_tonumber(L, 3);
  c->stretchX                   = lua_tonumber(L, 4);
  c->stretchY                   = lua_tonumber(L, 5);
  c->colorA                     = lua_tonumber(L, 6);
  c->rows                       = lua_tointeger(L, 7);
  c->columns                    = lua_tointeger(L, 8);
  c->hiddenContainer            = lua_toboolean(L, 9);

  std::uint8_t id = me->_facade->createNewInterface(c);
#ifdef DEBUG
  me->logDebug(std::format("{} ### created a new game Interface from lua with id: {}", me->getName(), id));
#endif
  assert(id > 0);
  lua_pushinteger(L, id);
  return 1;
};

std::vector<ElementProxy*> InterfaceManager::fillInfo(lua_State* L)
{
  std::vector<ElementProxy*> result;
  lua_pushnil(L);
  while (lua_next(L, 1) != 0)
  {
    ElementProxy* e = new ElementProxy();
    lua_getfield(L, -1, "type");
    e->type      = (ElementType)lua_tointeger(L, -1);
    e->elementId = Config::incUiId();
    lua_pop(L, 1);

    lua_getfield(L, -1, "position");
    if (lua_istable(L, -1))
    {
      lua_getfield(L, -1, "x");
      e->xPosition = lua_tonumber(L, -1);
      lua_pop(L, 1);
      lua_getfield(L, -1, "y");
      e->yPosition = lua_tonumber(L, -1);
      lua_pop(L, 1);
      lua_getfield(L, -1, "stretchX");
      e->stretchX = lua_tonumber(L, -1);
      lua_pop(L, 1);
      lua_getfield(L, -1, "stretchY");
      e->stretchY = lua_tonumber(L, -1);
      lua_pop(L, 1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "color");
    if (lua_istable(L, -1))
    {
      lua_getfield(L, -1, "r");
      e->colorR = lua_tonumber(L, -1);
      lua_pop(L, 1);

      lua_getfield(L, -1, "g");
      e->colorG = lua_tonumber(L, -1);
      lua_pop(L, 1);

      lua_getfield(L, -1, "b");
      e->colorB = lua_tonumber(L, -1);
      lua_pop(L, 1);

      lua_getfield(L, -1, "a");
      e->colorA = lua_tonumber(L, -1);
      lua_pop(L, 1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "gridPosition");
    if (lua_istable(L, -1))
    {
      lua_getfield(L, -1, "row");
      e->row = lua_tointeger(L, -1);
      lua_pop(L, 1);
      lua_getfield(L, -1, "column");
      e->column = lua_tointeger(L, -1);
      lua_pop(L, 1);
      lua_getfield(L, -1, "count");
      e->columnElements = lua_tointeger(L, -1);
      lua_pop(L, 1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "progress");
    if (!lua_isnil(L, -1))
    {
      e->progress = lua_tointeger(L, -1);
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "text");
    if (!lua_isnil(L, -1))
    {
      e->text       = lua_tostring(L, -1);
      e->givenFlags = 0x0008;
    }
    lua_pop(L, 1);
    result.push_back(e);
    lua_pop(L, 1);
  }
  return result;
}


int InterfaceManager::updateInterfaceValue(lua_State* L)
{
  InterfaceManager* me = (InterfaceManager*)lua_touserdata(L, lua_upvalueindex(1));
  std::uint16_t id     = lua_tointeger(L, 1);
  std::uint8_t event   = lua_tointeger(L, 2);
  std::string value    = lua_tostring(L, 3);
  me->_facade->notify(id, event, value);
  return 0;
};

int InterfaceManager::destroyInterface(lua_State* L)
{
  InterfaceManager* me = (InterfaceManager*)lua_touserdata(L, lua_upvalueindex(1));
  std::uint16_t id     = lua_tointeger(L, 1);
  bool result          = me->_facade->destroyInterface(id);
  lua_pushboolean(L, result);
  return 1;
}

void InterfaceManager::refreshGameInterfaces()
{
  LuaFacade::getLuaFacadeInstance().onUIRefresh(0);
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
