local uimanager = require("Interface.GameUIRegistry")

local Panel = {}
Panel.__index = Panel

---@class Panel
function Panel.new(parent)
  return setmetatable({
    _parent = parent,
    _children = {},
    _anchor = nil,
    _offsetX = 0,
    _offsetY = 0,
    _stretchX = 1,
    _stretchY = 1,
    _alpha = 1.0,
    _rows = 1,
    _columns = 1,
    _hidden = false,
  }, Panel)
end

function Panel:anchor(position, offsetX, offsetY)
  self._anchor = position
  self._offsetX = offsetX or 0
  self._offsetY = offsetY or 0
  return self
end

function Panel:size(sx, sy)
  self._stretchX = sx
  self._stretchY = sy
  return self
end

function Panel:alpha(a)
  self._alpha = a
  return self
end

function Panel:add(element)
  table.insert(self._children, element)
  return self
end

function Panel:dimension(r, c)
  self._rows = r
  self._columns = c
  return self
end

function Panel:isHidden(h)
  self._hidden = h
  return self
end

function Panel:build()
  local h, w = getScreenResolution()
  local x, y = self:_resolveAnchor(w, h)

  local elements = {}
  for _, child in ipairs(self._children) do
    local element = child:toElement()
    element.position.x = x
    element.position.y = y
    table.insert(elements, element)
  end

  local id = createInterface(elements, x, y, self._stretchX, self._stretchY, self._alpha, self._rows, self._columns,
    self._hidden)
  local this = self
  uimanager.register(id, elements, {
    function(rw, rh) return this:_resolveAnchor(rw, rh) end,
    self._stretchX,
    self._stretchY,
    self._alpha,
    self._rows,
    self._columns,
    parent = self._parent
  })

  return id
end

function Panel:_resolveAnchor(w, h)
  local anchors = {
    ["top-right"]    = function() return w - self._offsetX, h + self._offsetY end,
    ["top-left"]     = function() return self._offsetX, h + self._offsetY end,
    ["bottom-right"] = function() return w - self._offsetX, self._offsetY end,
    ["bottom-left"]  = function() return self._offsetX, self._offsetY end,
    ["center"]       = function() return w / 2 + self._offsetX, h / 2 + self._offsetY end,
    ["none"]         = function() return self._offsetX, self._offsetY end,
  }
  local fn = anchors[self._anchor]
  if fn then return fn() end
  return self._offsetX, self._offsetY
end

------- Elements ------

---@class Label
---@field color fun(self, r:  number, g: number, b: number, a?: number) : Label
---@field span fun(self, n: number) : Label
---@field stretch fun(self, x: number, y: number ): Label
---@field _type number
---@field _text string | nil
---@field _attachedEntity number | nil
local Label = {}
Label.__index = Label

function Label.new(text)
  return setmetatable({
    _type = 0,
    _text = text or "",
    _color = { r = 255, g = 255, b = 255, a = 1.0 },
    _span = 1,
    _grid = { row = 0, column = 0 },
    _stretch = { x = 1, y = 1 },
    _flags = 0,
    _attachedEntity = 0,
  }, Label)
end

function Label:attachEntity(id)
  self._attachedEntity = id
  return self
end

function Label:flag(flag)
  self._flags = flag | self._flags
  return self
end

function Label:color(r, g, b, a)
  self._color = { r = r, g = g, b = b, a = a or 1.0 }
  return self
end

function Label:span(n)
  self._span = n
  return self
end

function Label:grid(r, c)
  self._grid = { row = r, column = c }
  return self
end

function Label:stretch(x, y)
  self._stretch = { x = x, y = y }
  return self
end

function Label:toElement()
  return {
    type = self._type,
    position = { x = 0, y = 0, stretchX = self._stretch.x, stretchY = self._stretch.y },
    color = self._color,
    gridPosition = { row = self._grid.row, column = self._grid.column, count = self._span },
    text = self._text,
    flags = self._flags,
    attachedEntity = self._attachedEntity
  }
end

---@class ProgressBar : Label
---@field _progress number
local ProgressBar = setmetatable({}, { __index = Label })
ProgressBar.__index = ProgressBar

function ProgressBar.new(progress)
  local self = Label.new("")
  self._type = 7
  self._text = nil
  self._progress = progress or 0
  return setmetatable(self, ProgressBar)
end

function ProgressBar:progress(n)
  self._progress = n
  return self
end

function ProgressBar:toElement()
  local elem = Label.toElement(self)
  elem.progress = self._progress
  elem.text = nil
  return elem
end

---@class Button : Label
---@field _handler string | nil
local Button = setmetatable({}, { __index = Label })
Button.__index = Button

function Button.new(text)
  local self = Label.new(text)
  self._type = 2
  self._handler = nil
  return setmetatable(self, Button)
end

function Button:handler(func)
  self._handler = func
  return self
end

function Button:toElement()
  local elem = Label.toElement(self)
  elem.handler = self._handler
  return elem
end

return { Panel = Panel.new, Label = Label.new, ProgressBar = ProgressBar.new, Button = Button.new }
