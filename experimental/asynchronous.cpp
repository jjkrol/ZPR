#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <queue>

#define naturalnie  true
#define nietego     false


/*
 * issues:
 *  passing pointers - which type is the best for passing return values?
 *  memory leaks (who will destroy passed data?)
 *  coding style - java style bitch!
 *
 */

// general classes

class Ticket {
  public:
    Ticket (){};
    virtual ~Ticket (){};
    void * getPointer(){
      return pointer;
    };

    void setPointer(void * newPointer){
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
    void *  pointer;
};

struct Message {
    boost::function<void*()> functionObject;
    Ticket * returnTicket;
};

class MessageQueue {
  public:
    MessageQueue (){};
    virtual ~MessageQueue (){};

    Ticket * push(Message inMsg){
      Ticket* ticket = new Ticket();
      inMsg.returnTicket = ticket; // loop has to know where to store the return value

      queueMutex.lock();
        queue.push(inMsg);
      queueMutex.unlock();

      return ticket;
    }

    Message pop(){
      Message retMsg;

      queueMutex.lock();
        retMsg = queue.front();
        queue.pop();
      queueMutex.unlock();

      return retMsg;
    }

    bool isEmpty(){
      return queue.empty();
    }  

  private:
    std::queue<Message> queue;
    boost::mutex queueMutex;
};

class Asynchronous{
  public:
  Asynchronous(){
    stopLoop = nietego;
    mainLoopThread = boost::thread(&Asynchronous::mainLoop, this) ;
  }

  virtual ~Asynchronous(){
      stopLoop = naturalnie;
      mainLoopThread.join();
  }

  protected:

    virtual void mainLoop(){
      while(!stopLoop){  
        if(!normalMessageQueue.isEmpty()){
          Message msg = normalMessageQueue.pop();
          void * output = msg.functionObject() ; 
          Ticket * ticket = msg.returnTicket;
          ticket->setPointer(output); 
          ticket->setReady(naturalnie); // not really safe
        }
      }
    }

    virtual void * invokeMethod(boost::function<void*()> funct){
      //prepare a message
      Message msg;
      msg.functionObject = funct;
      
      //prepare return ticket
      Ticket * ticket;
      ticket = normalMessageQueue.push(msg);

      //wait for the order to be complete
      while(!ticket->isReady())
        ;
      return ticket->getPointer();
    }

    MessageQueue normalMessageQueue;
    bool stopLoop;
    boost::thread mainLoopThread;
};

// specific classes

class PizzaDeliverySystem : public Asynchronous {
  public:

    /* all public methods are similar to that pattern:
     * returntype methodName(arguments){
     *  return *reinterpret_cast<returntype*>( if we return by value
     *  OR
     *  return reinterpret_cast<returntype*>( if we return pointer
     *    invokeMethod(
     *      boost::bind(
     *        &ObjectName::internalMethodName, this, arguments
     *        )
     *      )
     *    );
     * }
     */

    std::string getPizza(std::string pizzaName){
      return *reinterpret_cast<std::string*>(
          invokeMethod(
            boost::bind(
              &PizzaDeliverySystem::internalGetPizza, this, pizzaName
              )
            )
          );
    }

  private:

    /* all private methods follow that pattern:
     * void* internalMethodName(arguments){
     *  pointer = do_some_stuff_here_and_return_a_pointer_to_the_result();
     *  return reinterpret_cast<void*>(pointer);
     * }
     */

    void* internalGetPizza(std::string pizzaName){
      std::cout<<"-- Chef: I'm making a delicious "<<pizzaName<<" pizza..."<<std::endl;
      sleep(3);

      std::string * pizza = new std::string(pizzaName+" and green peppers, mushrooms, olives, chives");

      return reinterpret_cast<void*>(pizza);
    }
};

class MeInMyHouse {
  public:
    MeInMyHouse (){};
    ~MeInMyHouse (){};

    void letsEatPizza(){
      /*
       * In external classes you run their own methods 
       * calling methods of an Asynchronous object.
       * You run them in separate threads
       * and join the threads  when you need the result.
       */

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
      std::cout<<"Got my first pizza, it has: "<<pizza1<<std::endl;
      sleep(1);

      pizzaOrder2.join();
      std::cout<<"Got my second pizza, it has: "<<pizza2<<std::endl;

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
