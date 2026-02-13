function Worldpos_lclick(id, x, y, z)
  print("clicked pos" .. x .. y .. z);
  MoveCurPlayer(id, x, y, z);
end
