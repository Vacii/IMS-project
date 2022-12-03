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

//Will change at 6am
#define NUMOFPICKERS 0
#define NUMOFWAREHOUSEMEN 0

#define NUMOFCARS 4
#define CARCAPACITY 12

Queue driverWithCar;
Queue FrozenPacked("Objednávky ready na expedici");
Queue DrugsPacked("Drogerie ready na expedici");
Queue DurablesPacked("Trvanlivé zboží ready na expedici");
Queue IncomingOrder("Přišla nová objednávka");
Queue WarehouseQ("Objednávka čeká na příjmutí skladem");

Queue WaitingOrders("Objednávky čekající na další den");

Store Picking("Picking", NUMOFPICKERS, IncomingOrder);
Store WarehouseWork("Warehouse", NUMOFWAREHOUSEMEN, WarehouseQ);
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
int remaining_orders = random_gen(20,50);
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
      Into(FrozenPacked);
      Passivate();
    }
};

class PackDurables : public Process{
    void Behavior(){
      Into(DurablesPacked);
      Passivate();
    }
};

class PackDrugs : public Process{
    void Behavior(){
      Into(DrugsPacked);
      Passivate();
    }
};

class PackSeparator : public Process{
    void Behavior(){
      double failureFrozen = random_gen_double(0.0, 100.0);
      double failureDurables = random_gen_double(0.0, 100.0);
      double failureDrugs = random_gen_double(0.0, 100.0);

      //when there was no failure
      if (
          failureFrozen > 0.7 &&
          failureDurables > 0.3 &&
          failureDrugs > 0.5
      ) {
        (new PackFrozen)->Activate();
        (new PackDurables)->Activate();
        (new PackDrugs)->Activate();
      } else{     //failure handler
        IncomingOrder.Insert(this);
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
    if (order_to_be_taken <= CARCAPACITY){
      for(unsigned int i=0;i<order_to_be_taken;i++){
        if (FrozenPacked.Length() == 0){
          break;
        }
        (FrozenPacked.GetFirst())->Activate();
        (DrugsPacked.GetFirst())->Activate();
        (DurablesPacked.GetFirst())->Activate();
        Wait(5);
      }
    } else{
      for(unsigned int i=0;i<CARCAPACITY;i++){
        if (FrozenPacked.Length() == 0){
          break;
        }
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

      //while store is not open, just add to picking queue and passivate.
      if (Picking.Capacity() < 1){
        //when store is after oppening hours, insert new order request into orders for next day
        if (part_of_day == 4){
          Into(WaitingOrders);
          Passivate();
          return;
        }
        Into(IncomingOrder);
        Passivate();
        return;
      }

      Enter(Picking);
      Wait(random_gen_double(15.0,30.0));
      //when the store closes, picker drops his current order, puts it into waiting queue (for another day) and leaves
      if (Time >= 1320){
        while (Picking.Used() != 0){
          Leave(Picking,1);
        }
        while (IncomingOrder.Length() != 0){
          WaitingOrders.Insert(IncomingOrder.GetFirst());
          Passivate();
        }
        
        Picking.SetCapacity(0);
        return;
      }
      Leave(Picking,1);

      Enter(WarehouseWork);
      Wait(random_gen_double(3.0,6.0));
      if (Time >= 1320){

        while (WarehouseWork.Used() != 0){
          Leave(WarehouseWork,1);
        }

        while (WarehouseQ.Length() != 0){
          WaitingOrders.Insert(WarehouseQ.GetFirst());
          Passivate();
        }

        WarehouseWork.SetCapacity(0);
        return;
      }
      Leave(WarehouseWork,1);
      Table(Time-Prichod);

      (new PackSeparator)->Activate();
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
        Activate(Time+random_gen_double(5.0,10.0));
        break;
      case 4:
        (new Order)->Activate();
        Activate(Time+random_gen_double(12.0,15.0));
        break;
      default:
        (new Order)->Activate();
        Activate(Time+random_gen_double(15.0,30.0));
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
      if (Picking.Capacity() == 0){
        Picking.SetCapacity(8);
        WarehouseWork.SetCapacity(1);
      }
    } else if(Time < 780){              //before 1pm
      part_of_day = 2;
    } else if(Time < 1080){             //before 6pm
      part_of_day = 3;
    } else if(Time >= 1320) {            //store is closed after 10pm
      part_of_day = 4;
      while(FrozenPacked.Length() != 0){
        WaitingOrders.Insert(FrozenPacked.GetFirst());
        Passivate();
      }
      DrugsPacked.clear();
      DurablesPacked.clear();
    }
    Activate(Time+5);
  }

};

int main(){
  Init(0,1440);                          //time simulates 24h (one day)
  (new TimeOfDayGen)->Activate();
  (new OrderGenerator)->Activate();     //order generator
  (new DriverGenerator)->Activate(480);    //driver generator, starts at 8am
  Run();                                //run simulation
  Picking.Output();                     //print of results
  IncomingOrder.Output();
  WarehouseWork.Output();
  WarehouseQ.Output();
  FrozenPacked.Output();
  Car.Output();
  WaitingOrders.Output();
  Table.Output();
  Table2.Output();
  return 0;
}
