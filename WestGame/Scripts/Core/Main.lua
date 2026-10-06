local intro
local logic

function LoadScene(name)
  if name == nil then
    return nil
  end

  if name == "Intro" then
    intro.load()
  end

  logic.EndTurnButton()
end

function Init()
  intro = require("Scenes.IntroScene")
  logic = require("Interface.InterfaceLogic")
end
