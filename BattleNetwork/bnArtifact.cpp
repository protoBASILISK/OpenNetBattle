#include "bnArtifact.h"
#include <random>
#include <time.h>

#include "bnArtifact.h"
#include "bnTile.h"
#include "bnField.h"
#include "bnTextureResourceManager.h"

Artifact::Artifact(Field* _field) {
  this->SetField(_field);
  this->SetTeam(Team::UNKNOWN);
  this->SetPassthrough(true);
}

Artifact::Artifact() {
  SetLayer(0);
  texture = nullptr;
  this->SetPassthrough(true);
}

Artifact::~Artifact() {
}

void Artifact::AdoptTile(Battle::Tile * tile)
{
  tile->AddEntity(*this);

  if (!isSliding) {
    this->setPosition(tile->getPosition());
  }
}
