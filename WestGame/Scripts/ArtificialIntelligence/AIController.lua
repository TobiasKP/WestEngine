local state = require("Core.GameState")
local utils = require("Utils.Utils")

local AIController = {}

function Execute(entityId)
  state.TransitionEntityState(entityId, StateMachine.GetEntityState(entityId), StateMachine.EntityStates.IDLE)
  local points = state.EntityHasActionPointsLeft(entityId)
  if points == 0 then
    westLog(LogLevel.Info, "AI entity " .. tostring(entityId) .. " has no action points left, ending its actions")
    if state.AIFinished() == true then
      westLog(LogLevel.Info, "All AI entities finished, ending AI turn")
      aiEndAction(true)
      state.ToggleGameState();
    else
      aiEndAction(false)
    end
    return;
  end

  westLog(LogLevel.Cycle,
    "AI entity " .. tostring(entityId) .. " gathering world information (" .. tostring(points) ..
    " action points left)")
  local info = gatherWorldInformation(entityId)
  local tiles = info.TilesInRange
  local enemies = info.EnemiesInRange
  if points > 1 or utils.TableLength(enemies) == 0 then
    if utils.TableLength(tiles) == 0 then
      westLog(LogLevel.Error, "No tiles in range for AI entity " .. tostring(entityId) .. ", cannot move")
      return
    end
    westLog(LogLevel.Info, "AI entity " .. tostring(entityId) .. " moving to tile " .. tostring(tiles[1]))
    if aiMoveCommand(entityId, tiles[1]) == false then
      return
    end
    state.TransitionEntityState(entityId, StateMachine.GetEntityState(entityId), StateMachine.EntityStates.MOVING)
  else
    westLog(LogLevel.Info, "AI entity " .. tostring(entityId) .. " attacking entity " .. tostring(enemies[1]))
    aiAttackCommand(entityId, enemies[1])
    state.TransitionEntityState(entityId, StateMachine.GetEntityState(entityId), StateMachine.EntityStates.ATTACKING)
  end
end

AIController.execute = Execute

return AIController



--// AI Information
--constexpr std::string_view C_GET_WORLD_INFO = "gatherWorldInformation";
--constexpr std::string_view C_AI_MOVE        = "aiMoveCommand";
--constexpr std::string_view C_AI_ATTACK      = "aiAttackCommand";
--constexpr std::string_view C_AI_END         = "aiEndAction";
