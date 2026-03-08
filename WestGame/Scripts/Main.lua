local intro

function LoadScene(name)
  if name == nil then
    return nil
  end

  if name == "Intro" then
    if DEBUG then
      print("loading Intro")
    end
    intro.load()
  end
end

function Init()
  intro = require("Intro/IntroScene")
end
