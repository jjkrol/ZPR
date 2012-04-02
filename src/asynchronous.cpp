#include "../include/asynchronous.hpp"
#include "../include/messageQueue.hpp"
#include "../include/ticket.hpp"
#include "../include/message.hpp"
#include <stdlib.h>
/*
 * issues:
 *  memory leaks (who will destroy passed data?)
 *  loose threads in concurrentLoop
 */


Asynchronous::Asynchronous(){
  stopLoops = nietego;
  mainLoopThread = boost::thread(&Asynchronous::mainLoop, this) ;
  concurrentLoopThread = boost::thread(&Asynchronous::concurrentLoop, this);
  std::cout<<"asynch constructor"<<std::endl;
}

 Asynchronous::~Asynchronous(){
  stopLoops = naturalnie;
  mainLoopThread.join();
  concurrentLoopThread.join();
}

 void Asynchronous::mainLoop(){
  boost::unique_lock<boost::mutex> lock(normalMessageQueue.mut);
  while(!stopLoops){
    while(normalMessageQueue.isEmpty()){
      normalMessageQueue.cond.wait(lock);
      Message msg = normalMessageQueue.pop();
      runTask(msg); 
    }
  }
}

 void Asynchronous::concurrentLoop(){ 
   std::cout<<"entering councurrent loop"<<std::endl;
  boost::thread threadFromQueue;
  boost::unique_lock<boost::mutex> lock(concurrentMessageQueue.mut);
  while(!stopLoops){
    while(concurrentMessageQueue.isEmpty()){
      concurrentMessageQueue.cond.wait(lock);
      Message msg = concurrentMessageQueue.pop();
      threadFromQueue = boost::thread(&Asynchronous::runTask, this, msg);
      //TODO what happens with those threads?
    }
  }
}

void Asynchronous::runTask(Message msg){
  //run the task
  void * output = msg.functionObject() ; 

  //fill the response 
  Ticket * ticket = msg.returnTicket;
  ticket->setPointer(output); 
  ticket->setReady();
}

 void * Asynchronous::invokeQueuedMethod(boost::function<void*()> funct){
  return invokeMethod(funct, normalMessageQueue);
}

 void * Asynchronous::invokeConcurrentMethod(boost::function<void*()> funct){
  return invokeMethod(funct, concurrentMessageQueue);
}

 void * Asynchronous::invokeMethod(boost::function<void*()> funct, MessageQueue& mq){
  //prepare a message
  Message msg;
  msg.functionObject = funct;

  //prepare return ticket
  Ticket * ticket;
  ticket = mq.push(msg);

  //wait for the order to be complete
  ticket->wait();
  void * pointer = ticket->getPointer();
  delete ticket; //TODO would be better do delete this in the queue, but how?
  return pointer;
}
