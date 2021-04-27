#include "bnTextureResourceManager.h"
#include "bnAudioResourceManager.h"
#include "bnField.h"
#include "bnSpell.h"
#include "bnAura.h"
#include "bnLayered.h"
#include "bnDefenseAura.h"
#include "battlescene/bnBattleSceneBase.h"

using sf::IntRect;

#define RESOURCE_PATH "resources/spells/auras.animation"

Aura::Aura(Aura::Type type, Character* owner) : type(type), Component(owner, Component::lifetimes::battlestep)
{
  timer = 50; // seconds

  persist = false;
  isOver = false;

  switch (type) {
  case Aura::Type::AURA_100:
    health = 100;
    break;
  case Aura::Type::AURA_200:
    health = 200;
    break;
  case Aura::Type::AURA_1000:
    health = 1000;;
    break;
  case Aura::Type::BARRIER_10:
    health = 100;
    persist = true;
    break;
  case Aura::Type::BARRIER_200:
    health = 200;
    persist = true;
    break;
  case Aura::Type::BARRIER_500:
    health = 500;
    persist = true;
    break;
  }

  currHP = health;

  DefenseAura::Callback onHit = [this](Spell& in, Character& owner, bool windRemove) { 
    auto hitbox = in.GetHitboxProperties();
    if (windRemove) {
      if (fx && fx->flyStartTile == nullptr) {
        // Attach visual fx node to the tile and make it fly up and away based on direction
        auto direction = in.GetDirection();
        if (direction == Direction::none) {
          direction = owner.GetDirection();
        }

        if (direction == Direction::left) {
          this->fx->flyAccel = { -5.f, -12.0f };
        }
        else {
          this->fx->flyAccel = { 5.f, -12.0f };
        }

        owner.RemoveNode(this->fx);

        this->fx->flyStartTile = owner.GetTile();
        this->fx->flyStartTile->AddNode(this->fx);
        this->fx->SetLayer(-1); // stay in front of the tile

        // this will trigger the flicker-out animation as it flies away
        // and at the end of the flicker-out animation, it will cleanup memory
        this->health = this->fx->currHP = 0;
        this->fx = nullptr;
      }
      else {
        // We can no longer protect this user->..
        owner.Hit(hitbox);
      }
    }
    else {
      TakeDamage(in.GetHitboxProperties().damage);
    }
  };
  
  defense = new DefenseAura(onHit);
  
  owner->AddDefenseRule(defense);
  fx = owner->CreateComponent<Aura::VisualFX>(owner, type);
  fx->currHP = health;
  fx->timer = timer;
  owner->AddNode(fx);
}

void Aura::OnUpdate(double _elapsed) {
  if (Injected() == false) return;

  // If the aura has been replaced by another defense rule, remove all
  // associated components 
  if (defense->IsReplaced()) {
    Eject();
    return;
  }

  currHP = health;
  
  // The timer will be used for non-persistent aura types
  // And again when the aura is due for deletion (flicker time)
  // Though it should only lose time during battle steps
  if(!persist || isOver) {
    timer -= _elapsed;
  }

  if (!isOver) {
    if (health == 0 || timer <= 0.0) {
      isOver = true;
      timer = 2;
    }
  }
  else if (timer <= 0.0) {
    if (auto character = GetOwnerAs<Character>()) {
      character->RemoveDefenseRule(defense);
    }

    Eject();
    return;
  }

 if (GetOwner()->GetTile() == nullptr) {
   fx? fx->Hide() : (void)0;
   return;
 }

 if (GetOwner()->WillRemoveLater()) {
   timer = 0;
   Eject();
 }

 if (fx) {
   // Update the visual fx
   fx->currHP = currHP;
 }
}

void Aura::Inject(BattleSceneBase& bs)
{
  bs.Inject(this);
}

const Aura::Type Aura::GetAuraType()
{
  return type;
}
 
void Aura::Persist(bool enable) {
  persist = enable;
}

const bool Aura::IsPersistent() const {
   return persist;
 }
  
const int Aura::GetHealth() const {
  return health;
}

void Aura::TakeDamage(int damage)
{
  if (type >= Aura::Type::BARRIER_10) {
    health = health - damage;
  }
  else if (health <= damage) {
    health = 0;
  }

  health = std::max(0, health);
}

Aura::~Aura()
{
  if(fx) {
    GetOwner()->RemoveNode(fx);
    fx->Eject();
  }

  delete defense;
}

//////////////////////////////////////////////////////
//                Visual FX object                  //
//////////////////////////////////////////////////////
Aura::VisualFX::VisualFX(Entity* owner, Aura::Type type) : 
  type(type),
  UIComponent(owner) {
    
  ResourceHandle handle;
  auraSprite.setTexture(*handle.Textures().GetTexture(TextureType::SPELL_AURA));
  aura = new SpriteProxyNode(auraSprite);
  SetLayer(1); // behind player

  fontTextureRef = LOAD_TEXTURE(AURA_NUMSET);
  font.setTexture(*fontTextureRef);

  //Components setup and load
  animation = Animation(RESOURCE_PATH);
  animation.Reload();

  switch (type) {
  case Aura::Type::AURA_100:
    animation.SetAnimation("AURA_100");
    break;
  case Aura::Type::AURA_200:
    animation.SetAnimation("AURA_200");
    break;
  case Aura::Type::AURA_1000:
    animation.SetAnimation("AURA_1000");
    break;
  case Aura::Type::BARRIER_10:
    animation.SetAnimation("BARRIER_10");
    break;
  case Aura::Type::BARRIER_200:
    animation.SetAnimation("BARRIER_200");
    break;
  case Aura::Type::BARRIER_500:
    animation.SetAnimation("BARRIER_500");
    break;
  }

  animation << Animator::Mode::Loop;
  animation.Update(0, aura->getSprite());

  AddNode(aura);
  this->setPosition(0, -owner->GetHeight() / 2.f / 2.f); // descale from 2.0x upscale and then get the half of the height

  // we want to draw relative to the component's owner
  SetDrawOnUIPass(false);
}

Aura::VisualFX::~VisualFX() {
  if (flyStartTile) {
    flyStartTile->RemoveNode(this);
  }

  delete aura;
}

void Aura::VisualFX::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  auto this_states = states;
  this_states.transform *= getTransform();

  UIComponent::draw(target, this_states);

  // Only draw HP font for Barriers. Auras are hidden.
  if (type <= Type::AURA_1000) return;

  // 0 - 5 are on first row
  // Glyphs are 8x15 
  // 2nd row starts +1 pixel down

  if (currHP > 0) {
    int size = (int)(std::to_string(currHP).size());
    int hp = currHP;
    float offsetx = -(((size) * 4.0f) / 2.0f) * font.getScale().x;
    int index = 0;
    while (index < size) {
      const char c = std::to_string(currHP)[index];
      int number = std::atoi(&c);
      int rowstart = 0;

      if (number > 4) {
        rowstart = 16;
      }

      int col = 8 * (number % 5);

      font.setTextureRect(sf::IntRect(col, rowstart, 8, 15));
      font.setPosition(sf::Vector2f(offsetx, 15.0f));

      float fontScaleX = font.getScale().x;

      // this is flipped, but we do not want to flip fonts
      if (this_states.transform.getMatrix()[0] < 0.0f && fontScaleX > 0.f) {
        font.setScale(-fontScaleX, font.getScale().y);
      }

      auto font_states = this_states;
      font_states.transform *= font.getTransform();

      target.draw(font, font_states);

      offsetx += 4.0f * font.getScale().x;

      index++;
    }
  }
}

void Aura::VisualFX::Inject(BattleSceneBase& bs) {
  bs.Inject((Component*)this);
  bs.Audio().Play(AudioType::APPEAR);
}

void Aura::VisualFX::OnUpdate(double _elapsed)
{
  timer += _elapsed;

  // accumulate acceleration if nonzero
  flySpeed.x += flyAccel.x * float(_elapsed);
  flySpeed.y += flyAccel.y * float(_elapsed);

  sf::Vector2f newPos = getPosition();
  newPos.x += flySpeed.x;
  newPos.y += flySpeed.y;

  setPosition(newPos);

  // Barriers flicker every 3rd frame
  if (type > Type::AURA_1000) {
    if (from_seconds(timer).count() % 4 == 0) {
      Hide();
    }
    else {
      Reveal();
    }
  }

  if (currHP == 0) {
    if (!flyStartTile) {
      auraSprite.setColor(sf::Color(255, 255, 255, 50));
    }
    else if (currHP == 0 && (flyStartTile->getPosition().y + getPosition().y) < 0) {
      // we have flown up and out of the screen
      Eject();
    }
  }

  animation.Update(_elapsed, aura->getSprite());
}