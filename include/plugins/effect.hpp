#pragma once

class Photo;

class Effect{
  public:
    virtual void execute(Photo* photo) = 0;
};
