////////////////////////////////////////////////////////////////////////////////
// OledMenu - by f3rcode (based on SerialMenu (Dan Truong's library)
////////////////////////////////////////////////////////////////////////////////

#include <OledMenu.hpp>

OledMenu* OledMenu::singleton = nullptr;
uint8_t OledMenu::portStatus =  uint8_t(0);

OledMenu::OledMenu() :
    oled(WIDTH, HEIGHT, &Wire, OLED_RESET),
    oldCursor(0),
    cursor(0),
    size(0),
    number(0),
    inNumberMenu(false)
{
    Serial.begin(9600);
    while (!Serial);

    pcIntInit();
    pinMode(ENTER_BUTTON, INPUT);
    digitalWrite(ENTER_BUTTON, HIGH); //PULLUP
    pinMode(UP_BUTTON, INPUT);
    digitalWrite(UP_BUTTON, HIGH); //PULLUP
    pinMode(DOWN_BUTTON, INPUT);
    digitalWrite(DOWN_BUTTON, HIGH); //PULLUP
}

void OledMenu::pcIntInit()
   {
   PCMSK0 |= (1 << PCINTx(0))|(1 << PCINTx(1))|(1 << PCINTx(2));  //enables pin <--PREFERABLY NOT HARDCODED!
   PCIFR |= (1 << PCIF0); //clear any oustanding interrupt
   PCICR |= (1 << PCIE0); //enables interrupt for the group PCINT0..7 corresponding to portB (D8 to D13)
   }

//static
OledMenu &OledMenu::get()
  {
      if (!singleton)
        singleton = new OledMenu;

      return *singleton;
  }

//static
const OledMenu& OledMenu::get(OledMenuEntry* array, uint8_t arraySize)
  {
      (void) OledMenu::get();
      singleton->load(array, arraySize);
      return *singleton;
  }

//method to init Oled
//Wire::begin() hangs when Oled::begin() called from OledMenu constructor
void OledMenu::OledBegin()
{
  Wire.begin();         // initialize I2C
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize screen with address 0x3C
}

void OledMenu::show()
{
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(TEXTSIZE);


    if (!inNumberMenu)
    {
      if (cursor != 0)
      {
        oled.setCursor(10, HEIGHT/2);
        oled.print("<");
      }
      oled.setCursor(WIDTH/4, HEIGHT/2);
      // String in PROGMEM Flash, move it via a SRAM buffer to print it
      char buffer[PROGMEM_BUF_SIZE];
      const char * progMemPt = menu[cursor].message;
      uint8_t len = strlcpy_P(buffer, progMemPt, PROGMEM_BUF_SIZE);
      oled.print(buffer);
      while (len >= PROGMEM_BUF_SIZE)
      {
        len -= PROGMEM_BUF_SIZE - 1;
        progMemPt += PROGMEM_BUF_SIZE - 1;
        len = strlcpy_P(buffer, progMemPt, PROGMEM_BUF_SIZE);
        oled.print(buffer);
      }
      if (cursor != size -1)
      {
        oled.setCursor(WIDTH-10, HEIGHT/2);
        oled.print(">");
      }
      oled.display();
    }
    else
    {
      //void print (const char* text1, const char* text2)
      oled.setCursor(WIDTH/4, 10);
      oled.print(menu[0].message); //VORSICHT! MENU[0].MESSAGE HAS NO STATIC DATA... SOMETHING IS OVERWRITING THIS CHAR* FIELDS... OVERFLOW SOMEWHERE?
      oled.setCursor(WIDTH/4, HEIGHT/2);//TODO TESTS
      //reckonNumberMenu();
      //oled.print(menu[1].message);
      oled.print(number-1);
      oled.print("   [");
      oled.print(number);
      oled.print("]   ");
      oled.print(number+1);
      oled.display();
    }

}


void OledMenu::print(const char* text, const uint8_t delayMs)
{
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(TEXTSIZE);
  oled.setCursor(0, 0);
  oled.print(text);
  oled.display();
  if (delayMs != 0 )
  {
    delay(delayMs);
    show();
  }
}

void OledMenu::print (const char* text1, const char* text2)
{
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(TEXTSIZE);
  oled.setCursor(0, 0);
  oled.print(text1);
  oled.setCursor(0, CHARSIZE*TEXTSIZE);
  oled.print(text2);
  oled.display();
}

void OledMenu::print(int integer, const uint8_t delayMs)
{
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(TEXTSIZE);
  oled.setCursor(0, 0);
  oled.print(integer);
  oled.display();
  if (delayMs != 0 )
  {
    delay(delayMs);
    show();
  }
}

void OledMenu::print (float number)
{
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(TEXTSIZE);
  oled.setCursor(0, 0);
  oled.print(number);
  oled.display();
}

void OledMenu::print (float number1,float number2)
{
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(TEXTSIZE);
  oled.setCursor(0, 0);
  oled.print(number1);
  oled.setCursor(0, CHARSIZE*TEXTSIZE);
  oled.print(number2);
  oled.display();
}

void OledMenu::print (float number1,float number2,float number3)
{
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(TEXTSIZE);
  oled.setCursor(0, 0);
  oled.print(number);
  oled.setCursor(0, CHARSIZE*TEXTSIZE);
  oled.print(number2);
  oled.setCursor(12*CHARSIZE*TEXTSIZE, CHARSIZE*TEXTSIZE);
  oled.print(number3);
  oled.display();

}

void OledMenu::print (float number1,float number2,float number3,const char* text)
{
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setTextSize(TEXTSIZE);
  oled.setCursor(0, 0);
  oled.print(number1);
  oled.setCursor(11, 0);
  oled.print(text);
  oled.setCursor(0, CHARSIZE*TEXTSIZE);
  oled.print(number2);
  oled.setCursor(12*CHARSIZE*TEXTSIZE, CHARSIZE*TEXTSIZE);
  oled.print(number3);
  oled.display();
}

/*
* OledMenu::run function mod
*/
void OledMenu::run(const uint16_t loopDelayMs)
{
  if (!inNumberMenu)
  {
    if (oldCursor!=cursor)
    {
      oldCursor=cursor;
      show();
    }

    delay(loopDelayMs);
    return;
 }
 else
 {
   if (cursor<oldCursor)
   {
      number--;
      show();
   }
   else if (cursor>oldCursor)
   {
     number++;
     show();
   }
   cursor=oldCursor;
   delay(loopDelayMs);
 }
}

//static
void OledMenu::enterSelected()
{

  //digitalWrite(5,LOW);//DEBUG

  if (!singleton->inNumberMenu)
  {
    singleton->menu[singleton->cursor].actionCallback();
  }
  else
  {
    singleton->inNumberMenu = !singleton->inNumberMenu;
    //Display former menu
    singleton->cursor = 0;
    singleton->oldCursor = 0;
    delay(400);
    singleton->callbackAux(singleton->number);
  }
 }

//static
void OledMenu::upSelected()
{

  //digitalWrite(5,HIGH);//DEBUG

  if (!singleton->inNumberMenu && singleton->cursor >= singleton->size-1)
  {
    return;
  }
  else
  {
    singleton->cursor++;
  }
  return;
}

 //static
 void OledMenu::downSelected()
{

 //digitalWrite(5,HIGH);//DEBUG

  if (!singleton->inNumberMenu && singleton->cursor <= 0)
  {
    return;
  }
  else
  {
    singleton->cursor--;
  }
}
//ISR_NOBLOCK insert a SEI() instruction right at the beginning
// in order to not defer any other interrupt more than absolutely needed.
// This way, nested interrupts are enabled giving buttons interrupts low priority
ISR(PCINT0_vect, ISR_NOBLOCK)
{
    uint8_t newPortStatus=PINB;
    uint8_t triggerPins=PCMSK0 & (OledMenu::portStatus ^ newPortStatus) & ~newPortStatus; //^ = xor detects a change and the final & detects rising edge


    OledMenu::portStatus=newPortStatus;


   if (triggerPins & _BV(digitalPinToPCMSKbit(UP_BUTTON)))  {OledMenu::upSelected();}

   else if (triggerPins & _BV(digitalPinToPCMSKbit(DOWN_BUTTON)))  {OledMenu::downSelected();}

   else if (triggerPins & _BV(digitalPinToPCMSKbit(ENTER_BUTTON))) {OledMenu::enterSelected();}
}
