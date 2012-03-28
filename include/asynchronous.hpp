#pragma once

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "messageQueue.hpp"

#define naturalnie true
#define nietego false

class MessageQueue;
class Message;

class Asynchronous{
  public:
    Asynchronous();

    virtual ~Asynchronous();

  protected:
    /*
     * This loop is used for doing tasks from the queue
     * one by one. 
     *
     */
    virtual void mainLoop();

    /* 
     * this loop runs tasks, that can be
     * run in parallel threads
     */

    virtual void concurrentLoop(); 

    void runParallelFunction(Message msg);

    virtual void * invokeQueuedMethod(boost::function<void*()> funct);

    virtual void * invokeConcurrentMethod(boost::function<void*()> funct);
  
    virtual void * invokeMethod(boost::function<void*()> funct, MessageQueue& mq);

    MessageQueue normalMessageQueue;
    MessageQueue concurrentMessageQueue;
    bool stopLoops;
    boost::thread mainLoopThread;
    boost::thread concurrentLoopThread;
};
