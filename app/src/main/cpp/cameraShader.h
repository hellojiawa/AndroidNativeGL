#ifndef _CM_CAMAERA_SHADER_
#define _CM_CAMAERA_SHADER_

#define DEBUG 0

#include <GLES2/gl2.h>
#include "matrix.h"

void drawFrame(void *ins, const float pDouble[12], const float pDouble1[8]);

void bindTexture(GLenum texture_n, unsigned int texture_id, GLsizei width, GLsizei height, const void * buffer);

#endif
