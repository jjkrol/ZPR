#include "../include/messageQueue.hpp"
#include "../include/ticket.hpp"
#include "../include/message.hpp"

Ticket * MessageQueue::push(Message inMsg){
  Ticket* ticket = new Ticket();
  inMsg.returnTicket = ticket; // loop has to know where to store the return value

  boost::lock_guard<boost::mutex> lock(mut);
  queueMutex.lock();
  {
    queue.push(inMsg);
  }
  queueMutex.unlock();
  cond.notify_all();
  return ticket;
}

Message MessageQueue::pop(){
  Message retMsg;

  queueMutex.lock();
  {
    retMsg = queue.front();
    queue.pop();
  }
  queueMutex.unlock();

  return retMsg;
}

bool MessageQueue::isEmpty(){
  return queue.empty();
}  

