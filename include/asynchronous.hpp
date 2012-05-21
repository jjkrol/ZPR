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

    /**
     * @brief This loop is used for doing tasks from the queue
     * one by one. 
     */
    virtual void mainLoop();

    /**
     * @biref this loop runs tasks, that can be
     * run in parallel threads
     */
    virtual void concurrentLoop(); 

    /**
     * @brief used by loops to run tasks
     */
    void runTask(Message msg);

    /**
     * @biref used for invoking queued methods through a message queue
     */
    virtual void * invokeQueuedMethod(boost::function<void*()> funct);

    /**
     * @brief used for invoking concurrent methods through a message queue
     */
    virtual void * invokeConcurrentMethod(boost::function<void*()> funct);
  
    /**
     * @brief function that cares about putting the task into the queue
     * and handles the return value
     */
    virtual void * invokeMethod(boost::function<void*()> funct, MessageQueue& mq);

    /** 
     * @brief used for returning deleting pointers and returning value
     * so that function returning by value do not produce memory leaks
     */
    template<class T>
     T returnByValueConcurrent(boost::function<void*()> functionObject){
     T * retPtr = reinterpret_cast<T*>(
         invokeConcurrentMethod(
           functionObject
           )
         );
     T retVal = * retPtr;
     delete retPtr;
     return retVal;
    }

    template<class T>
     T returnByValueQueued(boost::function<void*()> functionObject){
    T * retPtr = reinterpret_cast<T*>(
         invokeQueuedMethod(
           functionObject
           )
         );
     T retVal = * retPtr;
     delete retPtr;
     return retVal;
    }

    template<class T>
     T * returnByPointerConcurrent(boost::function<void*()> functionObject){
     T * retPtr = reinterpret_cast<T*>(
         invokeConcurrentMethod(
           functionObject
           )
         );
     return retPtr;
    }

    template<class T>
     T * returnByPointerQueued(boost::function<void*()> functionObject){
     T * retPtr = reinterpret_cast<T*>(
         invokeQueuedMethod(
           functionObject
           )
         );
     return retPtr;
    }

    bool stopLoops;
    boost::condition_variable condMainLoop; //is this necessary?
    boost::mutex mutMainLoop;
    boost::condition_variable condConcLoop; 
    boost::mutex mutConcLoop;

    MessageQueue normalMessageQueue;
    boost::thread mainLoopThread;
    MessageQueue concurrentMessageQueue;
    boost::thread concurrentLoopThread;
};
