#include "../include/ticket.hpp"

void * Ticket::getPointer(){
  return pointer;
}

void Ticket::setPointer(void * newPointer){
  pointer = newPointer;
}

void Ticket::wait(){
  boost::unique_lock<boost::mutex> lock(mut);
  while(!ready){
    cond.wait(lock);
  }
}

void Ticket::setReady(){
  boost::lock_guard<boost::mutex> lock(mut);
  ready = true;
  cond.notify_all();
}

