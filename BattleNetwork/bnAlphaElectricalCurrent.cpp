#include "bnAlphaElectricalCurrent.h"
#include "bnTextureResourceManager.h"
#include "bnAudioResourceManager.h"
#include "bnHitbox.h"
#include "bnField.h"

#define RESOURCE_PATH "resources/mobs/alpha/alpha.animation"

AlphaElectricCurrent::AlphaElectricCurrent(Team team, int count) : 
  countMax(count), 
  count(0), 
  Spell(team)
{
  setTexture(LOAD_TEXTURE(MOB_ALPHA_ATLAS));
  anim = CreateComponent<AnimationComponent>(this);
  anim->SetPath(RESOURCE_PATH);
  anim->Load();
  setScale(2.f, 2.f);
  SetHeight(29);

  auto props = Hit::DefaultProperties;
  props.flags |= Hit::flash;
  props.damage = 100;
  props.element = Element::elec;
  SetHitboxProperties(props);
}

AlphaElectricCurrent::~AlphaElectricCurrent()
{
}

void AlphaElectricCurrent::OnSpawn(Battle::Tile & start)
{
  auto endTrigger = [this]() {
    count++;

    if (count >= countMax) {
      Remove();

      // Remove() lags behind 1 frame
      Hide();
    }
  };

  anim->SetAnimation("ELECTRIC", endTrigger);
  anim->SetPlaybackMode(Animator::Mode::Loop);

  auto attackTopAndBottomRowTrigger = [this]() {
    for (int i = 1; i < 4; i++) {
      auto hitbox = new Hitbox(GetTeam(), 100);
      hitbox->HighlightTile(Battle::Tile::Highlight::solid);
      hitbox->SetHitboxProperties(GetHitboxProperties());
      GetField()->AddEntity(*hitbox, i, 1);
    }
    for (int i = 1; i < 4; i++) {
      auto hitbox = new Hitbox(GetTeam(), 100);
      hitbox->HighlightTile(Battle::Tile::Highlight::solid);
      hitbox->SetHitboxProperties(GetHitboxProperties());
      GetField()->AddEntity(*hitbox, i, 3);
    }

    // This is the center tile that the electric current attack "appears" to be hovering over
    auto hitbox = new Hitbox(GetTeam(), 100);
    hitbox->HighlightTile(Battle::Tile::Highlight::solid);
    hitbox->SetHitboxProperties(GetHitboxProperties());
    GetField()->AddEntity(*hitbox, 3, 2);

    Audio().Play(AudioType::THUNDER);
  };

  auto attackMiddleRowTrigger = [this]() {
    for (int i = 1; i < 4; i++) {
      auto hitbox = new Hitbox(GetTeam());
      hitbox->SetHitboxProperties(GetHitboxProperties());
      hitbox->HighlightTile(Battle::Tile::Highlight::solid);
      GetField()->AddEntity(*hitbox, i, 2);
    }

    Audio().Play(AudioType::THUNDER);
  };


  anim->AddCallback(1, attackTopAndBottomRowTrigger);
  anim->AddCallback(4, attackMiddleRowTrigger);
}


void AlphaElectricCurrent::OnUpdate(double _elapsed)
{
  // In order to lay ontop of alpha's layer, we keep the spell on top of his position
  // but offset the sprite to be 2 tiles to the left...
  Entity::drawOffset = { -(tile->GetWidth() * 2.0f), -GetHeight() };
}

void AlphaElectricCurrent::Attack(Character * _entity)
{
}

void AlphaElectricCurrent::OnDelete()
{
}
