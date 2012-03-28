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
 *  loose threads in concurrentLoop
 */

// general classes

class Ticket {
  public:
    void * getPointer(){
      return pointer;
    };

    void setPointer(void * newPointer){
      pointer = newPointer;
    }

    void wait(){
      boost::unique_lock<boost::mutex> lock(mut);
      while(!ready){
        cond.wait(lock);
      }
    }

    void setReady(){
      boost::lock_guard<boost::mutex> lock(mut);
      ready = true;
      cond.notify_all();
    }

  private:
    boost::mutex mut;
    boost::condition_variable cond;
    bool ready;
    void * pointer;
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
      stopLoops = nietego;
      mainLoopThread = boost::thread(&Asynchronous::mainLoop, this) ;
      concurrentLoopThread = boost::thread(&Asynchronous::concurrentLoop, this);
    }

    virtual ~Asynchronous(){
      stopLoops = naturalnie;
      mainLoopThread.join();
      concurrentLoopThread.join();
    }

  protected:
    /*
     * This loop is used for doing tasks from the queue
     * one by one. 
     *
     */
    virtual void mainLoop(){
      while(!stopLoops)  
        if(!normalMessageQueue.isEmpty()){
          Message msg = normalMessageQueue.pop();
          void * output = msg.functionObject() ; 
          Ticket * ticket = msg.returnTicket;
          ticket->setPointer(output); 
          ticket->setReady(); // not really safe

        }
    }

    /* 
     * this loop runs tasks, that can be
     * run in parallel threads
     */

    virtual void concurrentLoop(){ 
      boost::thread threadFromQueue;
      while(!stopLoops)  
        if(!concurrentMessageQueue.isEmpty()){
          Message msg = concurrentMessageQueue.pop();
          threadFromQueue = boost::thread(&Asynchronous::invokeConcurrent, this, msg);
          //TODO what happens with those threads?
        }
    }

    void invokeConcurrent(Message msg){
      void * output = msg.functionObject() ; 
      Ticket * ticket = msg.returnTicket;

      ticket->setPointer(output); 
      ticket->setReady();
    }

    virtual void * invokeQueuedMethod(boost::function<void*()> funct){
/*      //prepare a message
      Message msg;
      msg.functionObject = funct;

      //prepare return ticket
      Ticket * ticket;
      ticket = normalMessageQueue.push(msg);

      //wait for the order to be complete
      ticket->wait();
      return ticket->getPointer();
*/ 
      return invokeMethod(funct, normalMessageQueue);
    }

    virtual void * invokeConcurrentMethod(boost::function<void*()> funct){
/*      //prepare a message
      Message msg;
      msg.functionObject = funct;

      //prepare return ticket
      Ticket * ticket;
      ticket = concurrentMessageQueue.push(msg);

      //wait for the order to be complete
      ticket->wait();
      return ticket->getPointer();
 */
      return invokeMethod(funct, concurrentMessageQueue);
    }
  
    virtual void * invokeMethod(boost::function<void*()> funct, MessageQueue& mq){
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

    MessageQueue normalMessageQueue;
    MessageQueue concurrentMessageQueue;
    bool stopLoops;
    boost::thread mainLoopThread;
    boost::thread concurrentLoopThread;
};

// specific classes

class PizzaDeliverySystem : public Asynchronous {
  public:
    PizzaDeliverySystem(){
      pizzaHouseName = "Kotek";
    }

    /*
     * for simple public methods
     * that do not change anything
     * and do not depend on external objects either
     * we can leave the thread thing alone
     * and just return the value:
     */
    std::string getPizzaHouseName(){
      return pizzaHouseName; 
    }

    /* 
     * public methods that use other objects and/or change something
     * are similar to that pattern:
     *
     * returntype methodName(arguments){
     *  return *reinterpret_cast<returntype*>( if we return by value
     *  OR
     *  return reinterpret_cast<returntype*>( if we return pointer
     *    invokeQueuedMethod(
     *      boost::bind(
     *        &ObjectName::internalMethodName, this, arguments
     *        )
     *      )
     *    );
     * }
     */
    std::string getPizza(std::string pizzaName){
      return *reinterpret_cast<std::string*>(
          invokeQueuedMethod(
            boost::bind(
              &PizzaDeliverySystem::internalGetPizza, this, pizzaName
              )
            )
          );
    }
    /*
     * for public methods that do not change anything but may tak a while
     * (for example certain database queries, disk operations)
     * we can use the concurrent queue, which runs all messages
     * in parallel threads
     * pattern is similar to the previous one, the only difference
     * is that we use invokeConcurrentMethod instead of invokeQueuedMethod
     * try it and see what happens
     * 
     *
     *
     */

  private:

    /* all private methods follow that pattern:
     * void* internalMethodName(arguments){
     *  pointer = do_some_stuff_here_and_return_a_pointer_to_the_result();
     *  return reinterpret_cast<void*>(pointer);
     * }
     */

    void* internalGetPizza(std::string pizzaName){
      std::cout<<"-- Chef: I'm making a delicious "<<pizzaName<<" pizza..."<<std::endl;
      if(pizzaName == "peperoni"){ // show the difference between concurrent and queued message queue
      sleep(5);
      }
      else{
sleep(1);
      }
      std::string * pizza = new std::string(pizzaName+" and green peppers, mushrooms, olives, chives");
      return reinterpret_cast<void*>(pizza);
    }

    std::string pizzaHouseName;
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
      pizzaOrder2.join();

      std::cout<<"Delicious!"<<std::endl;
    }

  private:
    std::string pizza1, pizza2;
    PizzaDeliverySystem pizzaGuys;

    void getPizza1(){
      pizza1 = pizzaGuys.getPizza("peperoni");
      std::cout<<"Got my first pizza, it has: "<<pizza1<<std::endl;
    }

    void getPizza2(){
      pizza2 = pizzaGuys.getPizza("pineapple");
      std::cout<<"Got my second pizza, it has: "<<pizza2<<std::endl;
    }
};


int main(int argc, char** argv){
  MeInMyHouse me;
  std::cout<<"I'm at home, hungry. Let's order some pizza!"<<std::endl;
  sleep(2);

  me.letsEatPizza();

}
