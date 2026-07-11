#ifndef __AMEE_GLCHECK_H__
#define __AMEE_GLCHECK_H__
#pragma once
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <cstdio>



#define GL_CHECK(Call) \
    do { \
        while (glGetError() != GL_NO_ERROR) {} \
        Call; \
        GLenum GLErr = glGetError(); \
        if (GLErr != GL_NO_ERROR) { \
            fprintf(stderr, "[OpenGL] %s:%d error 0x%04x in '%s'\n", __FILE__, __LINE__, GLErr, #Call); \
        } \
    } while(0)

#endif // __AMEE_GLCHECK_H__
