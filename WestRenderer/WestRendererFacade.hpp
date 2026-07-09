#pragma once

class WestRendererFacade
{
public:
  static WestRendererFacade& getRendererFacade();

  void clearColor() {};
  void renderEntity() {};
  void renderInterface() {};
  void renderWorld() {};
  void renderDebugEntities() {};
};
