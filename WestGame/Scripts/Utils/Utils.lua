local Utils = {}

function TableLength(T)
  local count = 0
  if T == nil then return count end
  for _ in pairs(T) do count = count + 1 end
  return count
end

Utils.TableLength = TableLength

return Utils
