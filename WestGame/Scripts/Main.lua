local intro

function LoadScene(name)
  if name == nil then
    return nil
  end

  if name == "Intro" then
    print("loading Intro")
    return intro.load()
  end
end

function Init()
   intro = require("Intro/IntroScene")
end
