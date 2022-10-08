#include <OledMenu.hpp>

int8_t hoursLimit = 0;
int8_t warningTemperature = 0;

void foo(uint8_t number);

OledMenu& menu = OledMenu::get();


void foo(uint8_t number)
{
  Serial.print("Here we are: ");
  Serial.println(number);
  //int
  menu.print(number,(uint8_t) 500);
}

void setup() 
{
  Serial.begin(9600);
  while (!Serial){}; //<---Serial init on OledMenu constructor

  //LED on 5th. Debugging purpose
  pinMode(5, INPUT);
  digitalWrite(5, HIGH); //PULLUP

  menu.OledBegin();
  menu.getNumber("Temp: ", 8, [](int v){
                        Serial.println("Inside First");
                        warningTemperature = v;
                        menu.getNumber("Time (hours): ", 5, [](int v){
                          Serial.println("Inside Second");
                          hoursLimit = 2 * v;
                          menu.print("Go!");
                          foo(hoursLimit);});
                        });
}

void loop() 
{
   menu.run(200);
}
