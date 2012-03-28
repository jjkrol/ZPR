#pragma once

#include <boost/thread.hpp>

class Ticket{
  public:
    void * getPointer();
    void setPointer(void * newPointer);

    void wait();

    void setReady();

  private:
    boost::mutex mut;
    boost::condition_variable cond;
    bool ready;
    void * pointer;
};
