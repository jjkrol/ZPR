#include "../include/asynchronous.hpp"
#include "../include/messageQueue.hpp"
#include "../include/ticket.hpp"
#include "../include/message.hpp"

/*
 * issues:
 *  memory leaks (who will destroy passed data?)
 *  loose threads in concurrentLoop
 */


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
      runTask(msg); 
    }
}

 void Asynchronous::concurrentLoop(){ 
  boost::thread threadFromQueue;
  while(!stopLoops)  
    if(!concurrentMessageQueue.isEmpty()){
      Message msg = concurrentMessageQueue.pop();
      threadFromQueue = boost::thread(&Asynchronous::runTask, this, msg);
      //TODO what happens with those threads?
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
