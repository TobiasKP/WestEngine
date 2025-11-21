#include "ElementFactory.h"

#include "../../RenderManagment/TextRenderManager.h"
#include "../Umbrella.hpp"

#include <stb_image.h>
#include <unordered_map>

IElement* ElementFactory::createElementInternal(ElementProxy* e)
{
  ElementType type = e->type;
  IElement* result = nullptr;

  switch (type)
  {
    case BUTTON:
      result = new Button();
      fillBasicInfos(e, result);
      fillText(e, result);
      registerElementEvent(e, result);
      break;
    case LABEL:
      result = new Label();
      fillBasicInfos(e, result);
      fillText(e, result);
      break;
    case CONTAINER:
      result = new ContainerElement();
      fillBasicInfos(e, result);
      break;
    case ICON:
      // result = new Icon();
      fillBasicInfos(e, result);
      break;
    case DEBUG_ELEMENT:
      result = new DebugElement();
      fillBasicInfos(e, result);
      fillText(e, result);
      break;
    default:
      result = new DebugElement();
      fillBasicInfos(e, result);
      break;
  }

  assert(result != nullptr);
  if (e->texture != nullptr)
  {
    fillTexture(e, result);
  }

  if (result->zIndex == -1)
  {
    result->zIndex = 2;
  }

  return result;
}

void ElementFactory::fillTexture(ElementProxy* ep, IElement* el)
{
  GLuint texture;
  glGenTextures(1, &texture);

  std::int32_t width, height, numComponents;
  const char* textureFile = ep->texture->path;
  char cwd[128];
  char filePath[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL)
  {
    return;
  }

  snprintf(filePath, sizeof(filePath), "%s%s%s", cwd, "/", textureFile);

  unsigned char* imgData = stbi_load(filePath, &width, &height, &numComponents, 0);
  if (imgData == NULL)
  {
    return;
  }

  glBindTexture(GL_TEXTURE_2D, texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage3D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(imgData);
  el->texture = texture;
}

void ElementFactory::fillText(ElementProxy* ep, IElement* el)
{
  if (ep->text.size() == 0)
  {
    return;
  }

  Text* t      = new Text();
  t->plaintext = ep->text;

  t->coordinates.reserve(ep->text.size() * 4);
  t->positions.reserve(ep->text.size());
  t->positions = TextRenderManager::calculateTextPositions(ep->text, el->stretchX);

  for (char c : ep->text)
  {
    std::array<float, 4> coords = getTextureCoordinatesForChar(c);
    t->coordinates.insert(t->coordinates.end(), coords.begin(), coords.end());
  }

  assert(t->coordinates.size() == ep->text.size() * 4);
  assert(t->positions.size() == ep->text.size());
  el->flags |= 0x08;
  el->text   = std::unique_ptr<Text>(t);
}

void ElementFactory::registerElementEvent(ElementProxy* ep, IElement* e)
{
  e->eventHandler   = ep->eventHandler;
  e->supportsEvents = true;
  e->zIndex         = 5;
  _eObserver->registerElement(e);
}

void ElementFactory::registerElementValue(ElementProxy* ep, IElement* e)
{
  e->supportsEvents = true;
  e->zIndex         = 5;
  _vObserver->registerElement(e);
}

void ElementFactory::fillBasicInfos(ElementProxy* ep, IElement* el)
{
  el->id              = ep->elementId;
  el->xLL             = ep->xPosition;
  el->yLL             = ep->yPosition;
  el->stretchX        = ep->stretchX;
  el->stretchY        = ep->stretchY;
  el->colorR          = ep->colorR / 255.0f;
  el->colorG          = ep->colorG / 255.0f;
  el->colorB          = ep->colorB / 255.0f;
  el->colorA          = ep->colorA;
  el->rowElements     = ep->rowElements;
  el->columnElements  = ep->columnElements;
  el->eventHandler    = ep->eventHandler;
  el->zIndex          = ep->zIndex;
  el->flags          |= ep->givenFlags;
}

std::array<float, 4> ElementFactory::getTextureCoordinatesForChar(char character)
{
  const std::unordered_map<char, TextRenderManager::GlyphData>& res = TextRenderManager::getGlyphCache();

  assert(character >= 32 && character <= 126 && res.size() > 0);

  auto it = res.find(character);
  if (it != res.end())
  {
    return it->second.textureCoords;
  }

  auto fallback = res.find(' ');
  if (fallback != res.end())
  {
    return fallback->second.textureCoords;
  }

  return std::array<float, 4>{0};
}
