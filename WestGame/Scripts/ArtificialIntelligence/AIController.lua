local state = require("Core.GameState")
local utils = require("Utils.Utils")

local AIController = {}

function Execute(entityId)
  state.TransitionEntityState(entityId, StateMachine.GetEntityState(entityId), StateMachine.EntityStates.IDLE)
  local points = state.EntityHasActionPointsLeft(entityId)
  if points == 0 then
    print("GGG - ending AI " .. entityId)
    if state.AIFinished() == true then
      print("GGG - ending AI Turn")
      aiEndAction(true)
      state.ToggleGameState();
    else
      aiEndAction(false)
    end
    return;
  end

  print("GGG - Gather information");
  local info = gatherWorldInformation(entityId)
  local tiles = info.TilesInRange
  local enemies = info.EnemiesInRange
  if points > 1 or utils.TableLength(enemies) == 0 then
    if utils.TableLength(tiles) == 0 then
      print("GGG - no tiles in Range for entity " .. entityId .. " something went wrong")
      return
    end
    print("GGG - AI " .. entityId .. " moving to " .. tiles[1])
    aiMoveCommand(entityId, tiles[1])
    state.TransitionEntityState(entityId, StateMachine.GetEntityState(entityId), StateMachine.EntityStates.MOVING)
  else
    print("GGG - AI " .. entityId .. " attacking " .. enemies[1])
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
