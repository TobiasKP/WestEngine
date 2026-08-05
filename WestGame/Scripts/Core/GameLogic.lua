local Logic = {}

local interface = require("Interface.InterfaceLogic")
local state = require("Core.GameState")
local entity = require("Core.EntityUtils")

local switch = {
  ["closeinterface"] = function(id)
    interface.DestroyInterface(id);
  end,
  ["endturn"] = function(id)
    state.ToggleGameState()
    interface.DestroyInterface(id);
    ExecuteAI()
  end
}

function RclickEntity(id)
  interface.ConstructInfoPanel(id)
end

function Attack(bulletType, wRange, distanceToTarget, wDmg, wAccuracy, target, spawnX, spawnY)
  if wRange < distanceToTarget then
    wAccuracy = wAccuracy - (distanceToTarget - wRange) * 10
  end
  local ran = math.random(0, 100);
  local hit = true
  if ran > wAccuracy then
    hit = false
  end
  if bulletType == 1 then
    entity.FillProjectileInfo(2.0, target, wDmg, hit, spawnX, spawnY)
    entity.QueueEntity("../Entities/Misc/SmallProjectile")
  elseif bulletType == 2 then
    entity.FillProjectileInfo(4.0, target, wDmg, hit, spawnX, spawnY)
    entity.QueueEntity("../Entities/Misc/MediumProjectile")
  else
    print("Error defining projectile, dismissed call...")
  end
end

function ExecuteAI()

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
