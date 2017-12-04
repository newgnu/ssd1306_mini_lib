#if defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__) | defined(__AVR_AT90Tiny26__) | defined(__AVR_ATtiny26__)
  #include <TinyWireM.h>
  #define SSD1306Wire TinyWireM
  #define SSD1306_BUFFER 16 //TinyWireM only has a buffer of 18 bytes. -1 for the address, and another -1 for the first 'mode' byte, leaves us with 16 usable bytes before we need to bounce the connection.
#else
  #include <Wire.h>
  #define SSD1306Wire Wire
  #define SSD1306_BUFFER 30 //Wire has a buffer of 32 bytes. drop the address and 'mode' bytes, and this is the data we can send.
  #define SSD1306_CLOCK_SPEED 400000L
#endif

/******** CHANGE SCREEN SIZE HERE ********/
//#define SCREEN_128x64
#define SCREEN_128x32

#ifdef SCREEN_128x64
  #define SSD1306_COLUMNS 127
  #define SSD1306_PAGES 7
  #define SSD1306_SCREEN_BYTES 1024
#else //128x32
  #define SSD1306_COLUMNS 127
  #define SSD1306_PAGES 3
  #define SSD1306_SCREEN_BYTES 512
#endif

#ifndef SSD1306_ADDRESS
  #define SSD1306_ADDRESS 0x3C
#endif

#define SSD1306_COMMAND 0x00 //command mode
#define SSD1306_DATA    0x40 //data mode

class SSD1306 {
  public:
    void begin(void);
    void write(uint8_t mode, uint8_t data[], int size);
    void writeP(uint8_t mode, int data, int size);  //same as 'write', but for progmem variables
    void clear();
    void area(uint8_t page_start, uint8_t page_end, uint8_t col_start, uint8_t col_end); //sets a specific draw area
    void fill(uint8_t page_start, uint8_t page_end, uint8_t col_start, uint8_t col_end, uint8_t data);
    void display_off();
    void display_on();
    void contrast(uint8_t value);
};
