#include "bnMettaur.h"
#include "bnMettaurIdleState.h"
#include "bnMettaurAttackState.h"
#include "bnMettaurMoveState.h"
#include "bnExplodeState.h"
#include "bnTile.h"
#include "bnField.h"
#include "bnWave.h"
#include "bnTextureResourceManager.h"
#include "bnAudioResourceManager.h"
#include "bnEngine.h"

#define RESOURCE_NAME "mettaur"
#define RESOURCE_PATH "resources/mobs/mettaur/mettaur.animation"

#define MOVING_ANIMATION_SPRITES 2
#define MOVING_ANIMATION_WIDTH 32
#define MOVING_ANIMATION_HEIGHT 41

#define IDLE_ANIMATION_WIDTH 54
#define IDLE_ANIMATION_HEIGHT 56

#define HIT_ANIMATION_SPRITES 3
#define HIT_ANIMATION_WIDTH 34
#define HIT_ANIMATION_HEIGHT 31

#define ATTACK_ANIMATION_SPRITES 9
#define ATTACK_ANIMATION_WIDTH 53
#define ATTACK_ANIMATION_HEIGHT 56

vector<int> Mettaur::metIDs = vector<int>();
int Mettaur::currMetIndex = 0;

Mettaur::Mettaur(void)
  : animationComponent(this), AI<Mettaur>(this) {
  this->StateChange<MettaurIdleState>();
  name = "Mettaur";
  Entity::team = Team::RED;
  health = 20;
  hitHeight = 0;
  textureType = TextureType::MOB_METTAUR_IDLE;
  healthUI = new MobHealthUI(this);

  setTexture(*TEXTURES.GetTexture(textureType));
  setScale(2.f, 2.f);

  this->SetHealth(health);

  //Components setup and load
  animationComponent.setup(RESOURCE_NAME, RESOURCE_PATH);
  animationComponent.load();

  whiteout = SHADERS.GetShader(ShaderType::WHITE);

  metID = (int)Mettaur::metIDs.size();
  Mettaur::metIDs.push_back((int)Mettaur::metIDs.size());
}

Mettaur::~Mettaur(void) {

}

int* Mettaur::GetAnimOffset() {
  Mettaur* mob = this;

  int* res = new int[2];
  res[0] = 35;  res[1] = 35;

  if (mob->GetTextureType() == TextureType::MOB_METTAUR_IDLE) {
    res[0] = 35;
    res[1] = 35;
  } else if (mob->GetTextureType() == TextureType::MOB_METTAUR_ATTACK) {
    res[0] = 65;
    res[1] = 95;
  } else if (mob->GetTextureType() == TextureType::MOB_MOVE) {
    res[0] = 45;
    res[1] = 55;
  } 

  return res;
}

void Mettaur::Update(float _elapsed) {
  this->SetShader(nullptr);

  this->StateUpdate(_elapsed);

  // Explode if health depleted
  if (GetHealth() <= 0) {
    this->StateChange<ExplodeState<Mettaur>>();
    
    if (Mettaur::metIDs.size() > 0) {
      vector<int>::iterator it = find(Mettaur::metIDs.begin(), Mettaur::metIDs.end(), metID);

      if (it != Mettaur::metIDs.end()) {
        // Remove this mettaur out of rotation...
        Mettaur::currMetIndex++;

        Mettaur::metIDs.erase(it);
        if (Mettaur::currMetIndex >= Mettaur::metIDs.size()) {
          Mettaur::currMetIndex = 0;
        }
      }
    }

    this->Lock();
  } else {
    this->RefreshTexture();
    animationComponent.update(_elapsed);
  }

  healthUI->Update();
  Entity::Update(_elapsed);
}

void Mettaur::RefreshTexture() {
  if (state == MobState::MOB_IDLE) {
    textureType = TextureType::MOB_METTAUR_IDLE;
  } else if (state == MobState::MOB_MOVING) {
    textureType = TextureType::MOB_MOVE;
  } else if (state == MobState::MOB_ATTACKING) {
    textureType = TextureType::MOB_METTAUR_ATTACK;
  }
  setTexture(*TEXTURES.GetTexture(textureType));

  if (textureType == TextureType::MOB_METTAUR_IDLE) {
    setPosition(tile->getPosition().x + tile->GetWidth() / 2.0f - 25.0f, tile->getPosition().y + tile->GetHeight() / 2.0f - 45.0f);
    hitHeight = getLocalBounds().height;
  } else if (textureType == TextureType::MOB_MOVE) {
    setPosition(tile->getPosition().x + tile->GetWidth() / 2.0f - 35.0f, tile->getPosition().y + tile->GetHeight() / 2.0f - 60.0f);
  } else if (textureType == TextureType::MOB_METTAUR_ATTACK) {
    setPosition(tile->getPosition().x + tile->GetWidth() / 2.0f - 55.0f, tile->getPosition().y + tile->GetHeight() / 2.0f - 105.0f);
    hitHeight = getLocalBounds().height;
  }
}

vector<Drawable*> Mettaur::GetMiscComponents() {
  vector<Drawable*> drawables = vector<Drawable*>();
  drawables.push_back(healthUI);

  return drawables;
}

int Mettaur::GetStateFromString(string _string) {
  int size = 4;
  string MOB_STATE_STRINGS[] = { "MOB_MOVING", "MOB_IDLE", "MOB_HIT", "MOB_ATTACKING" };
  for (int i = 0; i < size; i++) {
    if (_string == MOB_STATE_STRINGS[i]) {
      return static_cast<MobState>(i);
    }
  }
  Logger::Logf("Failed to find corresponding enum: %s\n", _string);
  return -1;
}


void Mettaur::SetAnimation(int _state, std::function<void()> onFinish) {
  this->state = static_cast<MobState>(_state);
  animationComponent.setAnimation(_state, onFinish);
}

TextureType Mettaur::GetTextureType() const {
  return textureType;
}

int Mettaur::GetHealth() const {
  return health;
}

void Mettaur::SetHealth(int _health) {
  health = _health;
}

int Mettaur::Hit(int _damage) {
  (health - _damage < 0) ? health = 0 : health -= _damage;
  SetShader(whiteout);
  return health;
}

float Mettaur::GetHitHeight() const {
  return hitHeight;
}

const bool Mettaur::IsMettaurTurn() const
{
  return (Mettaur::metIDs.at(Mettaur::currMetIndex) == this->metID);
}

void Mettaur::NextMettaurTurn() {
  Mettaur::currMetIndex++;

  if (Mettaur::currMetIndex >= Mettaur::metIDs.size()) {
    Mettaur::currMetIndex = 0;
  }
}