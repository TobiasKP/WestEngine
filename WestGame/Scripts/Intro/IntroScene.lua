local entitiyList = { "Cube", "EnemyCube" }
local w = "DemoWorld"

local utils = require("EntityUtils")

IntroScene = {}

function load()
  utils.World(w)
  for _, entity in ipairs(entitiyList) do
    local result = utils.LoadEntity(entity)
    if result == 1 then
      print("Error loading Entity")
    end
  end
end

IntroScene.load = load

return IntroScene
