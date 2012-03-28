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

  private:
    std::queue<Message> queue;
    boost::mutex queueMutex;
};
