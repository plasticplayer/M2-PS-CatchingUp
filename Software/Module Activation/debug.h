// Affichage en mode debug ?
#define DEBUG_ON

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
#define debugPrint  Serial.print
#define debugPrintln  Serial.println
#else
#define debugPrint
#define debugPrintln
#endif



#endif

