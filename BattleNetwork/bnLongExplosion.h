#pragma once
#include "bnArtifact.h"
#include "bnField.h"

class LongExplosion : public Artifact
{
private:
  //Animation
  float explosionProgress;
  float explosionProgress2;
  Sprite explosion;
  Sprite explosion2;
  FrameAnimation explode;
  float x1, x2, y1, y2;
public:
  LongExplosion(Field* _field, Team _team);
  ~LongExplosion();

  virtual void Update(float _elapsed);
  virtual int GetStateFromString(string _string);
  virtual void AddAnimation(int _state, FrameAnimation _animation, float _duration);
  virtual bool Move(Direction _direction) { return false; }
  vector<Drawable*> GetMiscComponents();

};

