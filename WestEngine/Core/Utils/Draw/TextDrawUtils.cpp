// #include "../../../CoreHeaders/Utils/Draw/TextDrawUtils.h"

// #include <../../../Libs/GLM/ext/matrix_clip_space.hpp>

// #include "../../../CoreHeaders/RenderManager.h"
// #include "../../../CoreHeaders/Utils/DataUtils/UniformUtils.h"
/*#include "../../../Globals/Globals.h"

// TODO: refactor
TextDrawUtils::TextDrawUtils() {
  _loader = new ObjectLoader(&WestLogger::getLoggerInstance());
  float vertices[12] = {0.0f, (float)Global::HEIGHT - 50,
                        0.0f, 50.0f,
                        (float)Global::HEIGHT - 50, 0.0f,
                        50.0f, (float)Global::HEIGHT,
                        0.0f, 0.0f,
                        (float)Global::HEIGHT, 0.0f};
  std::int32_t indices[6] = {0, 1, 2, 0, 2, 3};
  Model *m = _loader->loadModel(vertices, sizeof(vertices), indices,
                                sizeof(indices), 0, 0, 0, 0);
  Texture *t = new Texture();
  t->id = _loader->loadTexture(CoreConstants::TEXT_BITMAP);

  m->texture = t;
  Text *text = new Text();

  Shader *s = new Shader();
  //s->vertexShaderFile = (char *)CoreConstants::TEXT_V_SHADER;
  //s->fragShaderFile = (char *)CoreConstants::TEXT_F_SHADER;
  s->shadergroup = CoreConstants::TEXT_SHADERGROUP;

  _textGlyphs = new Entity(Global::INTERNAL_ENTITY_ID++);
  _textGlyphs->addComponent(BitMasks::Components::SHADER, s);
  _textGlyphs->addComponent(BitMasks::Components::MODEL, m);
  _textGlyphs->addComponent(BitMasks::Components::TEXT, text);
}

void TextDrawUtils::createGlyph(const char c) {
  std::int32_t charIndex = static_cast<std::int32_t>(c);
  std::int32_t col = charIndex % _columns;
  std::int32_t row = charIndex / _rows - 1;

  GLfloat left = static_cast<GLfloat>(col * _charWidth) / _bitmapWidth;
  GLfloat right = static_cast<GLfloat>((col + 1) * _charWidth) / _bitmapWidth;

  GLfloat top = static_cast<GLfloat>((row - 1) * _charHeight) / _bitmapHeight;
  GLfloat bottom = static_cast<GLfloat>(row * _charHeight) / _bitmapHeight;

  std::vector<GLfloat> coords = {left,  top,    left,  bottom,
                                 right, bottom, right, top};

  _bitmapCoords.emplace_back(coords);
  coords.clear();
}

void TextDrawUtils::setTextShaderProgram() {
  Shader *s = (Shader *)_textGlyphs->getComponent(BitMasks::Components::SHADER);
  Text *t = (Text *)_textGlyphs->getComponent(BitMasks::Components::TEXT);
  GLuint shaderId = s->programId;
  glUseProgram(shaderId);
  glm::mat4 ortho =
      glm::ortho(0.0f, (float)Global::WIDTH, 0.0f, (float)Global::HEIGHT);
  //UniformUtils::setUniform(t->orthoUniform, ortho);
}

void TextDrawUtils::resetTextShaderProgram() {
  glUseProgram(RenderManager::getUsedShaderProgram());
}

void TextDrawUtils::renderText(size_t length, glm::vec2 screenPosition) {
  Text *t = (Text *)_textGlyphs->getComponent(BitMasks::Components::TEXT);
  for (std::int32_t i = 0; i < length; i++) {
    char c = t->text[i];
    createGlyph(c);
  }

  Model *model =
      (Model *)_textGlyphs->getComponent(BitMasks::Components::MODEL);
  glBindVertexArray(model->id);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat), &screenPosition,
               GL_STATIC_DRAW);
  GLuint bitmapVBO;
  glGenBuffers(1, &bitmapVBO);
  glBindBuffer(GL_ARRAY_BUFFER, bitmapVBO);

  std::vector<GLfloat> flattenedCoords;
  for (const auto &coords : _bitmapCoords) {
    flattenedCoords.insert(flattenedCoords.end(), coords.begin(), coords.end());
  }
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * flattenedCoords.size(),
               flattenedCoords.data(), GL_STATIC_DRAW);

  for (int i = 0; i < 4; ++i) {
    glEnableVertexAttribArray(2 + i);
    glVertexAttribPointer(2 + i, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<void *>(i * 2 * sizeof(GLfloat)));
    glVertexAttribDivisor(2 + i, 1);
  }

  UniformUtils::setUniform(model->texture->uniform, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, model->texture->id);
  glDrawElementsInstanced(GL_TRIANGLES, model->vertexCount, GL_UNSIGNED_INT, 0,
                          length);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  _bitmapCoords.clear();
}*/
