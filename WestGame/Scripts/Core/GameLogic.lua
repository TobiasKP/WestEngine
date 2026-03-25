local Logic = {}

local interface = require("Interface.InterfaceLogic")

local switch = {
  ["closeinterface"] = function(id)
    interface.destroyInterface(id);
  end
}


function RclickEntity(id)
  --TODO get Health from component and display in UI
  local health = getHealth(id);
  local screenX, screenY = getPosition(id);
  print(health .. ":" .. screenX .. "-" .. screenY)
end

function LclickEntity(id)
  print(id)
end

function Internal(toCall, id)
  if switch(toCall) then
    switch[toCall](id)
  else
    print("Error calling " .. toCall .. " not supported")
  end
end

Logic.RclickEntity = RclickEntity
Logic.LclickEntity = LclickEntity
Logic.Internal = Internal

return Logic
