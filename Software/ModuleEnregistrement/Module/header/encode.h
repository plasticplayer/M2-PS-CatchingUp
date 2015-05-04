#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bcm_host.h"
#include "ilclient.h"

void writeImageH264(char *image, int32_t filledLen,FILE * fileDesc);
void deinitH264();
bool initH264(int _height,int _width,unsigned char _frameRate);



#ifdef __cplusplus
}
#endif
