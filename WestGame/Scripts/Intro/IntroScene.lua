local entitiyList = { "Cube", "DemoWorld" }

local Entities = {}

local utils = require("Utils")

IntroScene = {}

function load()
  for _, entity in ipairs(entitiyList) do
    local result = utils.LoadEntity(entity)
    if result == 1 then
      print("Error loading Entity")
      -- TODO bring to Stack for cpp error log
    else
      table.insert(Entities, result)
    end
  end
  return Entities
end

IntroScene.load = load

return IntroScene
