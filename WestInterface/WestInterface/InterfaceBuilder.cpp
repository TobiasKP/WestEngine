#include "InterfaceBuilder.h"

#include <Config.h>
#include <format>
#include <PathUtils.h>
#include <stb_image.h>

using namespace WestInterface;

InterfaceBuilder::InterfaceBuilder()
{
  _factory  = nullptr;
  _dataPool = nullptr;
}

InterfaceBuilder::InterfaceBuilder(EventObserver* eo, ValueObserver* vo)
{
  _vObserver = vo;
  _eObserver = eo;
  _factory   = new ElementFactory(eo, vo);
  _dataPool  = new ComponentDataPool();
}

InterfaceBuilder::~InterfaceBuilder()
{
  if (_current != nullptr)
  {
    _logger.log(Level::Error, "@@@ Interface still in build process but Builder was destroyed.\n");
    delete _current;
  }
  delete _factory;
  delete _dataPool;
}

void InterfaceBuilder::createNewInterface(Container* c)
{
  _current = new ContainerElement();
  assert(_current != nullptr);
  _current->poolPosition = _dataPool->reserveNew(c->rows * c->columns);
  _dataPool->setPositionUpdateCallback([this](std::uint32_t oldPos, std::uint32_t newPos)
                                       { _current->updatePositions(oldPos, newPos); });
  _current->stretchX        = c->stretchX;
  _current->stretchY        = c->stretchY;
  _current->xLL             = c->xScreenPosition;
  _current->yLL             = c->yScreenPosition;
  _current->rowElements     = c->rows;
  _current->columnElements  = c->columns;
  _current->colorR          = c->colorR / 255.0;
  _current->colorG          = c->colorG / 255.0;
  _current->colorB          = c->colorB / 255.0;
  _current->colorA          = c->colorA;
  _current->id              = Config::incUiId();
  _current->dataPool        = _dataPool;
  _current->flags          |= c->givenFlags;
  assert(_current->xLL >= 0 && _current->yLL >= 0); 

#ifdef DEBUG
  _logger.log(Level::Info,
              std::format("@@@ Creating new Interface -> {} : {}\n", c->xScreenPosition, c->yScreenPosition));
#endif
}

void InterfaceBuilder::createBackground(TextureInformation* t)
{
  GLuint texture;
  glGenTextures(1, &texture);

  std::int32_t width, height, numComponents;
  std::string filePath = PathUtils::resolve(std::string("/") + t->path);
  stbi_set_flip_vertically_on_load(true);
  unsigned char* imgData = stbi_load(filePath.c_str(), &width, &height, &numComponents, 0);
  if (imgData == NULL)
  {
    _logger.log(Level::Error, "@@@ GNA.\n");
    return;
  }

  GLenum format;
  switch (numComponents)
  {
    case 1:
      format = GL_RED;
      break;
    case 3:
      format = GL_RGB;
      break;
    case 4:
      format = GL_RGBA;
      break;
    default:
      _logger.log(Level::Error, "@@@ Unsupported image format\n");
      return;
  }

  glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, width, height / 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
  glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, t->wrapping_x);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, t->wrapping_y);
  stbi_set_flip_vertically_on_load(false);
  stbi_image_free(imgData);
  _current->texture  = texture;
  _current->flags   |= 0x04;
}

void InterfaceBuilder::addElement(ElementProxy* e)
{
  assert(e != nullptr);
  IElement* newElement = transform(e);
  assert(newElement != nullptr);

  newElement->poolPosition = _dataPool->reserveNew(newElement->rowElements * newElement->columnElements);
  if (newElement->poolPosition == -1)
  {
    _logger.log(Level::Error, "@@@ Error reserving size for new Element do not add Element.\n");
    return;
  }
  _current->addChild(newElement, e->column, e->row);
}

ContainerElement* InterfaceBuilder::build()
{
  assert(_current != nullptr);
  ContainerElement* result = _current;
  _current                 = nullptr;
#ifdef DEBUG
  _logger.log(Level::Info, "@@@ Interface build complete returning Parent container\n");
#endif
  return result;
}

IElement* InterfaceBuilder::transform(ElementProxy* e)
{
  assert(e != nullptr);
  IElement* result = _factory->createElement(e);
  assert(result != nullptr);
  return result;
}
