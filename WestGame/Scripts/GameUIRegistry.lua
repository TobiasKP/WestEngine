UIManager = {}

local uiIdToParent = {};
local registeredUIs = {};
local additionalInfo = {}

--local root = debug.getinfo(1, 'S').source:sub(2):gsub("[^/]+$", "")

function RegisterUI(id, ui, info)
  registeredUIs[id] = ui;
  additionalInfo[id] = info;
  if uiIdToParent[info.parent] == nil then
    uiIdToParent[info.parent] = {}
  end
  table.insert(uiIdToParent[info.parent], id);
end

function UnregisterUI(id)
  local parent = additionalInfo[id].parent;
  registeredUIs[id] = nil;
  additionalInfo[id] = nil;
  for i, uiId in ipairs(uiIdToParent[parent]) do
    if uiId == id then
      table.remove(uiIdToParent[parent], i)
      break
    end
  end
  if #uiIdToParent[parent] == 0 then
    uiIdToParent[parent] = nil
  end
end

function UpdateUIState(id, ui, info)
  registeredUIs[id] = ui;
  additionalInfo[id] = info;
end

function GetUIs()
  return { registeredUIs, additionalInfo };
end

function GetUiByParent(parent)
  return uiIdToParent[parent];
end

UIManager.register = RegisterUI;
UIManager.unregister = UnregisterUI;
UIManager.update = UpdateUIState;
UIManager.get = GetUIs;
UIManager.getByParent = GetUiByParent;

return UIManager;
