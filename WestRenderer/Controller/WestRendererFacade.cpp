#include "../WestRendererFacade.hpp"


WestRendererFacade& WestRendererFacade::getRendererFacade()
{
  static WestRendererFacade instance;
  return instance;
};
