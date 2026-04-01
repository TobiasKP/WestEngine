local Logic = {}

local interface = require("Interface.InterfaceLogic")
local builder = require("Interface.InterfaceBuilder")
local state = require("Core.GameState")
local entity = require("Core.EntityUtils")

local switch = {
  ["closeinterface"] = function(id)
    interface.DestroyInterface(id);
  end,
  ["endturn"] = function()
    state.ToggleGameState()
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
        :attachEntity(id)
        :grid(5, 2)
        :flag(0x01))
      :build()
end

function Attack(wRange, distanceToTarget, wDmg, wAccuracy, target, spawnX, spawnY)
  if wRange < distanceToTarget then
    wAccuracy = wAccuracy - (distanceToTarget - wRange) * 10
  end
  local ran = math.random(0, 100);
  local hit = true
  if ran > wAccuracy then
    hit = false
  end
  entity.FillProjectileInfo(2.0, target, wDmg, hit, spawnX, spawnY)
  entity.QueueEntity("../Entities/Misc/SmallProjectile")
end

function Internal(toCall, id)
  if switch[toCall] then
    switch[toCall](id)
  else
    print("Error calling " .. toCall .. " not supported")
  end
end

Logic.RclickEntity = RclickEntity
Logic.Attack = Attack
Logic.Internal = Internal

return Logic
