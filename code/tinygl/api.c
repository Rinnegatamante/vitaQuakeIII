#include "zgl.h"
#include <string.h>
#include <stdio.h>

/* glVertex */

void pglVertex4f(float x, float y, float z, float w) {
    GLParam p[5];

    p[0].op = OP_Vertex;
    p[1].f = x;
    p[2].f = y;
    p[3].f = z;
    p[4].f = w;

    gl_add_op(p);
}

void pglVertex2f(float x, float y) {
    pglVertex4f(x, y, 0, 1);
}

void pglVertex3f(float x, float y, float z) {
    pglVertex4f(x, y, z, 1);
}

void pglVertex3fv(float *v) {
    pglVertex4f(v[0], v[1], v[2], 1);
}

/* glNormal */

void pglNormal3f(float x, float y, float z) {
    GLParam p[4];

    p[0].op = OP_Normal;
    p[1].f = x;
    p[2].f = y;
    p[3].f = z;

    gl_add_op(p);
}

void pglNormal3fv(float *v) {
    pglNormal3f(v[0], v[1], v[2]);
}

/* glColor */

void pglColor4f(float r, float g, float b, float a) {
    GLParam p[8];

    p[0].op = OP_Color;
    p[1].f = r;
    p[2].f = g;
    p[3].f = b;
    p[4].f = a;
    /* direct convertion to integer to go faster if no shading */
    p[5].ui = (unsigned int) (r * (ZB_POINT_RED_MAX - ZB_POINT_RED_MIN) +
                              ZB_POINT_RED_MIN);
    p[6].ui = (unsigned int) (g * (ZB_POINT_GREEN_MAX - ZB_POINT_GREEN_MIN) +
                              ZB_POINT_GREEN_MIN);
    p[7].ui = (unsigned int) (b * (ZB_POINT_BLUE_MAX - ZB_POINT_BLUE_MIN) +
                              ZB_POINT_BLUE_MIN);
    gl_add_op(p);
}

void pglColor4fv(float *v) {
    GLParam p[8];

    p[0].op = OP_Color;
    p[1].f = v[0];
    p[2].f = v[1];
    p[3].f = v[2];
    p[4].f = v[3];
    /* direct convertion to integer to go faster if no shading */
    p[5].ui = (unsigned int) (v[0] * (ZB_POINT_RED_MAX - ZB_POINT_RED_MIN) +
                              ZB_POINT_RED_MIN);
    p[6].ui = (unsigned int) (v[1] * (ZB_POINT_GREEN_MAX - ZB_POINT_GREEN_MIN) +
                              ZB_POINT_GREEN_MIN);
    p[7].ui = (unsigned int) (v[2] * (ZB_POINT_BLUE_MAX - ZB_POINT_BLUE_MIN) +
                              ZB_POINT_BLUE_MIN);
    gl_add_op(p);
}

void pglColor3f(float x, float y, float z) {
    pglColor4f(x, y, z, 1);
}

void pglColor3fv(float *v) {
    pglColor4f(v[0], v[1], v[2], 1);
}


/* TexCoord */

void pglTexCoord4f(float s, float t, float r, float q) {
    GLParam p[5];

    p[0].op = OP_TexCoord;
    p[1].f = s;
    p[2].f = t;
    p[3].f = r;
    p[4].f = q;

    gl_add_op(p);
}

void pglTexCoord2f(float s, float t) {
    pglTexCoord4f(s, t, 0, 1);
}

void pglTexCoord2fv(float *v) {
    pglTexCoord4f(v[0], v[1], 0, 1);
}

void pglEdgeFlag(GLboolean flag) {
    GLParam p[2];

    p[0].op = OP_EdgeFlag;
    p[1].i = flag;

    gl_add_op(p);
}

/* misc */

void pglShadeModel(GLenum mode) {
    GLParam p[2];

    assert(mode == GL_FLAT || mode == GL_SMOOTH);

    p[0].op = OP_ShadeModel;
    p[1].i = mode;

    gl_add_op(p);
}

void pglCullFace(GLenum mode) {
    GLParam p[2];

    assert(mode == GL_BACK ||
           mode == GL_FRONT ||
           mode == GL_FRONT_AND_BACK);

    p[0].op = OP_CullFace;
    p[1].i = mode;

    gl_add_op(p);
}

void pglFrontFace(GLenum mode) {
    GLParam p[2];

    assert(mode == GL_CCW || mode == GL_CW);

    mode = (mode != GL_CCW);

    p[0].op = OP_FrontFace;
    p[1].i = mode;

    gl_add_op(p);
}

void pglPolygonMode(GLenum face, GLenum mode) {
    GLParam p[3];

    assert(face == GL_BACK ||
           face == GL_FRONT ||
           face == GL_FRONT_AND_BACK);
    assert(mode == GL_POINT || mode == GL_LINE || mode == GL_FILL);

    p[0].op = OP_PolygonMode;
    p[1].i = face;
    p[2].i = mode;

    gl_add_op(p);
}


/* glEnable / glDisable */

void pglEnable(GLenum cap) {
    GLParam p[3];

    p[0].op = OP_EnableDisable;
    p[1].i = cap;
    p[2].i = 1;

    gl_add_op(p);
}

void pglDisable(GLenum cap) {
    GLParam p[3];

    p[0].op = OP_EnableDisable;
    p[1].i = cap;
    p[2].i = 0;

    gl_add_op(p);
}

/* glBegin / glEnd */

void pglBegin(GLenum mode) {
    GLParam p[2];

    p[0].op = OP_Begin;
    p[1].i = mode;

    gl_add_op(p);
}

void pglEnd(void) {
    GLParam p[1];

    p[0].op = OP_End;

    gl_add_op(p);
}

/* matrix */

void pglMatrixMode(GLenum mode) {
    GLParam p[2];

    p[0].op = OP_MatrixMode;
    p[1].i = mode;

    gl_add_op(p);
}

void pglLoadMatrixf(const float *m) {
    GLParam p[17];
    int i;

    p[0].op = OP_LoadMatrix;
    for (i = 0; i < 16; i++) p[i + 1].f = m[i];

    gl_add_op(p);
}

void pglLoadIdentity(void) {
    GLParam p[1];

    p[0].op = OP_LoadIdentity;

    gl_add_op(p);
}

void pglMultMatrixf(const float *m) {
    GLParam p[17];
    int i;

    p[0].op = OP_MultMatrix;
    for (i = 0; i < 16; i++) p[i + 1].f = m[i];

    gl_add_op(p);
}

void pglPushMatrix(void) {
    GLParam p[1];

    p[0].op = OP_PushMatrix;

    gl_add_op(p);
}

void pglPopMatrix(void) {
    GLParam p[1];

    p[0].op = OP_PopMatrix;

    gl_add_op(p);
}

void pglRotatef(float angle, float x, float y, float z) {
    GLParam p[5];

    p[0].op = OP_Rotate;
    p[1].f = angle;
    p[2].f = x;
    p[3].f = y;
    p[4].f = z;

    gl_add_op(p);
}

void pglTranslatef(float x, float y, float z) {
    GLParam p[4];

    p[0].op = OP_Translate;
    p[1].f = x;
    p[2].f = y;
    p[3].f = z;

    gl_add_op(p);
}

void pglScalef(float x, float y, float z) {
    GLParam p[4];

    p[0].op = OP_Scale;
    p[1].f = x;
    p[2].f = y;
    p[3].f = z;

    gl_add_op(p);
}


void pglViewport(int x, int y, int width, int height) {
    GLParam p[5];

    p[0].op = OP_Viewport;
    p[1].i = x;
    p[2].i = y;
    p[3].i = width;
    p[4].i = height;

    gl_add_op(p);
}

void pglFrustum(double left, double right, double bottom, double top,
               double nearz, double farv) {
    GLParam p[7];

    p[0].op = OP_Frustum;
    p[1].f = left;
    p[2].f = right;
    p[3].f = bottom;
    p[4].f = top;
    p[5].f = nearz;
    p[6].f = farv;

    gl_add_op(p);
}

/* lightening */

void pglMaterialfv(GLenum mode, GLenum type, const float *v) {
    GLParam p[7];
    int i, n;

    assert(mode == GL_FRONT || mode == GL_BACK || mode == GL_FRONT_AND_BACK);

    p[0].op = OP_Material;
    p[1].i = mode;
    p[2].i = type;
    n = 4;
    if (type == GL_SHININESS) n = 1;
    for (i = 0; i < 4; i++) p[3 + i].f = v[i];
    for (i = n; i < 4; i++) p[3 + i].f = 0;

    gl_add_op(p);
}

void pglMaterialf(GLenum mode, GLenum type, float v) {
    GLParam p[7];
    int i;

    p[0].op = OP_Material;
    p[1].i = mode;
    p[2].i = type;
    p[3].f = v;
    for (i = 0; i < 3; i++) p[4 + i].f = 0;

    gl_add_op(p);
}

void pglColorMaterial(GLenum mode, GLenum type) {
    GLParam p[3];

    p[0].op = OP_ColorMaterial;
    p[1].i = mode;
    p[2].i = type;

    gl_add_op(p);
}

void pglLightfv(GLenum light, GLenum type, const float *v) {
    GLParam p[7];
    int i;

    p[0].op = OP_Light;
    p[1].i = light;
    p[2].i = type;
    /* TODO: 3 composants ? */
    for (i = 0; i < 4; i++) p[3 + i].f = v[i];

    gl_add_op(p);
}


void pglLightf(GLenum light, GLenum type, float v) {
    GLParam p[7];
    int i;

    p[0].op = OP_Light;
    p[1].i = light;
    p[2].i = type;
    p[3].f = v;
    for (i = 0; i < 3; i++) p[4 + i].f = 0;

    gl_add_op(p);
}

void pglLightModeli(GLenum pname, int param) {
    GLParam p[6];
    int i;

    p[0].op = OP_LightModel;
    p[1].i = pname;
    p[2].f = (float) param;
    for (i = 0; i < 3; i++) p[3 + i].f = 0;
    gl_add_op(p);
}

void pglLightModelfv(GLenum pname, float *param) {
    GLParam p[6];
    int i;

    p[0].op = OP_LightModel;
    p[1].i = pname;
    for (i = 0; i < 4; i++) p[2 + i].f = param[i];

    gl_add_op(p);
}

/* clear */

void pglClear(GLbitfield mask) {
    GLParam p[2];

    p[0].op = OP_Clear;
    p[1].i = mask;

    gl_add_op(p);
}

void pglClearColor(float r, float g, float b, float a) {
    GLParam p[5];

    p[0].op = OP_ClearColor;
    p[1].f = r;
    p[2].f = g;
    p[3].f = b;
    p[4].f = a;

    gl_add_op(p);
}

void pglClearDepth(double depth) {
    GLParam p[2];

    p[0].op = OP_ClearDepth;
    p[1].f = depth;

    gl_add_op(p);
}


/* textures */

void pglTexImage2D(GLenum target, int level, int components,
                  GLsizei width, GLsizei height, int border,
                  GLenum format, GLenum type, const void *pixels) {
    GLParam p[10];

    p[0].op = OP_TexImage2D;
    p[1].i = target;
    p[2].i = level;
    p[3].i = components;
    p[4].i = width;
    p[5].i = height;
    p[6].i = border;
    p[7].i = format;
    p[8].i = type;
    p[9].p = pixels;

    gl_add_op(p);
}


void pglBindTexture(GLenum target, GLuint texture) {
    GLParam p[3];

    p[0].op = OP_BindTexture;
    p[1].i = target;
    p[2].i = texture;

    gl_add_op(p);
}

void pglTexEnvi(GLenum target, GLenum pname, int param) {
    GLParam p[8];

    p[0].op = OP_TexEnv;
    p[1].i = target;
    p[2].i = pname;
    p[3].i = param;
    p[4].f = 0;
    p[5].f = 0;
    p[6].f = 0;
    p[7].f = 0;

    gl_add_op(p);
}

void pglTexParameteri(GLenum target, GLenum pname, int param) {
    GLParam p[8];

    p[0].op = OP_TexParameter;
    p[1].i = target;
    p[2].i = pname;
    p[3].i = param;
    p[4].f = 0;
    p[5].f = 0;
    p[6].f = 0;
    p[7].f = 0;

    gl_add_op(p);
}

void pglPixelStorei(GLenum pname, int param) {
    GLParam p[3];

    p[0].op = OP_PixelStore;
    p[1].i = pname;
    p[2].i = param;

    gl_add_op(p);
}

/* selection */

void pglInitNames(void) {
    GLParam p[1];

    p[0].op = OP_InitNames;

    gl_add_op(p);
}

void pglPushName(unsigned int name) {
    GLParam p[2];

    p[0].op = OP_PushName;
    p[1].i = name;

    gl_add_op(p);
}

void pglPopName(void) {
    GLParam p[1];

    p[0].op = OP_PopName;

    gl_add_op(p);
}

void pglLoadName(unsigned int name) {
    GLParam p[2];

    p[0].op = OP_LoadName;
    p[1].i = name;

    gl_add_op(p);
}

void
pglPolygonOffset(GLfloat factor, GLfloat units) {
    GLParam p[3];
    p[0].op = OP_PolygonOffset;
    p[1].f = factor;
    p[2].f = units;

    gl_add_op(p);
}

/* Special Functions */

void pglCallList(unsigned int list) {
    GLParam p[2];

    p[0].op = OP_CallList;
    p[1].i = list;

    gl_add_op(p);
}

void pglFlush(void) {
    /* nothing to do */
}

void pglHint(int target, int mode) {
    GLParam p[3];

    p[0].op = OP_Hint;
    p[1].i = target;
    p[2].i = mode;

    gl_add_op(p);
}

/* Non standard functions */

void pglDebug(int mode) {
    GLContext *c = gl_get_context();
    c->print_flag = mode;
}

static void __gluMakeIdentityf(GLfloat m[16]) {
    m[0 + 4 * 0] = 1;
    m[0 + 4 * 1] = 0;
    m[0 + 4 * 2] = 0;
    m[0 + 4 * 3] = 0;
    m[1 + 4 * 0] = 0;
    m[1 + 4 * 1] = 1;
    m[1 + 4 * 2] = 0;
    m[1 + 4 * 3] = 0;
    m[2 + 4 * 0] = 0;
    m[2 + 4 * 1] = 0;
    m[2 + 4 * 2] = 1;
    m[2 + 4 * 3] = 0;
    m[3 + 4 * 0] = 0;
    m[3 + 4 * 1] = 0;
    m[3 + 4 * 2] = 0;
    m[3 + 4 * 3] = 1;
}

static void normalize(float v[3]) {
    float r;

    r = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (r == 0.0) return;

    v[0] /= r;
    v[1] /= r;
    v[2] /= r;
}

static void cross(float v1[3], float v2[3], float result[3]) {
    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

// ---------------------------------------------------------------------

void pgluLookAt(GLdouble eyex, GLdouble eyey, GLdouble eyez, GLdouble centerx,
               GLdouble centery, GLdouble centerz, GLdouble upx, GLdouble upy,
               GLdouble upz) {
    float forward[3], side[3], up[3];
    GLfloat m[4][4];

    forward[0] = centerx - eyex;
    forward[1] = centery - eyey;
    forward[2] = centerz - eyez;

    up[0] = upx;
    up[1] = upy;
    up[2] = upz;

    normalize(forward);

    /* Side = forward x up */
    cross(forward, up, side);
    normalize(side);

    /* Recompute up as: up = side x forward */
    cross(side, forward, up);

    __gluMakeIdentityf(&m[0][0]);
    m[0][0] = side[0];
    m[1][0] = side[1];
    m[2][0] = side[2];

    m[0][1] = up[0];
    m[1][1] = up[1];
    m[2][1] = up[2];

    m[0][2] = -forward[0];
    m[1][2] = -forward[1];
    m[2][2] = -forward[2];

    pglMultMatrixf(&m[0][0]);
    pglTranslatef(-eyex, -eyey, -eyez);
}

void pgluPerspective(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar) {
    const GLdouble pi = 3.1415926535897932384626433832795;
    GLdouble fW, fH;
    fH = tan(fovY / 360 * pi) * zNear;
    fW = fH * aspect;
    pglFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void pglOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top,
              GLfloat nearz, GLfloat farz) {

    float m[4 * 4];
    m[0x0] = 2.0f / (right - left);
    m[0x4] = 0.0f;
    m[0x8] = 0.0f;
    m[0xC] = -(right + left) / (right - left);

    m[0x1] = 0.0f;
    m[0x5] = 2.0f / (top - bottom);
    m[0x9] = 0.0f;
    m[0xD] = -(top + bottom) / (top - bottom);

    m[0x2] = 0.0f;
    m[0x6] = 0.0f;
    m[0xA] = -2.0f / (farz - nearz);
    m[0xE] = (farz + nearz) / (farz - nearz);

    m[0x3] = 0.0f;
    m[0x7] = 0.0f;
    m[0xB] = 0.0f;
    m[0xF] = 1.0f;

    pglLoadMatrixf(m);

}
