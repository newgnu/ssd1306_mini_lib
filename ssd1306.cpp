#include <avr/pgmspace.h>  //needed for PROGMEM variables
#include "ssd1306.h"

static const uint8_t SSD1306_CONFIG[] PROGMEM = {
//  0xAE,        //'display off'
  0xD5, 0x80,  //'set display clock ratio'. suggested default is 0x80

#ifdef SCREEN_128x64
  0xA8, 63,    //'set multiplex ratio'. line height -1. in this case, a 64px tall screen
  0xDA, 0x12,  //'set com pins' (see data sheet) -- set to 0x12 at reset. use 0x02 for 32px tall oled?
#else //128x32
  0xA8, 31,    //'set multiplex ratio'. line height -1. in this case, a 64px tall screen
  0xDA, 0x02,  //'set com pins' (see data sheet) -- set to 0x12 at reset. use 0x02 for 32px tall oled?
#endif

  0xD3, 0x00,  //'set display offset' to 0 -- this is returned to 0 at reset
  0x40 | 0x00, //'set start line' to 0 (second byte, but really only 6 bits) -- this is returned to 0 at reset
  
  0x20, 0x00,  //'set memory mode' to 0x00 (horizontal addressing)
  0xA0 | 0x01, //'set segment re-map' to 1. column 127 is mapped to seg0
  0xC8,        //'set com output scan direction' to top-down. C0 would be bottom-up
  0x8D, 0x14,  //'set charge pump' to 0x14 (use 0x10 for 'externalvcc')
  0x81, 0xCF,  //'set contrast' to 0xCF. Use 0x9F if external vcc, or 0x8F if 32px tall oled (int or ext vcc)
  0xD9, 0xF1,  //'set precharge' to 0xF1. Use 0x22 if external vcc
//  0xDB, 0x40,  //'set vcom deselect level' to 0x40 (datasheet says this is out of bounds?)
  0xA4,        //'resume to ram display' -- default at reset. 0xA5 ignores ram
  0xA6,        //'display normal' (0xA7 for inverted) -- default at reset
  0xAF         //and finally, 'display on' command
};

void SSD1306::begin(){
  SSD1306Wire.begin();
  #ifdef SSD1306_CLOCK_SPEED
    SSD1306Wire.setClock(SSD1306_CLOCK_SPEED);
  #endif
  SSD1306::writeP(SSD1306_COMMAND, SSD1306_CONFIG, sizeof(SSD1306_CONFIG));
  SSD1306::clear();
}

void SSD1306::write(uint8_t mode, uint8_t data[], int size){
  SSD1306Wire.beginTransmission(SSD1306_ADDRESS);
  SSD1306Wire.write(mode);
  for (int i = 0; i < size; i++){
    if (i % SSD1306_BUFFER >= SSD1306_BUFFER -1){  //reset connection before we run out of buffer space.
      SSD1306Wire.endTransmission();
      SSD1306Wire.beginTransmission(SSD1306_ADDRESS);
      SSD1306Wire.write(mode);
    }
    SSD1306Wire.write(data[i]);
  }
  SSD1306Wire.endTransmission();
}

void SSD1306::writeP(uint8_t mode, int data, int size){
  SSD1306Wire.beginTransmission(SSD1306_ADDRESS);
  SSD1306Wire.write(mode);
  for (int i = 0; i < size; i++){
    if (i % SSD1306_BUFFER >= SSD1306_BUFFER -1){  //reset connection before we run out of buffer space.
      SSD1306Wire.endTransmission();
      SSD1306Wire.beginTransmission(SSD1306_ADDRESS);
      SSD1306Wire.write(mode);
    }
    SSD1306Wire.write(pgm_read_byte_near(data +i));
  }
  SSD1306Wire.endTransmission();
}

void SSD1306::clear(){
  SSD1306::area(0, SSD1306_PAGES, 0, SSD1306_COLUMNS); //set area to full screen size
  for (int i = 0; i <= SSD1306_SCREEN_BYTES/SSD1306_BUFFER ; i++){ //fill screen with 0x00. nested loops because of buffer size issue
    SSD1306Wire.beginTransmission(SSD1306_ADDRESS);
    SSD1306Wire.write(SSD1306_DATA);
    for (int j = 0; j < SSD1306_BUFFER; j++){
      SSD1306Wire.write(0x00);
    }
    SSD1306Wire.endTransmission();
  }
  SSD1306::area(0, SSD1306_PAGES, 0, SSD1306_COLUMNS); //set area to full screen again, mostly just to jump cursor to 0,0
}

void SSD1306::display_on(){
  uint8_t data[] = {0xAF};
  SSD1306::write(SSD1306_COMMAND, data, sizeof(data));
}

void SSD1306::display_off(){
  uint8_t data[] = {0xAE};
  SSD1306::write(SSD1306_COMMAND, data, sizeof(data));
}

void SSD1306::contrast(uint8_t value){
  uint8_t data[] = {0x81, value};
  SSD1306::write(SSD1306_COMMAND, data, sizeof(data));
}

void SSD1306::area(uint8_t page_start, uint8_t page_end, uint8_t col_start, uint8_t col_end){
  uint8_t location[] = {
    0x20, 0x00,                //horizontal addressing mode
    0x21, col_start, col_end,  //set start/end column
    0x22, page_start, page_end //set start_end page
  };
  SSD1306::write(SSD1306_COMMAND, location, sizeof(location));
}

void SSD1306::fill(uint8_t page_start, uint8_t page_end, uint8_t col_start, uint8_t col_end, uint8_t data){
  SSD1306::area(page_start, page_end, col_start, col_end);

  SSD1306Wire.beginTransmission(SSD1306_ADDRESS);
  SSD1306Wire.write(SSD1306_DATA);
  for (int i = 0; i < ((page_end-page_start)+1) * ((col_end-col_start)+1); i++){
    if (i % SSD1306_BUFFER >= SSD1306_BUFFER -1){  //reset connection before we run out of buffer space.
      SSD1306Wire.endTransmission();
      SSD1306Wire.beginTransmission(SSD1306_ADDRESS);
      SSD1306Wire.write(SSD1306_DATA);
    }
    SSD1306Wire.write(data);
  }
  SSD1306Wire.endTransmission();
}

