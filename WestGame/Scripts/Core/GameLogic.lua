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
      :size(7, 7)
      :alpha(0.8)
      :add(builder.Button("X")
        :handler("closeinterface")
        :color(215, 207, 196, 1.0)
        :span(1)
        :grid(6, 6))
      :add(builder.Label("HP")
        :span(2)
        :grid(5, 0)
        :color(215, 207, 196, 1.0))
      :add(builder.ProgressBar()
        :color(142, 59, 70, 1.0)
        :span(4)
        :progress(health)
        :grid(5, 2)
        :flag(0x01))
      :build()
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
