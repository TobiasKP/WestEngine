local entitiyList = { "../Entities/Player/Cube", "../Entities/Npc/EnemyCube" }
local w = "../Entities/World/DemoWorld"

local utils = require("Core.EntityUtils")

IntroScene = {}

function load()
  utils.World(w)
  for _, entity in ipairs(entitiyList) do
    local result = utils.LoadEntity(entity, nil)
    if result == 1 then
      print("Error loading Entity")
    end
  end
end

IntroScene.load = load

return IntroScene
