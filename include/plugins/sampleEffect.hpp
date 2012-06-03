#pragma once

#include "effect.hpp"

class Photo;

class SampleEffect : public Effect {
  public:
    enum Operation {
      NOOPERATION,
      VERTICAL,
      HORIZONTAL
    };
    SampleEffect(Operation operation);
    virtual ~SampleEffect() {};
    void execute(Photo* photo);

  private:
    Operation operation;
};
