local entitiyList = { "../Entities/Player/Cube", "../Entities/Npc/EnemyCube" }
local w = "../Entities/World/DemoWorld"

local utils = require("Core.EntityUtils")

IntroScene = {}

function Load()
  utils.World(w)
  for _, entity in ipairs(entitiyList) do
    local result = utils.LoadEntity(entity, nil)
    if result == 1 then
      westLog(LogLevel.Error, "Error loading entity in intro scene: " .. tostring(entity))
    end
  end
end

IntroScene.load = Load

return IntroScene
