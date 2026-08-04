local intro
local builder

function LoadScene(name)
  if name == nil then
    return nil
  end

  if name == "Intro" then
    intro.load()
  end

  EndTurnButton()
end

function EndTurnButton()
  builder.Panel("endturn")
      :anchor("bottom-middle", 0, 80)
      :size(1, 1)
      :alpha(1.0)
      :add(builder.Button("E")
        :handler("endturn")
        :color(215, 207, 196, 1.0)
        :span(1)
        :grid(0, 0))
      :build()
end

function Init()
  intro = require("Scenes.IntroScene")
  builder = require("Interface.InterfaceBuilder")
end
