//
// Created by admin on 2020/5/18.
//

#ifndef _H_GL_UTILS_
#define _H_GL_UTILS_

#include <GLES2/gl2.h>

typedef struct Instance {
    //顶点着色器位置数据引用
    unsigned int maPositionHandle;
    //顶点着色器纹理坐标引用
    unsigned int maTexCoorHandle;
    //着色器程序引用
    unsigned int pProgram;
    //顶点着色器最终变化矩阵引用
    unsigned int maMVPMatrixHandle;
    //片元着色器采样器引用
    unsigned int myTextureHandle;
    unsigned int muTextureHandle;
    unsigned int mvTextureHandle;
    //纹理数据
    unsigned int yTexture;
    unsigned int uTexture;
    unsigned int vTexture;
    //着色器渲染宽高
    unsigned int pWidth;
    unsigned int pHeight;
    //屏幕的宽高
    unsigned int vWidth;
    unsigned int vHeight;
    //yuv数据
    unsigned long yBufferSize;
    signed char *yBuffer;
    signed char *uBuffer;
    unsigned long uBufferSize;
    signed char *vBuffer;
    unsigned long vBufferSize;
    unsigned long yuvBufferSize;
} Instance;


GLuint load_shader(GLenum shaderType, const char *pSource);

GLuint create_program(const char *pVertexSource, const char *pFragmentSource);

void delete_program(GLuint &program);

void check_gl_error(const char *op);

#endif

