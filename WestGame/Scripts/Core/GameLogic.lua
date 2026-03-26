local Logic = {}

local interface = require("Interface.InterfaceLogic")
local builder = require("Interface.InterfaceBuilder")

local switch = {
  ["closeinterface"] = function(id)
    interface.DestroyInterface(id);
  end
}


function RclickEntity(id)
  local health = getHealth(id);
  local screenX, screenY = getPosition(id);
  builder.Panel("npcinfo")
      :anchor("none", screenX, screenY)
      :size(5, 5)
      :alpha(0.5)
      :add(builder.Button("X")
        :handler("closeinterface")
        :color(142, 59, 70, 1.0)
        :span(1)
        :grid(4, 4))
      :add(builder.ProgressBar()
        :color(142, 59, 70, 1.0)
        :span(5)
        :progress(health)
        :grid(2, 0))
      :build()
  print(health .. ":" .. screenX .. "-" .. screenY)
end

function LclickEntity(id)
  print(id)
end

function Internal(toCall, id)
  if switch[toCall] then
    switch[toCall](id)
  else
    print("Error calling " .. toCall .. " not supported")
  end
end

Logic.RclickEntity = RclickEntity
Logic.LclickEntity = LclickEntity
Logic.Internal = Internal

return Logic
