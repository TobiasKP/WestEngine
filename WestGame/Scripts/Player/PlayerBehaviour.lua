local C = {}

C.IDLE = 0;
C.MOVING = 1;
local _current = 0;

function C.getCurrentState()
  return _current;
end

function C.setState(state)
  _current = state;
end

return C
