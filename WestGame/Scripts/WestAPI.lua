local playerB

local sep = package.config:sub(1, 1) -- "/" or "\"
local root = debug.getinfo(1, 'S').source:sub(2):gsub("[^" .. sep .. "]+$", "")

-- Add root and all subdirs to package.path, needed for all Folders from within Scipts
package.path = root .. "?.lua;" .. root .. "Player/?.lua;" .. package.path


function Init()
  playerB = require("PlayerBehaviour")
end

function Worldpos_lclick(id, x, y, z)
  print("clicked pos" .. x .. y .. z);
  local state = playerB.getCurrentState();
  if state == playerB.IDLE then
    playerB.setState(playerB.MOVING);
    MoveCurPlayer(id, x, y, z);
  end
end

function StateChange(id, state)
  print(playerB.getCurrentState());
  print(state);
  playerB.setState(state);
end
