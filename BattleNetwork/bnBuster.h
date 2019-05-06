#pragma once
#include "bnSpell.h"
#include "bnAnimationComponent.h"

/**
 * @class Buster
 * @author mav
 * @date 05/05/19
 * @file bnBuster.h
 * @brief Classic buster attack 
 * 
 * NOTE: This comes from legacy code and could be improved
 */
class Buster : public Spell {
public:
  /**
   * @brief If _charged is true, deals 10 damage
   */
  Buster(Field* _field, Team _team, bool _charged);
  virtual ~Buster();

  virtual void Update(float _elapsed);
  
  /**
   * @brief Moves across tiles in direction
   * @param _direction direction to move
   * @return true if tile exists
   */
  virtual bool Move(Direction _direction);
  
  /**
   * @brief Deal impact damage
   * @param _entity
   */
  virtual void Attack(Character* _entity);
private:
  bool isCharged;
  bool spawnGuard;
  Character* contact;
  int damage;
  float cooldown;
  float random; // offset
};