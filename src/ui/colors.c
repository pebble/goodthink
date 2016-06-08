#include <pebble.h>

GColor getColorFromAngle(uint8_t angle){
  if(angle < 10)
    return GColorFromHEX(0xFF0000);
  else if(angle < 25)
    return GColorFromHEX(0xFF5500);
  else if(angle < 40)
    return GColorFromHEX(0xFFAA00);
  else if(angle < 55)
    return GColorFromHEX(0xFFFF00);
  else if(angle < 70)
    return GColorFromHEX(0xaaff00);
  else if(angle < 90)
    return GColorFromHEX(0x55ff00);
  else if(angle < 110)
    return GColorFromHEX(0x00ff55);
  else if(angle < 120)
    return GColorFromHEX(0x00ffaa);
  else if(angle < 130)
    return GColorFromHEX(0x00ffff);
  else if(angle < 140)
    return GColorFromHEX(0x00aaff);
  else if(angle < 145)
    return GColorFromHEX(0x0055ff);
  else if(angle < 155)
    return GColorFromHEX(0x0000ff);
  else if(angle < 163)
    return GColorFromHEX(0x5500ff);
  else if(angle < 172)
    return GColorFromHEX(0xaa00ff);
  else 
    return GColorFromHEX(0xff00ff);
}
