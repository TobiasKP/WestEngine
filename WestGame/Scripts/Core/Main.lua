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
  local height, width = getScreenResolution();
  builder.Panel("npcinfo")
      :anchor("none", width / 2, height - 80)
      :size(2, 2)
      :alpha(1.0)
      :add(builder.Button("E")
        :handler("endturn")
        :color(215, 207, 196, 1.0)
        :span(1)
        :grid(1, 1))
      :build()
end

function Init()
  intro = require("Scenes.IntroScene")
  builder = require("Interface.InterfaceBuilder")
end
