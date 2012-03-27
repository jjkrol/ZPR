#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <queue>

/*
 * issues:
 *  passing pointers (which type?)
 *  memory leaks (who will destroy passed data?)
 *
 */

class Ticket {
public:
  Ticket (){};
  virtual ~Ticket (){};
  int * getPointer(){
    return pointer;
  };

  void setPointer(int * newPointer){
    pointer = newPointer;
  }

  bool isReady(){
    return ready;
  }
  void setReady(bool readyState){
    ready = readyState;
  }

private:
  bool ready;
  int *  pointer;
};

class Message {
public:
  Message (){};
  virtual ~Message (){};
  boost::function<int*()> functionAddress;
  Ticket * returnTicket;
private:
};

class MessageQueue {
public:
  MessageQueue (){};
  virtual ~MessageQueue (){};
  Ticket * put(Message inMsg){
     std::cout<<"-- Putting order in the order queue"<<std::endl;
     sleep(1);
    Ticket* ticket = new Ticket();
    inMsg.returnTicket = ticket;
    queueMutex.lock();
    queue.push(inMsg);
    queueMutex.unlock();
    return ticket;
  }
  bool isEmpty(){
    return queue.empty();
  }  
  Message pop(){
    std::cout<<"-- Order was taken by the chef"<<std::endl;
    sleep(1);
    Message retMsg;
    queueMutex.lock();
    retMsg = queue.front();
    queue.pop();
    queueMutex.unlock();
    return retMsg;
  }

private:
  std::queue<Message> queue;
  boost::mutex queueMutex;
};

class PizzaDeliverySystem {
public:
  PizzaDeliverySystem (){
    stopLoop = false;
     mainLoopThread = boost::thread(&PizzaDeliverySystem::mainLoop, this) ;
  }

  ~PizzaDeliverySystem (){
    stopLoop = true;
    mainLoopThread.join();
  }

  std::string getPizza(std::string pizzaName){
    Message msg;
    Ticket * ticket;

    msg.functionAddress = boost::bind(&PizzaDeliverySystem::pleaseGetPizza, this, pizzaName); //&PizzaDeliverySystem::pleaseGetPizza;
    ticket = inMQ.put(msg);
    std::cout<<"-- The order ticket for pizza "<<pizzaName<<" was created, waiting for the pizza..."<<std::endl;
    sleep(1);
    while(!ticket->isReady())
      ;
    std::cout<<"-- The order ticket for "<<pizzaName<<" is ready, we're sending pizza to the client!"<<std::endl;
    sleep(1);
    return *reinterpret_cast<std::string*>(ticket->getPointer());
  }

private:
  void mainLoop(){
    Message msg;
    while(!stopLoop){  //implement a way to stop it
      if(!inMQ.isEmpty()){
        msg = inMQ.pop();
        int * output = msg.functionAddress() ; 
         Ticket * ticket = msg.returnTicket;
         ticket->setPointer(output); // = run(msg.function) // make class friend
         std::cout<<"-- Chef: The order  is ready, next please!"<<std::endl;
    sleep(1);
         ticket->setReady(true); // not really safe
      }
    }
  }
  int* pleaseGetPizza(std::string pizzaName){
    std::cout<<"-- Chef: I'm making a delicious "<<pizzaName<<" pizza..."<<std::endl;
    sleep(3);
    std::string * pizza = new std::string(pizzaName+" and green peppers, mushrooms, olives, chives");
         return reinterpret_cast<int*>(pizza);
  }

  MessageQueue inMQ;
  bool stopLoop;
  boost::thread mainLoopThread;
};

class MeInMyHouse {
public:
  MeInMyHouse (){};
  ~MeInMyHouse (){};
  void letsEatPizza(){
    std::cout<<"Ordering first pizza"<<std::endl;
    boost::thread pizzaOrder1(&MeInMyHouse::getPizza1, this);
    sleep(1);

    std::cout<<"Crap! Forgot those pizzas are small. Ordering second pizza"<<std::endl;
    boost::thread pizzaOrder2(&MeInMyHouse::getPizza2, this);
    sleep(1);

    std::cout<<"Turning on TV"<<std::endl;
    sleep(1);

    std::cout<<"Finding some cool movies"<<std::endl;
    sleep(1);

    pizzaOrder1.join();
    std::cout<<"Got my first pizza, it is has: "<<pizza1<<std::endl;
    sleep(1);

    pizzaOrder2.join();
    std::cout<<"Got my second pizza, it is has: "<<pizza2<<std::endl;

    sleep(1);

    std::cout<<"Delicious!"<<std::endl;
  }
private:
  std::string pizza1, pizza2;
  PizzaDeliverySystem pizzaGuys;

  void getPizza1(){
  pizza1 = pizzaGuys.getPizza("peperoni");
  }

  void getPizza2(){
  pizza2 = pizzaGuys.getPizza("pineapple");
  }
};


int main(int argc, char** argv){
    MeInMyHouse me;
    std::cout<<"I'm at home, hungry as fuck. Let's order some pizza!"<<std::endl;
    sleep(2);

    me.letsEatPizza();
  }
