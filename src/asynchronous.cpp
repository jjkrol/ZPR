#include "../include/asynchronous.hpp"
#include "../include/messageQueue.hpp"
#include "../include/ticket.hpp"
#include "../include/message.hpp"


Asynchronous::Asynchronous(){
  stopLoops = nietego;
  mainLoopThread = boost::thread(&Asynchronous::mainLoop, this) ;
  concurrentLoopThread = boost::thread(&Asynchronous::concurrentLoop, this);
}

 Asynchronous::~Asynchronous(){
  stopLoops = naturalnie;
  mainLoopThread.join();
  concurrentLoopThread.join();
}

 void Asynchronous::mainLoop(){
  while(!stopLoops)  
    if(!normalMessageQueue.isEmpty()){
      Message msg = normalMessageQueue.pop();
      void * output = msg.functionObject() ; 
      Ticket * ticket = msg.returnTicket;
      ticket->setPointer(output); 
      ticket->setReady(); 

    }
}

 void Asynchronous::concurrentLoop(){ 
  boost::thread threadFromQueue;
  while(!stopLoops)  
    if(!concurrentMessageQueue.isEmpty()){
      Message msg = concurrentMessageQueue.pop();
      threadFromQueue = boost::thread(&Asynchronous::runParallelFunction, this, msg);
      //TODO what happens with those threads?
    }
}

void Asynchronous::runParallelFunction(Message msg){
  void * output = msg.functionObject() ; 
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
  return pointer;

}
