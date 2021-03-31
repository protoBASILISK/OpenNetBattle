#pragma once
#include "bnCardAction.h"
#include "bnAnimation.h"
#include <SFML/Graphics.hpp>

class SpriteProxyNode;
class Character;
class TornadoCardAction : public CardAction {
private:
  sf::Sprite fan;
  SpriteProxyNode* attachment;
  Animation attachmentAnim;
  bool armIsOut;
  int damage;
public:
  TornadoCardAction(Character& owner, int damage);
  ~TornadoCardAction();
  void Update(double _elapsed) override;
  void OnEndAction() override;
  void OnExecute() override;
  void OnAnimationEnd() override;
}; 
