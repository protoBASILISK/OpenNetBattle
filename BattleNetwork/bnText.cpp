#include "bnText.h"
#include <cmath>
#include <cctype> // for control codes

void Text::AddLetterQuad(sf::Vector2f position, const sf::Color & color, char letter) const
{
  font.SetLetter(letter);
  const auto texcoords = font.GetTextureCoords();
  const auto origin = font.GetOrigin();
  const sf::Texture& texture = font.GetTexture();
  float width  = static_cast<float>(texture.getSize().x);
  float height = static_cast<float>(texture.getSize().y);
  
  float left   = 0;
  float top    = 0;
  float right  = static_cast<float>(texcoords.width);
  float bottom = static_cast<float>(texcoords.height);

  // fit tall letters on the same line
  float offset = texcoords.height - font.GetLineHeight();
  if (offset > 0) offset = 0;
  position.y = position.y + std::fabs(offset);

  // sfml uses screen space dimensions, not GL 0.0 -> 1.0 space...
  float u1 = static_cast<float>(texcoords.left);
  float v1 = static_cast<float>(texcoords.top);
  float u2 = static_cast<float>(texcoords.left + texcoords.width);
  float v2 = static_cast<float>(texcoords.top + texcoords.height);

  position -= origin;

  vertices.append(sf::Vertex(sf::Vector2f(position.x, position.y + bottom), color, sf::Vector2f(u1, v2)));
  vertices.append(sf::Vertex(sf::Vector2f(position.x, position.y), color, sf::Vector2f(u1, v1)));
  vertices.append(sf::Vertex(sf::Vector2f(position.x + right, position.y + bottom), color, sf::Vector2f(u2, v2)));
  vertices.append(sf::Vertex(sf::Vector2f(position.x, position.y), color, sf::Vector2f(u1, v1)));
  vertices.append(sf::Vertex(sf::Vector2f(position.x + right, position.y + bottom), color, sf::Vector2f(u2, v2)));
  vertices.append(sf::Vertex(sf::Vector2f(position.x + right, position.y), color, sf::Vector2f(u2, v1)));
}

void Text::UpdateGeometry() const
{
  if (!geometryDirty) return;

  vertices.clear();
  bounds = sf::FloatRect();

  if (message.empty()) return; // nothing to draw

  // Precompute the variables needed by the algorithm
  float whitespaceWidth = font.GetWhiteSpaceWidth();
  float letterSpacing = 1.0f; //  (whitespaceWidth / 3.f) * (Text::letterSpacing - 1.f);
  whitespaceWidth += letterSpacing;
  float lineSpacing = font.GetLineHeight() * Text::lineSpacing;
  float x = 0.f;
  float y = 1.0f;

  float minX = 1.0f;
  float minY = 1.0f;
  float maxX = 0.f;
  float maxY = 0.f;

  for (char letter : message) {
    // Handle special characters
    if ((letter == L' ') || (letter == L'\n') || (letter == L'\t'))
    {
      // Update the current bounds (min coordinates)
      minX = std::min(minX, x);
      minY = std::min(minY, y);

      switch (letter)
      {
      case L' ':  x += whitespaceWidth;     break;
      case L'\t': x += whitespaceWidth * 4; break;
      case L'\n': y += lineSpacing; x = 0;  break;
      }

      auto height = letter == '\n' ? 0 : lineSpacing;

      // Update the current bounds (max coordinates)
      maxX = std::max(maxX, x);
      maxY = std::max(maxY, y);

      // Next glyph, no need to create a quad for whitespace
      continue;
    }

    // skip user-defined control codes
    if (iscntrl(letter)) continue;

    AddLetterQuad(sf::Vector2f(x, y), color, letter);

    x += font.GetLetterWidth() + letterSpacing;

    // Update bound values
    auto texcoords = font.GetTextureCoords();
    float left = 0.0f;
    float top = 0.0f; 
    float right = static_cast<float>(texcoords.width); 
    float bottom = static_cast<float>(texcoords.height); 

    minX = std::min(minX, x + left * bottom);
    maxX = std::max(maxX, x + right * top);
    minY = std::min(minY, y + top);
    maxY = std::max(maxY, y + bottom);
  }

  // Update the bounding rectangle
  bounds.left = minX;
  bounds.top = minY;
  bounds.width = maxX - minX;
  bounds.height = maxY - minY;

  geometryDirty = false;
}

Text::Text(const Font& font) : font(font), message(""), geometryDirty(true)
{
  letterSpacing = (font.GetWhiteSpaceWidth() / 3.0f) + 1.0f;
  lineSpacing = 1.0f;
  color = sf::Color::White;
  vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
}

Text::Text(const std::string& message, const Font& font) : font(font), message(message), geometryDirty(true)
{
  letterSpacing = (font.GetWhiteSpaceWidth()/3.0f) + 1.0f;
  lineSpacing = 1.0f;
  color = sf::Color::White;
  vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
}

Text::Text(const Text& rhs) : font(rhs.font)
{
  letterSpacing = rhs.letterSpacing;
  lineSpacing = rhs.lineSpacing;
  message = rhs.message;
  color = rhs.color;
  bounds = rhs.bounds;
  vertices = rhs.vertices;
  geometryDirty = rhs.geometryDirty;
}

Text::~Text()
{
}

void Text::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
  UpdateGeometry();

  states.transform *= getTransform();
  states.texture = &font.GetTexture();

  target.draw(vertices, states);
}

void Text::SetFont(const Font& font)
{
  geometryDirty |= Text::font.GetStyle() != font.GetStyle();
  Text::font = font;
}

void Text::SetString(const std::string& message)
{
  geometryDirty |= Text::message != message;
  Text::message = message;
}

void Text::SetString(char c)
{
  geometryDirty |= Text::message != std::to_string(c);
  Text::message = std::string(1, c);
}

void Text::SetColor(const sf::Color & color)
{
  geometryDirty |= Text::color == color;

  Text::color = color;

  // Change vertex colors directly, no need to update whole geometry
  // (if geometry is updated anyway, we can skip this step)
  if (!geometryDirty)
  {
    for (std::size_t i = 0; i < vertices.getVertexCount(); ++i)
      vertices[i].color = Text::color;
  }
}

void Text::SetLetterSpacing(float spacing)
{
  geometryDirty |= letterSpacing != spacing;

  letterSpacing = spacing;
}

void Text::SetLineSpacing(float spacing)
{
  geometryDirty |= lineSpacing != spacing;

  lineSpacing = spacing;
}

const std::string & Text::GetString() const
{
  return message;
}

const Font & Text::GetFont() const
{
  return font;
}

const Font::Style & Text::GetStyle() const
{
  return font.GetStyle();
}

sf::FloatRect Text::GetLocalBounds() const
{
  UpdateGeometry();

  return bounds;
}

sf::FloatRect Text::GetWorldBounds() const
{
  return getTransform().transformRect(GetLocalBounds());
}