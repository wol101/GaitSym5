
/*
 * port.h
 *
 * FUNCTION:
 * This file contains defines for porting the tubing toolkit
 * from GL to OpenGL; it also allows one to define an interface
 * to get at the low-level GL output of this package via callbacks.
 *
 * HISTORY:
 * Created by Linas Vepstas --  February 1993
 * Added auto texture coord generation hooks, Linas April 1994
 */

#ifndef __GLE_PORT_H__
#define __GLE_PORT_H__

#include <stdio.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ====================================================== */
/* needed for Windows NT */

#ifdef WIN32
#include <windows.h>
#endif

/* ====================================================== */
/* Some compilers can't handle multiply-subscripted array's */

#if FUNKY_C
typedef gleDouble gleVector;
#define AVAL(arr,n,i,j)  arr(6*n+3*i+j)
#define VVAL(arr,n,i)  arr(3*n+i)

#else  /* FUNKY_C */
typedef double gleVector[3];
#define AVAL(arr,n,i,j)  arr[n][i][j]
#define VVAL(arr,n,i)  arr[n][i];

#endif /* FUNKY_C */

/* ====================================================== */
/* These are used to convey info about topography to the
 * texture mapping routines */

#define FRONT       1
#define BACK        2
#define FRONT_CAP   3
#define BACK_CAP    4
#define FILLET      5

/* ====================================================== */

#ifdef __GLE_DOUBLE
#define MULTMATRIX(m)       MULTMATRIX_D(m)
#define LOADMATRIX(m)       LOADMATRIX_D(m)
#define V3F(x,j,id)     V3F_D(x,j,id)
#define N3F(x)          N3F_D(x)
#define T2F(x,y)        T2F_D(x,y)
#else
#define MULTMATRIX(m)       MULTMATRIX_F(m)
#define LOADMATRIX(m)       LOADMATRIX_F(m)
#define V3F(x,j,id)     V3F_F(x,j,id)
#define N3F(x)          N3F_F(x)
#define T2F(x,y)        T2F_F(x,y)
#endif

/* ====================================================== */

#if DEBUG_OUTPUT
#undef GL_32
#undef OPENGL_10

#define BGNTMESH(i,len) printf ("bgntmesh() \n");
#define ENDTMESH()      printf ("endtmesh() \n");
#define BGNPOLYGON()    printf ("bgnpolygon() \n");
#define ENDPOLYGON()    printf ("endpolygon() \n");
#define V3F_F(x,j,id)   printf ("v3f(x)	%f %f %f \n", x[0], x[1], x[2]);
#define V3F_D(x,j,id)   printf ("v3d(x) %f %f %f \n", x[0], x[1], x[2]);
#define N3F_F(x)    printf ("n3f(x) %f %f %f \n", x[0], x[1], x[2]);
#define N3F_D(x)    printf ("n3d(x)	%f %f %f \n", x[0], x[1], x[2]);
#define C3F(x)      printf ("c3f(x) %f %f %f \n", x[0], x[1], x[2]);
#define C4F(x)      printf ("c4f(x) %f %f %f %f \n", x[0], x[1], x[2], x[3]);
#define T2F_F(x, y) printf ("t2f(x) %f %f \n", x, y);
#define T2F_D(x, y) printf ("t2d(x)	%f %f \n", x, y);

#define POPMATRIX()     printf ("popmatrix () \n");
#define PUSHMATRIX()    printf ("pushmatrix() \n");
#define MULTMATRIX_F(x) MULTMATRIX_D(x)
#define LOADMATRIX_F(x) LOADMATRIX_D(x)


#define LOADMATRIX_D(x) {               \
    int i, j;                        \
        printf ("loadmatrix (x) \n");            \
        for (i=0; i<4; i++) {                \
            for (j=0; j<4; j++) {             \
                printf ( "%f ", (x)[i][j]);            \
        }                         \
            printf (" \n");                   \
    }                            \
}

#define MULTMATRIX_D(x) {               \
    int i, j;                        \
        printf ("multmatrix (x) \n");            \
        for (i=0; i<4; i++) {                \
            for (j=0; j<4; j++) {             \
                printf ( "%f ", (x)[i][j]);            \
        }                         \
            printf (" \n");                   \
    }                            \
}


#define __IS_LIGHTING_ON  (1)

#endif /* DEBUG_OUTPUT */

/* ====================================================== */

#if GL_32

#include <gl/gl.h>

#define BGNTMESH(i,len) bgntmesh()
#define ENDTMESH()  endtmesh()
#define BGNPOLYGON()    bgnpolygon()
#define ENDPOLYGON()    endpolygon()
#define V3F_F(x,j,id)   v3f(x)
#define V3F_D(x,j,id)   v3d(x)
#define N3F_F(x)    n3f(x)
#define T2F_F(x,y)
#define T2F_D(x,y)
#define C3F(x)      c3f(x)

#define POPMATRIX() popmatrix ()
#define PUSHMATRIX()    pushmatrix()
#define MULTMATRIX_F(x) multmatrix (x)
#define LOADMATRIX_F(x) loadmatrix (x)

#define N3F_D(x) {                  \
   float nnn[3];                    \
   nnn[0] = (float) (x)[0];                 \
   nnn[1] = (float) (x)[1];                 \
   nnn[2] = (float) (x)[2];                 \
   n3f (nnn);                       \
}

#define LOADMATRIX_D(x) {               \
   int i, j;                        \
   float mmm[4][4];                 \
   for (i=0; i<4; i++) {                \
      for (j=0; j<4; j++) {             \
         mmm[i][j] = (float) (x)[i][j];         \
      }                         \
   }                            \
   loadmatrix(mmm);                 \
}

#define MULTMATRIX_D(x) {               \
   int i, j;                        \
   float mmm[4][4];                 \
   for (i=0; i<4; i++) {                \
      for (j=0; j<4; j++) {             \
         mmm[i][j] = (float) (x)[i][j];         \
      }                         \
   }                            \
   multmatrix(mmm);                 \
}

/* #define __IS_LIGHTING_ON  (MSINGLE == getmmode()) */
#define __IS_LIGHTING_ON  (extrusion_join_style & TUBE_LIGHTING_ON)

#endif /* GL_32 */

/* ====================================================== */
#if OPENGL_10

#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE_CC__ /* Darwin */
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#if FLIP_NORMAL
#define N3F_F(x) {                  \
   float nnn[3];                    \
   nnn[0] = - (float) (x)[0];               \
   nnn[1] = - (float) (x)[1];               \
   nnn[2] = - (float) (x)[2];               \
   glNormal3fv (nnn);                   \
}
#define N3F_D(x) {                  \
   float nnn[3];                    \
   nnn[0] = - (float) (x)[0];               \
   nnn[1] = - (float) (x)[1];               \
   nnn[2] = - (float) (x)[2];               \
   glNormal3fv (nnn);                   \
}
#endif /* FLIP_NORMAL */

#define C3F(x)      glColor3fv(x)
#define T2F_F(x,y)  glTexCoord2f(x,y)
#define T2F_D(x,y)  glTexCoord2d(x,y)

#define POPMATRIX() glPopMatrix()
#define PUSHMATRIX()    glPushMatrix()

#define MULTMATRIX_F(x) glMultMatrixf ((const GLfloat *)x)
#define LOADMATRIX_F(x) glLoadMatrixf ((const GLfloat *)x)

#define MULTMATRIX_D(x) glMultMatrixd ((const GLdouble *)x)
#define LOADMATRIX_D(x) glLoadMatrixd ((const GLdouble *)x)

#define __IS_LIGHTING_ON  (glIsEnabled(GL_LIGHTING))

/* ====================================================== */
#ifdef AUTO_TEXTURE

#define BGNTMESH(i,len) {                   \
    if(_gle_gc -> bgn_gen_texture) (*(_gle_gc -> bgn_gen_texture))(i,len);\
    glBegin (GL_TRIANGLE_STRIP);            \
}

#define BGNPOLYGON() {                  \
    if(_gle_gc -> bgn_gen_texture) (*(_gle_gc -> bgn_gen_texture))();\
    glBegin (GL_POLYGON);               \
}

#define N3F_F(x) {                  \
    if(_gle_gc -> n3f_gen_texture) (*(_gle_gc -> n3f_gen_texture))(x); \
    glNormal3fv(x);                 \
}

#define N3F_D(x) {                  \
    if(_gle_gc -> n3d_gen_texture) (*(_gle_gc -> n3d_gen_texture))(x); \
    glNormal3dv(x);                 \
}

#define V3F_F(x,j,id) {                     \
    if(_gle_gc -> v3f_gen_texture) (*(_gle_gc -> v3f_gen_texture))(x,j,id);\
    glVertex3fv(x);                 \
}

#define V3F_D(x,j,id) {                     \
    if(_gle_gc -> v3d_gen_texture) (*(_gle_gc -> v3d_gen_texture))(x,j,id); \
    glVertex3dv(x);                 \
}

#define ENDTMESH() {                    \
    if(_gle_gc -> end_gen_texture) (*(_gle_gc -> end_gen_texture))(); \
    glEnd ();                   \
}

#define ENDPOLYGON() {                  \
    if(_gle_gc -> end_gen_texture) (*(_gle_gc -> end_gen_texture))(); \
    glEnd ();                   \
}

/* ====================================================== */
#else /* AUTO_TEXTURE */

#define BGNTMESH(i,len) glBegin (GL_TRIANGLE_STRIP);
#define BGNPOLYGON()    glBegin (GL_POLYGON);

#define N3F_F(x)    glNormal3fv(x)
#define N3F_D(x)    glNormal3dv(x)
#define V3F_F(x,j,id)   glVertex3fv(x);
#define V3F_D(x,j,id)   glVertex3dv(x);

#define ENDTMESH()  glEnd ()
#define ENDPOLYGON()    glEnd()

#endif /* AUTO_TEXTURE */

#endif /* OPENGL_10 */

#ifdef TRIANGLE_GENERATOR

#include "GLEmulator.h"
extern GLEmulator glEmulator;

void C3F(float *x);

// #define T2F_F(x,y)      glEmulator.texCoord2f(x,y)
#define T2F_D(x,y)      glEmulator.glTexCoord2d(x,y)

#define POPMATRIX()     glEmulator.glPopMatrix()
#define PUSHMATRIX()    glEmulator.glPushMatrix()

// #define MULTMATRIX_F(x) glEmulator.glMultMatrixf(reinterpret_cast<const float *>(x))
// #define LOADMATRIX_F(x) glEmulator.glLoadMatrixf(reinterpret_cast<const float *>(x))

#define MULTMATRIX_D(x) glEmulator.glMultMatrixd(reinterpret_cast<const double *>(x))
#define LOADMATRIX_D(x) glEmulator.glLoadMatrixd(reinterpret_cast<const double *>(x))

#define __IS_LIGHTING_ON (true)

#define BGNTMESH(i,len) glEmulator.glBeginTriangleStrip()
#define BGNPOLYGON()    glEmulator.glBeginPolygon()

// #define N3F_F(x)        glEmulator.glNormal3fv(x)
#define N3F_D(x)        glEmulator.glNormal3dv(x)
// #define V3F_F(x,j,id)   glEmulator.glVertex3fv(x)
#define V3F_D(x,j,id)   glEmulator.glVertex3dv(x)

#define ENDTMESH()      glEmulator.glEnd()
#define ENDPOLYGON()    glEmulator.glEnd()


#endif

#endif /* __GLE_PORT_H__ */
/* ================== END OF FILE ======================= */
