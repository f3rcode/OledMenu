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
struct OledMenuEntry
{
  char * message;
  void (*actionNumberCallback)(void (*callbackFunction)(int));
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
    static OledMenuEntry menu[2];
    // number of entries in the current menu
    uint8_t size;

    //submenu attributes
    uint8_t numberMenuSize;
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

    //init Oled
    void OledBegin();

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

      number=startingValue;
      Serial.println(number);

      menu[0].message = message;
      callbackAux = callback;

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
