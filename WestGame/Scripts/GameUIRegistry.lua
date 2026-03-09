UIManager = {}

local registeredUIs = {};
local additionalInfo = {}

local root = debug.getinfo(1, 'S').source:sub(2):gsub("[^/]+$", "")

function RegisterUI(id, ui, info)
  registeredUIs[id] = ui;
  additionalInfo[id] = info;
end

function UnregisterUI(id)
  registeredUIs[id] = nil;
  additionalInfo[id] = nil;
end

function UpdateUIState(id, ui, info)
  registeredUIs[id] = ui;
  additionalInfo[id] = info;
end

function GetUIs()
  return { registeredUIs, additionalInfo };
end

UIManager.register = RegisterUI;
UIManager.unregister = UnregisterUI;
UIManager.update = UpdateUIState;
UIManager.get = GetUIs;

return UIManager;
