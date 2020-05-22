#include <jni.h>
#include <string>
#include <GLES2/gl2.h>
#include <android/log.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include "GLUtils.h"

#include "matrix.h"
#include "cameraShader.h"

#define  LOG_TAG    "liuping"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define UNIT 1
#define TEXTURE_COOR_UNIT 1

//顶点着色器脚本代码
const char *codeVertexShader =
        "attribute vec3 aPosition;              \n"
        "uniform mat4 uMVPMatrix;               \n"
        "attribute vec2 aTexCoor;               \n"
        "varying vec2 vTexCoor;                 \n"
        "void main()                            \n"
        "{                                      \n"
        "	gl_Position = uMVPMatrix * vec4(aPosition, 1); 	\n"
        " 	vTexCoor = aTexCoor;							\n"
        "} 													\n";


const char *codeFragShader =
        "precision mediump float;							\n"
        "uniform sampler2D yTexture; 						\n"
        "uniform sampler2D uTexture; 						\n"
        "uniform sampler2D vTexture; 						\n"
        "varying vec2 vTexCoor;								\n"
        "void main()										\n"
        "{													\n"
        "	float y = texture2D(yTexture, vTexCoor).r;		\n"
        "	float u = texture2D(uTexture, vTexCoor).r;		\n"
        "	float v = texture2D(vTexture, vTexCoor).r;	   \n"
        "	vec3 yuv = vec3(y, u, v);						\n"
        "	vec3 offset = vec3(16.0 / 255.0, 128.0 / 255.0, 128.0 / 255.0);\n"
        "	mat3 mtr = mat3(1.0, 1.0, 1.0, -0.001, -0.3441, 1.772, 1.402, -0.7141, 0.001);\n"
        "	vec4 curColor = vec4(mtr * (yuv - offset), 1);	\n"
        "	gl_FragColor = curColor;						\n"
        "}													\n";


//渲染顶点坐标数据
const float dataVertex[] =
        {
                -1 * UNIT, 1 * UNIT, 0,
                -1 * UNIT, -1 * UNIT, 0,
                1 * UNIT, 1 * UNIT, 0,
                1 * UNIT, -1 * UNIT, 0
        };
//渲染纹理坐标数据
const float dataTexCoor[] =
        {
                0 * TEXTURE_COOR_UNIT, 0 * TEXTURE_COOR_UNIT,
                0 * TEXTURE_COOR_UNIT, 1 * TEXTURE_COOR_UNIT,
                1 * TEXTURE_COOR_UNIT, 0 * TEXTURE_COOR_UNIT,
                1 * TEXTURE_COOR_UNIT, 1 * TEXTURE_COOR_UNIT
        };

//全局变量
Instance *instance;


extern "C" JNIEXPORT void JNICALL
Java_com_lerp_nativegl_RendererWrapper_onSurfaceCreated(
        JNIEnv *env,
        jobject o/* this */, jint pWidth, jint pHeight) {
    instance = (Instance *) malloc(sizeof(Instance));
    memset(instance, 0, sizeof(Instance));
    //	1.初始化着色器
    instance->pProgram = create_program(codeVertexShader, codeFragShader);
    instance->maMVPMatrixHandle = glGetUniformLocation(instance->pProgram, "uMVPMatrix");
    instance->maPositionHandle = glGetAttribLocation(instance->pProgram, "aPosition");
    instance->maTexCoorHandle = glGetAttribLocation(instance->pProgram, "aTexCoor");
    instance->myTextureHandle = glGetUniformLocation(instance->pProgram, "yTexture");
    instance->muTextureHandle = glGetUniformLocation(instance->pProgram, "uTexture");
    instance->mvTextureHandle = glGetUniformLocation(instance->pProgram, "vTexture");
    //	2.初始化纹理
    glGenTextures(1, &instance->yTexture);
    glGenTextures(1, &instance->uTexture);
    glGenTextures(1, &instance->vTexture);

    //	3.分配Yuv数据内存
    instance->yBufferSize = sizeof(char) * pWidth * pHeight;
    instance->uBufferSize = sizeof(char) * pWidth / 2 * pHeight / 2;
    instance->vBufferSize = sizeof(char) * pWidth / 2 * pHeight / 2;
    instance->yBuffer = static_cast<signed char *>(malloc(instance->yBufferSize));
    instance->uBuffer = static_cast<signed char *>(malloc(instance->uBufferSize));
    instance->vBuffer = static_cast<signed char *>(malloc(instance->vBufferSize));
    memset(instance->yBuffer, 0, instance->yBufferSize);
    memset(instance->uBuffer, 0, instance->uBufferSize);
    memset(instance->vBuffer, 0, instance->vBufferSize);
    instance->pHeight = pHeight;
    instance->pWidth = pWidth;
    instance->yuvBufferSize = pWidth * pHeight * 3 / 2;
    //清理背景
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

extern "C" JNIEXPORT void JNICALL
Java_com_lerp_nativegl_RendererWrapper_onSurfaceChanged(
        JNIEnv *env,
        jobject o,
        jint width,
        jint height/* this */) {
    if (instance != 0) {
        instance->vWidth = width;
        instance->vHeight = height;
    }
    unsigned int eW, eH;
    float vRadio, radio;
    vRadio = (float) height / width;
    radio = ((float) instance->pWidth / instance->pHeight);
    if (vRadio < radio) {
        eH = instance->vHeight;
        eW = (unsigned int) (eH / radio);
    } else {
        eW = instance->vWidth;
        eH = (unsigned int) (eW * radio);
    }

    glViewport(0, 0, eW, eH);
}

extern "C" JNIEXPORT void JNICALL
Java_com_lerp_nativegl_RendererWrapper_onDrawFrame(
        JNIEnv *env,
        jobject o/* this */, jbyteArray yuvDatas) {
    jbyte *srcp = env->GetByteArrayElements(yuvDatas, 0);
    memcpy(instance->yBuffer, srcp, instance->yBufferSize);
    long i, j;
    j = 0;
    //相机出来的数据是NV21，YUV420p需要修改次数的代码
    for (i = instance->yBufferSize; i < instance->yuvBufferSize; i += 2) {
        instance->vBuffer[j] = srcp[i];
        instance->uBuffer[j] = srcp[i + 1];
        ++j;
    }

    env->ReleaseByteArrayElements(yuvDatas, srcp, JNI_ABORT);
    drawFrame(instance, dataVertex, dataTexCoor);
}

extern "C" JNIEXPORT void JNICALL
Java_com_lerp_nativegl_RendererWrapper_release(JNIEnv *env, jobject obj) {
    if (instance != 0) {
        free(instance->yBuffer);
        free(instance->uBuffer);
        free(instance->vBuffer);
        instance->yBuffer = 0;
        free(instance);
        instance = 0;
    }
}