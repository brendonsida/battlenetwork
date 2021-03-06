#include "bnTwoMettaurMob.h"

TwoMettaurMob::TwoMettaurMob(Field* field) : MobFactory(field)
{
}


TwoMettaurMob::~TwoMettaurMob()
{
}

Mob* TwoMettaurMob::Build() {
  Mob* mob = new Mob(field);
  int count = 2;

  for (int i = 0; i < field->GetWidth(); i++) {
    for (int j = 0; j < field->GetHeight(); j++) {
      Tile* tile = field->GetAt(i + 1, j + 1);
      if (tile->IsWalkable() && tile->GetTeam() == Team::RED) {
        if (rand() % 50 > 25 && count-- > 0)
          mob->Spawn<Mettaur, MettaurIdleState>(i + 1, j + 1);
      }
    }
  }
  return mob;
}