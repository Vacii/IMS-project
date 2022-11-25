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

#define NUMOFPICKERS 10
#define NUMOFWAREHOUSEMEN 1
#define NUMOFCARS 1

Store  Picking("Picking", NUMOFPICKERS);
Store WarehouseWork("Warehouse", NUMOFWAREHOUSEMEN);
Facility Car("Car");
Histogram Table("Table",0,25,20);

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


    Table(Time-Prichod);          //waiting and service time
  }
};

//Nevim jak implementovat generovani ridice a predani objednavky k nemu
class Driver : public Process{
    double Prichod;
    void Behavior(){
        Prichod = Time;
        Seize(Car);




    }
    void LoadOrder(){

    }
};

//input generator
class OrderGenerator : public Event{
  void Behavior(){
    (new Order)->Activate();        //new order
    Activate(Time+Exponential(1));  //every minute
  }
};

class DriverGenerator : public Event{
  void Behavior(){
    (new Driver)->Activate();        //new driver
    Activate(Time+Exponential(240)); //every 4h
  }
};

int main(){
  Init(0,960);                          //time limit is set for 16hours (one day)
  (new OrderGenerator)->Activate();     //order generator
  (new DriverGenerator)->Activate();    //driver generator
  Run();                                //run simulation
  Picking.Output();                     //print of results
  WarehouseWork.Output();
  Table.Output();
  return 0;
}