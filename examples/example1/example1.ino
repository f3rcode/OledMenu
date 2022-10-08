#include <OledMenu.hpp>

void foo(uint8_t number);

OledMenu& menu = OledMenu::get();

///////////////////////////////////////////////////////////////////////////////
// Main menu
///////////////////////////////////////////////////////////////////////////////
const char PROGMEM mainMenuTempeh[]  = "1 - Tempeh";
const char PROGMEM  mainMenuNatto[]  = "2 - Natto";
const char PROGMEM  mainMenuKoji[]  = "3 - Koji";
const char PROGMEM  mainMenuNumber[]  = "4 - Number Menu";

// Define the main menu
const OledMenuEntry mainMenu[] = {
  {mainMenuTempeh, [](){//prints on Oled and Serial
                                    //and shows menu on Oled again after
                                    //UNTIL PRESSING A BUTTON WHICH
                                    //CHANGES INTERNAL CURSOR VALUE
                                    Serial.println("Option1");
                                    menu.print("Option2");
                                    foo(0);
                                  }},
  {mainMenuNatto, [](){//prints on Oled and serial
                                  //and get the message on Oled screen for n msec.
                                  Serial.println("Option2");
                                  menu.print("Option2",(uint8_t) 500);
                                  foo(1);
                                  }},
  {mainMenuKoji, [](){Serial.println("Still not available.");
                                  menu.print("Still not available.");
                                  }},
  {mainMenuNumber, [](){//menu.print("Starring NumberMenu", (uint8_t) 1000);
                                     menu.getNumber("something", (uint8_t) 8, 
                                     [](int v){
                                        menu.print("setting number",(uint8_t) 500);
                                        menu.print(v,(uint8_t) 500);});
                                  }},
};
constexpr uint8_t mainMenuSize = GET_MENU_SIZE(mainMenu);

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
  menu.load(mainMenu, mainMenuSize);
  // Display current menu (mainMenu)
  menu.show();
}

void loop() 
{
   menu.run(200);
}
