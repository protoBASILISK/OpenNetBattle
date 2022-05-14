#pragma once
#ifdef ONB_MOD_SUPPORT

#include <sol/sol.hpp>
#include <functional>
#include <SFML/Graphics.hpp>
#include "dynamic_object.h"
#include "../bnCardAction.h"
#include "../bnAnimation.h"
#include "bnWeakWrapper.h"

class SpriteProxyNode;
class Character;
class ScriptedCardAction : public CardAction, public dynamic_object {
public:
  ScriptedCardAction(std::shared_ptr<Character> actor, const std::string& state);
  virtual ~ScriptedCardAction();

  void Init();
  void Update(double elapsed) override;
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
  std::optional<bool> CanMoveTo(Battle::Tile* next) override;
  void SetBackgroundData(const std::filesystem::path& bgTexturePath, const std::filesystem::path& animPath, float velx, float vely);
  void SetBackgroundColor(const sf::Color& color);

  void OnAnimationEnd() override;
  void OnActionEnd() override;
  void OnExecute(std::shared_ptr<Character> user) override;

  sol::object on_update_func;
  sol::object on_animation_end_func;
  sol::object on_action_end_func;
  sol::object on_execute_func;
  sol::object can_move_to_func;

private:
  WeakWrapper<ScriptedCardAction> weakWrap;
};

#endif