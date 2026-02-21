local playerB

function Init()
  playerB = require("Player/PlayerBehaviour")
end

function Worldpos_lclick(id, x, y, z)
  print("clicked pos" .. x .. y .. z);
  local state = playerB.getCurrentState();
  if state == playerB.IDLE then
    playerB.setState(playerB.MOVING);
    MoveCurPlayer(id);
  end
end

function StateChange(id, state)
  print(playerB.getCurrentState());
  print(state);
  playerB.setState(state);
  if (state == playerB.IDLE) then
    ActionFinished(id);
  end
end
