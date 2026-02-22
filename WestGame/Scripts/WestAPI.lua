local playerB

function Init()
  playerB = require("Player/PlayerBehaviour")
end

function Worldpos_lclick(id, x, y, z)
  if DEBUG then
    print("clicked pos" .. x .. y .. z);
  end
  local state = playerB.getCurrentState();
  if state == playerB.IDLE then
    playerB.setState(playerB.MOVING);
    MoveCurPlayer(id);
  end
end

function StateChange(id, state)
  if DEBUG then
    print(playerB.getCurrentState());
    print(state);
  end
  playerB.setState(state);
  if (state == playerB.IDLE) then
    ActionFinished(id);
  end
end
