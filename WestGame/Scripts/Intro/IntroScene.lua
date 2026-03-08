local entitiyList = { "Cube" }
local w = "DemoWorld"

local utils = require("Utils")

IntroScene = {}

function load()
  for _, entity in ipairs(entitiyList) do
    local result = utils.LoadEntity(entity)
    if result == 1 then
      print("Error loading Entity")
    end
  end
  utils.World(w)
end

IntroScene.load = load

return IntroScene
