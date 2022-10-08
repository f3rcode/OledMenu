#ifndef OledMENU_h
#define OledMENU_h

///////////////////////////////////////////////////////////////////////////////
// OledMenu (by f3rcode) -
// Menu Library for Arduino Oled based on SerialMenu (Dan Truong's library)
// [Lots of comments remain the same from SerialMenu library]
///////////////////////////////////////////////////////////////////////////////
// Notes (SerialMenu library's):
/////////
// Be carefuly with memory constrained boards, running out of SRAM will lead
// to flaky code behavior. Keep menu text minimal as it could eat up memory.
// To alleviate memory pressure menus can be stored in Flash program memory
// using the PROGMEM keyword, instead of SRAM data memory. However, even Flash
// memory is limited.
//
// Menu callback functions can be declared as separate functions or as lambda
// functions directly in the data structure with the menu data. Lambda notation
// is best for simple tasks like setting a global variable or calling another
// menu, as it keeps the code for a menu entry concise and within the menu
// entry definition. See the example.
//

#include <avr/pgmspace.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//////////////f3rcode//////////////
// Oled display
//////////////////////////////////
#define WIDTH 128
#define HEIGHT 64
#define OLED_RESET 4
#define TEXTSIZE 1
#define CHARSIZE 6
//buttons
#define ENTER_BUTTON 8 //PB0 = D8
#define UP_BUTTON 10 //PB1 = D9
#define DOWN_BUTTON 9 //PB2 = D10
//PROGMEM
#define PROGMEM_BUF_SIZE 8
//int
#define PCINTx(a) PCINT ## a //a MUST be in a range [0,7] corresponding to PB0..7 i.e. D8..13

///////////////////////////////////////////////////////////////////////////////
// Macro to get the number of menu entries in a menu array.
///////////////////////////////////////////////////////////////////////////////
#define GET_MENU_SIZE(menu) sizeof(menu)/sizeof(OledMenuEntry)

///////////////////////////////////////////////////////////////////////////////
// Define a menu entry as:
// - a menu message to display
// - a callback function to perform the menu action
///////////////////////////////////////////////////////////////////////////////
class OledMenuEntry
{
public:
  char * message;
  void (*actionCallback)();
  void (*actionNumberCallback)(void (*callbackFunction)(int));

  OledMenuEntry(){};

  OledMenuEntry(char* message, void (*actionCallback)()):
  message(message),
  actionCallback(actionCallback)
  {}

  OledMenuEntry(char* message, void (*actionNumberCallback)(void (*callbackFunction)(int))):
  message(message),
  actionNumberCallback(actionNumberCallback)
  {}

};

///////////////////////////////////////////////////////////////////////////////
// The menu is a singleton class in which you load an array of menu entries.
///////////////////////////////////////////////////////////////////////////////
class OledMenu
{
  private:
    // This class implements a singleton design pattern with one static instance
    static OledMenu * singleton;
    // Points to the array of menu entries for the current menu
    OledMenuEntry * menu;
    // number of entries in the current menu
    uint8_t size;

    //attributes to change to submenu
    OledMenuEntry * formerMenu;
    uint8_t formerMenuSize;

    //submenu attributes
    //char getNumberMenuIntro[15];
    OledMenuEntry getNumberMenu[1];
    uint8_t numberMenuSize;
    boolean inNumberMenu;
    void (*callbackAux)(int);

    //Oled navigation
    Adafruit_SSD1306 oled;
    int8_t cursor;
    int8_t oldCursor;

    //Constructor: init with an empty menu, prepares Oled screen
    OledMenu();

    void pcIntInit();

  public:

    static uint8_t portStatus;

    //getNumber value
    uint16_t number;

    // Get a pointer to the one singleton instance of this class
    static OledMenu& get();
    // Get a pointer to the one singleton instance of this class and point it
    // to the current menu
    static const OledMenu& get(OledMenuEntry* array, uint8_t arraySize);

    //init Oled
    void OledBegin();

    // Install the current menu to display
    inline void load(OledMenuEntry* array, uint8_t arraySize)
    {
      //reset screen values
      cursor=0;
      oldCursor=0;

      menu = array;
      size = arraySize;
    }

    // Display the current menu on the Serial console
    void show();

    // Following members use Oled print, so there is no need of instanciating
    // a new object if we just want to show a result on screen, for instance.
    void print(const char* text, const uint8_t delayMs = 0); // prints text and shows menu after delayMs if set
    void print (const char* text1, const char* text2);
    void print(int integer, const uint8_t delayMs = 0);  // prints text and shows menu after delayMs if set
    void print (float number);
    void print (float number1,float number2);
    void print (float number1,float number2,float number3);
    void print (float number1,float number2,float number3, const char* text);


    // getNumber passes a chosen number as a parameter to a callback
    // Note: this routine is showing a special menu
    // so user can choose a number and select it by using the buttons
    void getNumber(const char* message, const uint16_t startingValue, void (*callback)(int)){

      formerMenu=menu;
      formerMenuSize=size;

      //TODO:SHOULD IT BE DECLARED OUT OF THE FUNCTION SO THE MICROCONTROLLER HAS RESERVED
      //ENOUGH MEMORY FROM THE BEGINNING, AND THEREFORE CAN ALLOCATE THIS SECONDARY MENU?
      getNumberMenu[0] = {"", [](){}, [](void (*callbackFunction)(int)){
          singleton->inNumberMenu = !singleton->inNumberMenu;
          //Display former menu
          singleton->load(singleton->formerMenu, singleton->formerMenuSize);
          singleton->cursor = 0;
          singleton->oldCursor = 0;
          Serial.println(singleton->number);
          callbackFunction(singleton->number);
          singleton->show();
        });

      numberMenuSize = GET_MENU_SIZE(getNumberMenu);

      number=startingValue;
      Serial.println(number);

      sprintf(getNumberMenu[0].message, "%s", message);
      callbackAux = callback;
      inNumberMenu=true;

      load(getNumberMenu,numberMenuSize);
      show();
    }
///////////////////////////////////////////////////////////////////////////////
    // Run the menu
    void run(const uint16_t loopDelayMs);

    static void enterSelected();
    static void upSelected();
    static void downSelected();
};

 #endif
