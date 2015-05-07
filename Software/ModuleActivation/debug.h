// Affichage en mode debug ?
//#define DEBUG_ON

#ifndef __debuge_h_
#define __debuge_h_

/*static uint8_t * heapptr, * stackptr;
static void check_mem() {
  stackptr = (uint8_t *)malloc(4);          // use stackptr temporarily
  heapptr = stackptr;                     // save value of heap pointer
  free(stackptr);      // free up the memory again (sets stackptr to 0)
  stackptr =  (uint8_t *)(SP);           // save value of stack pointer
}*/

#ifdef DEBUG_ON
#define debugPrint(s)  Serial.print(s)
#define debugPrintln(s)  Serial.println(s)
#define debugPrintHex(s)  Serial.print(s,HEX)
#else
#define debugPrint(s)
#define debugPrintln(s)
#define debugPrintHex(s)
#define printf(f,...)
#endif



#endif

