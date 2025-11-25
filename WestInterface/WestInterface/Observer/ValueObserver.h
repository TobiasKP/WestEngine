#pragma once

#include "IObserver.h"

class ValueObserver : public IObserver
{
public:
  ValueObserver(WestLogger& l) : IObserver(l) {};
  ~ValueObserver() {};
};
