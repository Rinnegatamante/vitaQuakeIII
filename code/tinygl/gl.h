/*
 * The following constants come from Mesa
 */
#ifndef PGL_H
#define PGL_H

#ifdef __cplusplus
extern "C" {
#endif

/* functions */

void pglEnable(GLenum code);

void pglDisable(GLenum code);

void pglShadeModel(GLenum mode);

void pglCullFace(GLenum mode);

void pglPolygonMode(GLenum face, GLenum mode);

void pglBegin(GLenum type);

void pglEnd(void);

#define PROTO_GL1(name)                \
void pgl ## name ## 1f(float);    \
void pgl ## name ## 1d(double);    \
void pgl ## name ## 1fv(float *);        \
void pgl ## name ## 1dv(double *);

#define PROTO_GL2(name)                \
void pgl ## name ## 2f(float ,float);    \
void pgl ## name ## 2d(double ,double);    \
void pgl ## name ## 2fv(float *);        \
void pgl ## name ## 2dv(double *);

#define PROTO_GL3(name)                \
void pgl ## name ## 3f(float ,float ,float);    \
void pgl ## name ## 3d(double ,double ,double);    \
void pgl ## name ## 3fv(float *);        \
void pgl ## name ## 3dv(double *);

#define PROTO_GL4(name)                \
void pgl ## name ## 4f(float ,float ,float, float );    \
void pgl ## name ## 4d(double ,double ,double, double );    \
void pgl ## name ## 4fv(float *);        \
void pgl ## name ## 4dv(double *);

PROTO_GL2(Vertex)

PROTO_GL3(Vertex)

PROTO_GL4(Vertex)

PROTO_GL3(Color)

PROTO_GL4(Color)

PROTO_GL3(Normal)

PROTO_GL1(TexCoord)

PROTO_GL2(TexCoord)

PROTO_GL3(TexCoord)

PROTO_GL4(TexCoord)

void pglEdgeFlag(GLboolean flag);

/* matrix */
void pglMatrixMode(GLenum mode);

void pglLoadMatrixf(const float *m);

void pglLoadIdentity(void);

void pglMultMatrixf(const float *m);

void pglPushMatrix(void);

void pglPopMatrix(void);

void pglRotatef(float angle, float x, float y, float z);

void pglTranslatef(float x, float y, float z);

void pglScalef(float x, float y, float z);

void pglViewport(int x, int y, GLsizei width, GLsizei height);

void pglFrustum(double left, double right, double bottom, double top,
               double nearz, double far);

/* lists */
unsigned int pglGenLists(GLsizei range);

GLboolean pglIsList(unsigned int list);

void ppglNewList(unsigned int list, GLenum mode);

void pglEndList(void);

void pglCallList(unsigned int list);

/* clear */
void pglClear(GLbitfield mask);

void pglClearColor(float r, float g, float b, float a);

void pglClearDepth(double depth);

/* selection */
int pglRenderMode(GLenum mode);

void pglSelectBuffer(GLsizei size, unsigned int *buf);

void pglInitNames(void);

void pglPushName(unsigned int name);

void pglPopName(void);

void pglLoadName(unsigned int name);

/* textures */
void pglGenTextures(GLsizei n, unsigned int *textures);

void pglDeleteTextures(GLsizei n, const unsigned int *textures);

void pglBindTexture(GLenum target, GLuint texture);

void pglTexImage2D(GLenum target, int level, int components,
                  GLsizei width, GLsizei height, int border,
                  GLenum format, GLenum type, const void *pixels);

void pglTexEnvi(GLenum target, GLenum pname, int param);

void pglTexParameteri(GLenum target, GLenum pname, int param);

void pglPixelStorei(GLenum pname, int param);

/* lighting */

void pglMaterialfv(GLenum mode, GLenum type, const float *v);

void pglMaterialf(GLenum mode, GLenum type, float v);

void pglColorMaterial(GLenum mode, GLenum type);

void pglLightfv(GLenum light, GLenum type, const float *v);

void pglLightf(GLenum light, GLenum type, float v);

void pglLightModeli(GLenum pname, int param);

void pglLightModelfv(GLenum pname, float *param);

/* misc */

void pglFlush(void);

void pglHint(int target, int mode);

void pglGetIntegerv(GLenum pname, GLboolean *params);

void pglGetFloatv(GLenum pname, float *v);

void pglFrontFace(GLenum mode);

/* opengl 1.2 arrays */
void pglEnableClientState(GLenum array);

void pglDisableClientState(GLenum array);

void pglArrayElement(GLint i);

void pglVertexPointer(GLint size, GLenum type, GLsizei stride,
                     const GLvoid *pointer);

void pglColorPointer(GLint size, GLenum type, GLsizei stride,
                    const GLvoid *pointer);

void pglNormalPointer(GLenum type, GLsizei stride,
                     const GLvoid *pointer);

void pglTexCoordPointer(GLint size, GLenum type, GLsizei stride,
                       const GLvoid *pointer);

/* opengl 1.2 polygon offset */
void pglPolygonOffset(GLfloat factor, GLfloat units);

/* not implemented, just added to compile  */
/*
inline void glPointSize(float) {}
inline void glLineWidth(float) {}
inline void glDeleteLists(int, int) {}
inline void glDepthFunc(int) {}
inline void glBlendFunc(int, int) {}
inline void glTexEnvf(int, int, int) {}
inline void glOrtho(float,float,float,float,float,float){}
inline void glVertex2i(int,int) {}
inline void glDepthMask(int) {}
inline void glFogi(int, int) {}
inline void glFogfv(int, const float*) {}
inline void glFogf(int, float) {}
inline void glRasterPos2f(float, float) {}
inline void glPolygonStipple(void*) {}
inline void glTexParameterf(int, int, int) {};
*/
/* non compatible functions */

void pglDebug(int mode);

void pgluLookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx,
               GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy,
               GLdouble upz);

void pgluPerspective(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar);

void pglOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
              GLfloat nearz, GLfloat far);

#define pglOrtho pglOrthof

// TODO:
#define pglDepthMask(a)
#define pglBlendFunc(a, b)

#ifdef __cplusplus
}
#endif

#endif
