/***********************************************************************************

Inspried by PHPPLD.
2021 Rue Mohr.
This generic ROM-generator framework was customized for:

scanless keyboard encoder LUT

INPUTS
 - rows,    8 bits, 0 - 7
 - columns, 8 bits, 12,15,14,13,8,9,11,10 (this is to correct for the horrid 27512 pinout!)
 

OUTPUTS
 - key id               ( 6 bits, 0 - 5 )
 - more than 1 key down ( bit 6 )
 - signle key down      ( bit 7 )
 
FEEDBACK
 N/A



Address bits      8 bit rom size

       -- no parallel roms available --
     8                  2 k
     9                  4 k
     10                 8 k
     
       -- eeproms available from here --
     11                 16 k  (28C16)
     12                 32 k  (28C32)
     
       -- eprom practical sizes from here --
     13                 64 k  (2764)
     14                 128 k (27128)
     15                 256 k 
     16                 512 k
     17                 1 M  (27010)
     18                 2 M
     19                 4 M
     20                 8 M

       -- flash from here up --



**************************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include "ROMLib.h"


// the number of address lines you need !!!???!!!
#define InputBits 16

// the output data size, 8 or 16
#define OutputBits 8

// default output value
#define DFOutput  0x00



// Tuck this one away!. Bit reverser!  Please dont use this in real fft code,
//   YOU KNOW how many bits your working with, and you can use a 
//   specific case generator for it.
uint8_t uniReverse(uint8_t i, uint8_t bits) {

  uint8_t r, m, b;
  r = 0;             // result
  m = 1 << (bits-1); // mask will travel right
  b = 1;             // bit will travel left
  
  while(m) {
    if (i&b) r |=m;
    b <<= 1;
    m >>= 1;  
  }
  
  return r;  

}


// count set bits, unrolled edition.
// if using assember shift into the carry and use addc, 0
uint8_t bitCount(uint8_t n) {  
   uint8_t rv;
   rv = 0;
   if (n & 0x01) rv++;
   if (n & 0x02) rv++;
   if (n & 0x04) rv++;
   if (n & 0x08) rv++;
   if (n & 0x10) rv++;
   if (n & 0x20) rv++;
   if (n & 0x40) rv++;
   if (n & 0x80) rv++;   
   return rv;
}



int main(void) {

  uint16_t colI, rowI;                             // (custom) values are 16 bits      
  uint32_t codeO, keyO, multKeyO ;                 // (custom) bit fields are 32 bits



  uint32_t out;  // leave it alone!

  setup();       // open output file.
  
  
  // loop thru each address
  for( A=0; A<(1<<InputBits); A++) { // A is a bitfield (a mash-up of the vars we want)
       
     // reset vars  
     codeO    = 0;
     keyO     = 0;
     multKeyO = 0;
     // build input values
     spliceValueFromField( &colI,            A,  8,   0,1,2,3,4,5,6,7);        // col, 8 bits
     spliceValueFromField( &rowI,            A,  8,   12,15,14,13,8,9,11,10);  // row B, 8 bits (corrected for 27512 pinout)


     // do task
     // normalize, the data comes in inverted
     colI ^= 0xFF;  
     rowI ^= 0xFF;

     // how many bits are set ?
     if ((bitCount(colI) > 1)  || (bitCount(rowI) > 1) )   multKeyO = 1;              
     if ((bitCount(rowI) == 1) && (bitCount(colI) == 1))   keyO     = 1;
     
     // find the key based on first set bit.
     if (0) {
     } else if (colI & 0x01) { codeO = 0<<3;
     } else if (colI & 0x02) { codeO = 1<<3;
     } else if (colI & 0x04) { codeO = 2<<3;
     } else if (colI & 0x08) { codeO = 3<<3;
     } else if (colI & 0x10) { codeO = 4<<3;
     } else if (colI & 0x20) { codeO = 5<<3;
     } else if (colI & 0x40) { codeO = 6<<3;
     } else if (colI & 0x80) { codeO = 7<<3;
     }
     
     if (0) {
     } else if (rowI & 0x01) { codeO |= 0;
     } else if (rowI & 0x02) { codeO |= 1;
     } else if (rowI & 0x04) { codeO |= 2;
     } else if (rowI & 0x08) { codeO |= 3;
     } else if (rowI & 0x10) { codeO |= 4;
     } else if (rowI & 0x20) { codeO |= 5;
     } else if (rowI & 0x40) { codeO |= 6;
     } else if (rowI & 0x80) { codeO |= 7;
     } 
     
   
     // reconstitute the output
     // assign default values for outputs     
     out = DFOutput;
     spliceFieldFromValue( &out, codeO,     6,  0,1,2,3,4,5); // add 6 bits from codeO, that go in bit positions 0,1,2,3,4,5
     spliceFieldFromValue( &out, multKeyO,  1,  6);           // add to output, multiKeyO, which is 1 bit, placed at bit 6 of output
     spliceFieldFromValue( &out, keyO,      1,  7);           // add to output, keyO, which is 1 bit, placed at bit 7 of the output
          
     // submit entry to file
     write(fd, &out, OutputBits>>3);  // >>3 converts to bytes, leave it!
  }
  
  cleanup(); // close file
//  printf("}\n"); // have this code generate a table in C too!
  return 0;
}








