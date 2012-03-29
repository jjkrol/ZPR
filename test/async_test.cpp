#include "../include/asynchronous.hpp"

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
     *  return returnByValueConcurrent<returntype>(
     *  return returnByValueQueued<returntype>(
     *  return returnByPointerConcurrent<returntype>(
     *  return returnByPointerQueued<returntype>(
     *      boost::bind(
     *        &ObjectName::internalMethodName, this, arguments
     *        )
     *    );
     * }
     */
    std::string getPizza(std::string pizzaName){
      return returnByValueQueued<std::string>(
            boost::bind(
              &PizzaDeliverySystem::internalGetPizza, this, pizzaName
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
