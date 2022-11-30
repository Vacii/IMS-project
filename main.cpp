/**
 * @file main.cpp
 *
 * @brief Simulation of Tesco grocery delivery service
 *
 * @author Pavol Babjak - xbabja03
 * @author Lukáš Václavek - xvacla32
 *
 */

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h> 
#include <simlib.h>
#include <random>

//Time is in minutes


//TODO Nemáme moc domyšlené objednávky, ale určitě ne všechny mají být na aktuální den
//TODO beru jen jeden nákup k rozvozu, potřeba implementovat jaký počet nákupů se bude brát

#define NUMOFPICKERS 10
#define NUMOFWAREHOUSEMEN 1
#define NUMOFCARS 4

int finalized_orders = 0;
int shifts = 3;


//int remaining_orders = Random(10,30);

  std::random_device rd; // obtain a random number from hardware
  std::mt19937 gen(rd()); // seed the generator
  std::uniform_int_distribution<> distr(10, 30); // define the range
  //std::cout << distr(gen) << ' '; // generate numbers

int remaining_orders = distr(gen);

bool opened = false;

Store Picking("Picking", NUMOFPICKERS);
Store WarehouseWork("Warehouse", NUMOFWAREHOUSEMEN);
Store Car("Car", NUMOFCARS);
//Facility LoadingCar("Loading order");
//Facility Delivery("Delivering order");


Histogram Table("Table",0,3,20);

Queue driverWithCar;
Queue orderPacked("Objednavky ready na expediciu");


class DeliveryProcess : public Process{
  void Behavior(){
    //Seize(Delivery);
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(120, 240); // define the range
    Wait(distr(gen));
    Leave(Car, 1);
    //Release(Delivery);
  }
};

class LoadOrder : public Process{
  void Behavior(){
    //driverWithCar.GetFirst()->Activate();
    //Seize(LoadingCar);
    unsigned int order_to_be_taken = orderPacked.Length() / 4;
    for(unsigned int i=0;i<order_to_be_taken;i++)
    {
      (orderPacked.GetFirst())->Activate();
      Wait(5);
    }
   
    //Release(LoadingCar);

    (new DeliveryProcess)->Activate();
  }
};


/*class GroceryPicking : public Process {

    void Behavior() {
        Enter(Picking);
        Wait(2);
        Wait(Exponential(5));
        Wait(2)
        Leave(Picking, 1);
    }

};*/


//order handler
class Order : public Process{ 
  double Prichod;
  void Behavior(){
    Prichod = Time;
    //int products_count;
    //if (opened)
    //{
      Enter(Picking);
      std::random_device rd; // obtain a random number from hardware
      std::mt19937 gen(rd()); // seed the generator
      std::uniform_int_distribution<> distr(8, 12); // define the range
      Wait(distr(gen));
      Leave(Picking, 1);

      Enter(WarehouseWork);
      //std::random_device rd; // obtain a random number from hardware
      //std::mt19937 gen(rd()); // seed the generator
      std::uniform_int_distribution<> distr2(3, 6); // define the range
      Wait(distr2(gen));
      Leave(WarehouseWork, 1);

      Table(Time-Prichod);

      Into(orderPacked);
      Passivate();
   //}
  
    
   // (new GroceryPicking)->Activate();
   // (new CooledandFrozenPicking)->Activate();

    

   // finalized_orders++;

   /* if(driverWithCar.Length() > 0){
      // driverWithCar.GetFirst()->Activate();
      orderPacked.GetFirst()->Activate();
      (new LoadOrder)->Activate();
    } else {
      Into(orderPacked);
      Passivate();
    } */

      
    

              //waiting and service time
  }
};

class Driver : public Process{
    double Prichod;
    void Behavior(){
        Prichod = Time;
        Enter(Car);
        (new LoadOrder)->Activate();
        //Into(driverWithCar);
        //Passivate();


    }
};

//input generator
class OrderGenerator : public Event{
  void Behavior(){
    while (remaining_orders>0)
    {
      (new Order)->Activate(); 
      Activate(Time);
      remaining_orders--;
    }

    /*if (Time==0)
    {
      (new Order)->Activate();        //new order
      Activate(Time+360+Exponential(5));
      Wait(360);
    }
    else
    {
      (new Order)->Activate();        //new order
      Activate(Time+Exponential(5));  //new order comes every 5 mins exp
    }*/

    (new Order)->Activate();        //new order
      Activate(Time+Exponential(5));  //new order comes every 5 mins exp


    
    
  }
};

class DriverGenerator : public Event{
  void Behavior(){
    if (shifts>0)
    {
      for (int i=0;i<4;i++)
      {
        (new Driver)->Activate();        //new driver
        Activate(Time+240);  //every 4h
      }
      shifts--;
    }
    
                
  }
};

class Operate : public Event{
  void Behavior()
  {
    if (!opened)
    {
      opened=!opened;
      Activate(Time+960);
    }
    else
      {
        opened=!opened;
        Activate(Time+480);
      }
                
  }
};

int main(){
  printf("xxx %d",remaining_orders);
  Init(0,1440);                          //time limit is set for 16hours (one day)
  (new OrderGenerator)->Activate(360);     //order generator
  (new DriverGenerator)->Activate(480);    //driver generator
  Run();                                //run simulation
  Picking.Output();                     //print of results
  WarehouseWork.Output();
  Car.Output();
  printf("xxxxxxxx \n");
  orderPacked.Output();
  //LoadingCar.Output();
  //Delivery.Output();
  Table.Output();
  return 0;
}