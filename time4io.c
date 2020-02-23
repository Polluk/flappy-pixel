#include <stdint.h>
#include <pic32mx.h>
#include "mipslab.h"

int getbtns( void ){
  return (((PORTD & 0xe0) >> 4) | ((PORTF & 0x2) >> 1));  //returnerar läget på
}                                                        // knapparna
