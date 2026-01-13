local intro

local sep = package.config:sub(1,1) -- "/" or "\"
local root = debug.getinfo(1, 'S').source:sub(2):gsub("[^"..sep.."]+$", "")

-- Add root and all subdirs to package.path, needed for all Folders from within Scipts
package.path = root .. "?.lua;" .. root .. "Intro/?.lua;" .. package.path


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
   intro = require("IntroScene")
end
