/*! \brief Undernet background uses Background class to animate and scroll
 *  \class UndernetBackground
 */

#pragma once
#include <SFML/Graphics.hpp>
using sf::Texture;
using sf::Sprite;
using sf::IntRect;
using sf::Drawable;
#include <vector>
using std::vector;

#include "bnBackground.h"

class UndernetBackground : public Background
{
public:
  UndernetBackground();
  ~UndernetBackground();

  virtual void Update(float _elapsed);

private:
  float progress;
  int colorIndex; /** The current color to flash */
  std::vector<sf::Color> colors; /**< In the game, the undernet flashes colors */
  float colorProgress;
  sf::Time colorDuration; /**< Duration inbetween color clashes */
};
