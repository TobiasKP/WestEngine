local intro

function LoadScene(name)
  if name == nil then
    return nil
  end

  if name == "Intro" then
    intro.load()
  end
end

function Init()
  intro = require("Scenes.IntroScene")
end
