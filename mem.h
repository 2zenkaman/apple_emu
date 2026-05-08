#ifndef APPLE_II_MEM_H
#define APPLE_II_MEM_H

#include <stdint.h>

#define KB * 1024

#define MEMSIZ 64 KB
#define RAMSIZ 48 KB
#define PERSIZ 4  KB
#define ROMSIZ 12 KB
#define TXTSIZ 2  KB

uint8_t* at(uint16_t i, uint8_t waccess);

#endif