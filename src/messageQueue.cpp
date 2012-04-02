#include "../include/messageQueue.hpp"
#include "../include/ticket.hpp"
#include "../include/message.hpp"

Ticket * MessageQueue::push(Message inMsg){
  Ticket* ticket = new Ticket();
  inMsg.returnTicket = ticket; // loop has to know where to store the return value

  boost::lock_guard<boost::mutex> cond_lock(mut);
  //join those two mutexes?
  boost::unique_lock<boost::shared_mutex> lock(queueMutex);
  queue.push(inMsg);
  cond.notify_all();

  return ticket;
}

Message MessageQueue::pop(){
  Message retMsg;

  boost::unique_lock<boost::shared_mutex> lock(queueMutex);
  retMsg = queue.front();
  queue.pop();

  return retMsg;
}

bool MessageQueue::isEmpty(){
  boost::shared_lock<boost::shared_mutex> lock(queueMutex);
  return queue.empty();
}  

