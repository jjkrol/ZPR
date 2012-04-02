#pragma once

#include <boost/thread.hpp>
#include <queue>

class Ticket;
class Message;

class MessageQueue{
  public:
    Ticket * push(Message inMsg);
    Message pop();

    bool isEmpty();

    boost::condition_variable cond; 
    boost::mutex mut;

  private:
    std::queue<Message> queue;
    boost::shared_mutex queueMutex;
};
