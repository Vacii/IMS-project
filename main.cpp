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

//Time is in minutes


//TODO Nemáme moc domyšlené objednávky, ale určitě ne všechny mají být na aktuální den
//TODO beru jen jeden nákup k rozvozu, potřeba implementovat jaký počet nákupů se bude brát

#define NUMOFPICKERS 10
#define NUMOFWAREHOUSEMEN 1
#define NUMOFCARS 1

Store  Picking("Picking", NUMOFPICKERS);
Store WarehouseWork("Warehouse", NUMOFWAREHOUSEMEN);
Store Car("Car", NUMOFCARS);
Facility LoadingCar("Loading order");
Facility Delivery("Delivering order");


Histogram Table("Table",0,25,20);

Queue driverWithCar;
Queue orderPacked;

class DeliveryProcess : public Process{
  double Prichod;
  void Behavior(){
    Seize(Delivery);
    Wait(Exponential(180));
    Release(Delivery);
  }
};

class LoadOrder : public Process{
  double Prichod;
  void Behavior(){
    driverWithCar.GetFirst()->Activate();
    Seize(LoadingCar);
    Wait(8);
    Release(LoadingCar);
    Leave(Car, 1);
    (new DeliveryProcess)->Activate();
  }
};



//order handler
class Order : public Process{ 
  double Prichod;
  void Behavior(){
    Prichod = Time;
    Enter(Picking);
    Wait(10);
    Leave(Picking, 1);
    Enter(WarehouseWork);
    Wait(Exponential(3));
    Leave(WarehouseWork, 1);

    if(driverWithCar.Length() > 0){
      // driverWithCar.GetFirst()->Activate();
      orderPacked.GetFirst()->Activate();
      (new LoadOrder)->Activate();
    } else {
      Into(orderPacked);
      Passivate();
    }
    

    Table(Time-Prichod);          //waiting and service time
  }
};

class Driver : public Process{
    double Prichod;
    void Behavior(){
        Prichod = Time;
        Enter(Car);

        Into(driverWithCar);
        Passivate();


    }
};

//input generator
class OrderGenerator : public Event{
  void Behavior(){
    (new Order)->Activate();        //new order
    Activate(Time+Exponential(5));  //new order comes every 5 mins exp
  }
};

class DriverGenerator : public Event{
  void Behavior(){
    (new Driver)->Activate();        //new driver
    Activate(Time+240);              //every 4h
  }
};

int main(){
  Init(0,960);                          //time limit is set for 16hours (one day)
  (new OrderGenerator)->Activate();     //order generator
  (new DriverGenerator)->Activate(120);    //driver generator
  Run();                                //run simulation
  Picking.Output();                     //print of results
  WarehouseWork.Output();
  Car.Output();
  LoadingCar.Output();
  Delivery.Output();
  Table.Output();
  return 0;
}