local intro
local logic
local logger

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
  logger = require("Utils.LuaLogger")
  logger.Init();
end
