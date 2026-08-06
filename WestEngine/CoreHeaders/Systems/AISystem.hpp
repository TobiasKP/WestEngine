#pragma once


#include "../../Core/Scripting/LuaFacade.hpp"
#include "../Interfaces/ISystem.h"

class AISystem : public ISystem
{
public:
  AISystem(std::shared_ptr<EventDispatcher> d, WestLogger* logger, std::shared_ptr<ComponentRegistry> r);
  ~AISystem() override;

  void update() override;
  void updateDebuggingInfo() override;
  void init(const std::shared_ptr<World>& w) override;

private:
  void handleEvent(std::tuple<EventIdentifiers, EventPayload> event) override;


  bool isAIturn() const
  {
    return _state == 1;
  }

  std::int32_t _state;
  std::uint32_t _me;
  std::shared_ptr<World> _world;
  std::shared_ptr<ComponentRegistry> _registry;
  std::vector<std::uint32_t> _handled;
  bool _busy;
  LuaFacade* _facade;

  static int gatherWorldInformation(lua_State* l);
  static int aiMoveCommand(lua_State* l);
  static int aiAttackCommand(lua_State* l);
  static int aiEndAction(lua_State* l);
};
