local Logger = {}

function Init()
  local logfile = io.open("LuaLog.txt", "w");
  io.output(assert(logfile));
end

Logger.Init = Init

return Logger
