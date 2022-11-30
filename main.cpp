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

#define NUMOFPICKERS 10
#define NUMOFWAREHOUSEMEN 1
#define NUMOFCARS 4

int random_gen(int min,int max)
{
  std::random_device rd; // obtain a random number from hardware
  std::mt19937 gen(rd()); // seed the generator
  std::uniform_int_distribution<> distr(min, max); // define the range
  return(distr(gen));
}

int finalized_orders = 0;
int shifts = 3;

int remaining_orders = random_gen(10,30);
bool opened = false;

Store Picking("Picking", NUMOFPICKERS);
Store WarehouseWork("Warehouse", NUMOFWAREHOUSEMEN);
Store Car("Car", NUMOFCARS);

Histogram Table("Príprava objednávky až po expedíciu",0,3,20);
Histogram Table2("Nakládka tovar kuriérovi",0,3,20);

Queue driverWithCar;
Queue orderPacked("Objednavky ready na expediciu");


class DeliveryProcess : public Process{
  void Behavior(){
    Wait(random_gen(120,240));
    Leave(Car, 1);
  }
};

class LoadOrder : public Process{
  double zaciatok_nakladky;
  void Behavior(){
    zaciatok_nakladky = Time;
    unsigned int order_to_be_taken = orderPacked.Length() / 4;
    for(unsigned int i=0;i<order_to_be_taken;i++)
    {
      (orderPacked.GetFirst())->Activate();
      Wait(5);
    }
    Table2(Time-zaciatok_nakladky);

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
      Wait(random_gen(8,12));
      Leave(Picking,1);

      Enter(WarehouseWork);
      Wait(random_gen(3,6));
      Leave(WarehouseWork,1);

      Table(Time-Prichod);

      Into(orderPacked);
      Passivate();

  }
};

class Driver : public Process{
    double Prichod;
    void Behavior(){
        Prichod = Time;
        Enter(Car);
        (new LoadOrder)->Activate();
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
  Init(0,1440);                          //time limit is set for 16hours (one day)
  (new OrderGenerator)->Activate(360);     //order generator
  (new DriverGenerator)->Activate(480);    //driver generator
  Run();                                //run simulation
  Picking.Output();                     //print of results
  WarehouseWork.Output();
  Car.Output();
  orderPacked.Output();
  Table.Output();
  Table2.Output();
  return 0;
}