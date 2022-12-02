/**
 * @file main.cpp
 *
 * @brief Simulation of Tesco grocery warehouse and delivery service
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

#define NUMOFPICKERS 8
// #define NUMOFWAREHOUSEMEN 1
#define NUMOFCARS 4
#define CAPACITYOFCAR 10

Queue driverWithCar;
Queue orderPacked("Objednavky ready na expediciu");
Queue FrozenPacked("Mražené objednávky ready na expedici");
Queue DrugsPacked("Drogerie ready na expedici");
Queue DurablesPacked("Trvanlivé zboží ready na expedici");
Queue IncomingOrder("Přišla nová objednávka");

Store Picking("Picking", NUMOFPICKERS, IncomingOrder);
// Store WarehouseWork("Warehouse", NUMOFWAREHOUSEMEN);
Store Car("Car", NUMOFCARS);

Histogram Table("Príprava objednávky až po expedíciu",0,3,20);
Histogram Table2("Nakládka tovar kuriérovi",0,3,20);


/*
-----------------------------------------------
****************   Functions   ****************
-----------------------------------------------
*/
int random_gen(int min,int max){
  std::random_device rd; // obtain a random number from hardware
  std::mt19937 gen(rd()); // seed the generator
  std::uniform_int_distribution<> distr(min, max); // define the range
  return(distr(gen));
}

double random_gen_double(double min, double max){
  double f = (double)rand() / RAND_MAX;
  return min + f * (max - min);
}



/*
-----------------------------------------------
****************   Variables   ****************
-----------------------------------------------
*/
int finalized_orders = 0;
int shifts = 3;
int part_of_day = 0;
int remaining_orders = random_gen(10,30);
bool opened = false;



/*
-----------------------------------------------
*****************   Process   *****************
-----------------------------------------------
*/

//delivery in process
class DeliveryProcess : public Process{
  void Behavior(){
    if (random_gen_double(0.0, 100.0) > 5){     //5% failure rate
      Wait(random_gen(120,240));
      Leave(Car, 1);
    } else {
      Wait(random_gen(240,350));
      Leave(Car, 1);
    }
  }
};

class PackFrozen : public Process{
    void Behavior(){
        if (random_gen_double(0.0, 100.0) > 0.7){   //0.7% failure rate
          Into(FrozenPacked);
          Passivate();
        } else {
          Into(IncomingOrder);
          Passivate();
        }
    }
};

class PackDurables : public Process{
    void Behavior(){
        if (random_gen_double(0.0, 100.0) > 0.3){   //0.3% failure rate
          Into(DurablesPacked);
          Passivate();
        } else {
          Into(IncomingOrder);
          Passivate();
        }
    }
};

class PackDrugs : public Process{
    void Behavior(){
        if (random_gen_double(0.0, 100.0) > 0.5){   //0.5% failure rate
          Into(DrugsPacked);
          Passivate();
        } else {
          Into(IncomingOrder);
          Passivate();
        }
    }
};

//load orders into a delivery cars, num of orders is devided equaly into each car
class LoadOrder : public Process{
  double zaciatok_nakladky;
  void Behavior(){
    zaciatok_nakladky = Time;
    unsigned int order_to_be_taken = FrozenPacked.Length();
    //setting up cars maximal capacity
    if (order_to_be_taken <= CAPACITYOFCAR){
      for(unsigned int i=0;i<order_to_be_taken;i++){
        (FrozenPacked.GetFirst())->Activate();
        (DrugsPacked.GetFirst())->Activate();
        (DurablesPacked.GetFirst())->Activate();
        Wait(5);
      }
    } else{
      for(unsigned int i=0;i<CAPACITYOFCAR;i++){
        (FrozenPacked.GetFirst())->Activate();
        (DrugsPacked.GetFirst())->Activate();
        (DurablesPacked.GetFirst())->Activate();
        Wait(5);
      }
    }

    Table2(Time-zaciatok_nakladky);

    (new DeliveryProcess)->Activate();
  }
};

//order handler
class Order : public Process{ 
  double Prichod;
  void Behavior(){
    Prichod = Time;
      Enter(Picking);
      Wait(random_gen_double(20.0,40.0));
      Leave(Picking,1);

      // Enter(WarehouseWork);
      // Wait(random_gen_double(3.0,6.0));
      // Leave(WarehouseWork,1);

      Table(Time-Prichod);

      (new PackFrozen)->Activate();
      (new PackDurables)->Activate();
      (new PackDrugs)->Activate();
  }
};

//driver takes a car and is ready for loading orders
class Driver : public Process{
    double Prichod;
    void Behavior(){
        Prichod = Time;
        Enter(Car);
        (new LoadOrder)->Activate();
    }
};


/*
----------------------------------------------
*****************   Events   *****************
----------------------------------------------
*/
//input generator
class OrderGenerator : public Event{
  void Behavior(){
    while (remaining_orders>0){
      (new Order)->Activate(); 
      Activate(Time);
      remaining_orders--;
    }
    //number of orders is generated differently throughout the day
    switch(part_of_day){
      case 0:
        (new Order)->Activate();
        Activate(Time+random_gen_double(15.0,30.0));
        break;
      case 1:
        (new Order)->Activate();
        Activate(Time+random_gen_double(5.0,10.0));
        break;
      case 2:
        (new Order)->Activate();
        Activate(Time+random_gen_double(3.0,5.0));
        break;
      case 3:
        (new Order)->Activate();
        Activate(Time+random_gen_double(2.5,4.5));
        break;
      case 4:
        (new Order)->Activate();
        Activate(Time+random_gen_double(3.0,5.0));
        break;
      default:
        (new Order)->Activate();
        Activate(Time+random_gen_double(2.0,5.0));
    }
  }
};

//driver generator
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

//Part of the day generator
class TimeOfDayGen : public Event{
  void Behavior(){
    if (Time < 360){                    //store is closed before 6am
      part_of_day = 0;
    } else if(Time < 660){              //before 11am
      part_of_day = 1;
    } else if(Time < 780){              //before 1pm
      part_of_day = 2;
    } else if(Time < 1080){             //before 6pm
      part_of_day = 3;
    } else if(Time > 1320) {            //store is closed after 10pm
      part_of_day = 4;
    }
    Activate(Time+100);
  }
};

int main(){
  Init(0,1440);                          //time limit is set for 16hours (one day)
  (new TimeOfDayGen)->Activate();
  (new OrderGenerator)->Activate();     //order generator
  (new DriverGenerator)->Activate(120);    //driver generator
  Run();                                //run simulation
  Picking.Output();                     //print of results
  IncomingOrder.Output();
  // WarehouseWork.Output();
  Car.Output();
  Table.Output();
  Table2.Output();
  return 0;
}
