#include <stdio.h>
#include <stdint.h>

#define TRACKS  40
#define SECTORS 16
#define SECTORSIZE 345

uint8_t floppy[TRACKS][SECTORS][SECTORSIZE];

int main() {
    for (size_t t = 0; t < TRACKS; t++) {
        for (size_t s = 0; s < SECTORS; s++) {
            
        }
    }
}