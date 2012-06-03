#pragma once

#include "effect.hpp"

class Photo;

class DesaturateEffect : public Effect {
  public:
    DesaturateEffect(double);
    virtual ~DesaturateEffect() {};
    void execute(Photo* photo);

  private:
    float amount;
};
