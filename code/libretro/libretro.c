#include "../libretro-common/include/libretro.h"
#include "../libretro-common/include/retro_dirent.h"
#include "../libretro-common/include/features/features_cpu.h"
#include "../libretro-common/include/file/file_path.h"
#include "../libretro-common/include/glsym/glsym.h"

#include "libretro_core_options.h"
#include "sys_local.h"
#include "sys_loadlib.h"
#include "../qcommon/q_shared.h"
#include "sys_local.h"
#include "../renderercommon/tr_common.h"
#include "../qcommon/qcommon.h"
#include "../client/client.h"
#include "../client/snd_local.h"

#if defined(HAVE_PSGL)
#define RARCH_GL_FRAMEBUFFER GL_FRAMEBUFFER_OES
#define RARCH_GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_OES
#define RARCH_GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT
#elif defined(OSX_PPC)
#define RARCH_GL_FRAMEBUFFER GL_FRAMEBUFFER_EXT
#define RARCH_GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_EXT
#define RARCH_GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT
#else
#define RARCH_GL_FRAMEBUFFER GL_FRAMEBUFFER
#define RARCH_GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE
#define RARCH_GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0
#endif

#define SAMPLE_RATE   	48000
#define BUFFER_SIZE 	32768

int framerate = 165;
static int invert_y_axis = 1;
static unsigned audio_buffer_ptr;
static int16_t audio_buffer[BUFFER_SIZE];

int scr_width = 960, scr_height = 544;

void ( APIENTRY * qglBlendFunc )(GLenum sfactor, GLenum dfactor);
void ( APIENTRY * qglTexImage2D )(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
void ( APIENTRY * qglTexParameteri )(GLenum target, GLenum pname, GLint param);
void ( APIENTRY * qglBindFramebuffer )(GLenum target, GLuint framebuffer);
void ( APIENTRY * qglGenerateMipmap )(GLenum target);
void ( APIENTRY * qglTexSubImage2D )(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
void ( APIENTRY * qglDepthMask )(GLboolean flag);
void ( APIENTRY * qglPushMatrix )(void);
void ( APIENTRY * qglRotatef )(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void ( APIENTRY * qglTranslatef )(GLfloat x, GLfloat y, GLfloat z);
void ( APIENTRY * qglDepthRange )(GLclampd zNear, GLclampd zFar);
void ( APIENTRY * qglClear )(GLbitfield mask);
void ( APIENTRY * qglEnable )(GLenum cap);
void ( APIENTRY * qglDisable )(GLenum cap);
void ( APIENTRY * qglPopMatrix )(void);
void ( APIENTRY * qglGetFloatv )(GLenum pname, GLfloat *params);
void ( APIENTRY * qglOrtho )(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
void ( APIENTRY * qglFrustum )(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
void ( APIENTRY * qglLoadMatrixf )(const GLfloat *m);
void ( APIENTRY * qglLoadIdentity )(void);
void ( APIENTRY * qglMatrixMode )(GLenum mode);
void ( APIENTRY * qglBindTexture )(GLenum target, GLuint texture);
void ( APIENTRY * qglReadPixels )(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
void ( APIENTRY * qglPolygonMode )(GLenum face, GLenum mode);
void ( APIENTRY * qglVertexPointer )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void ( APIENTRY * qglColorPointer )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void ( APIENTRY * qglTexCoordPointer )(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
void ( APIENTRY * qglDrawElements )(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
void ( APIENTRY * qglClearColor )(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
void ( APIENTRY * qglCullFace )(GLenum mode);
void ( APIENTRY * qglViewport )(GLint x, GLint y, GLsizei width, GLsizei height);
void ( APIENTRY * qglDeleteTextures )(GLsizei n, const GLuint *textures);
void ( APIENTRY * qglClearStencil )(GLint s);
void ( APIENTRY * qglColor4f )(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
void ( APIENTRY * qglScissor )(GLint x, GLint y, GLsizei width, GLsizei height);
void ( APIENTRY * qglEnableClientState )(GLenum array);
void ( APIENTRY * qglDisableClientState )(GLenum array);
void ( APIENTRY * qglStencilFunc )(GLenum func, GLint ref, GLuint mask);
void ( APIENTRY * qglStencilOp )(GLenum fail, GLenum zfail, GLenum zpass);
void ( APIENTRY * qglScalef )(GLfloat x, GLfloat y, GLfloat z);
void ( APIENTRY * qglDepthFunc )(GLenum func);
void ( APIENTRY * qglTexEnvi )(GLenum target, GLenum pname, GLint param);
void ( APIENTRY * qglAlphaFunc )(GLenum func,  GLclampf ref);
void ( APIENTRY * qglClearDepth )(GLdouble depth);
void ( APIENTRY * qglFinish )(void);
void ( APIENTRY * qglGenTextures )(GLsizei n,GLuint * textures);
void ( APIENTRY * qglPolygonOffset )(GLfloat factor, GLfloat units);
void ( APIENTRY * qglClipPlane )(GLenum plane, const GLdouble * equation);
void ( APIENTRY * qglColorMask )(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
void ( APIENTRY * qglLineWidth )(GLfloat width);
void ( APIENTRY * qglStencilMask )(GLuint mask);

#define GL_FUNCS_NUM 52

#define MAX_INDICES 4096
uint16_t* indices;
float *gVertexBuffer;
uint8_t *gColorBuffer;
uint8_t *gColorBuffer255;
float *gTexCoordBuffer;
float *gVertexBufferPtr;
uint8_t *gColorBufferPtr;
float *gTexCoordBufferPtr;

typedef struct api_entry{
	void *ptr;
} api_entry;

api_entry funcs[GL_FUNCS_NUM];

char g_rom_dir[1024], g_pak_path[1024], g_save_dir[1024];

static struct retro_hw_render_callback hw_render;

void vglVertexPointerMapped(const GLvoid *pointer) {
	qglVertexPointer(3, GL_FLOAT, 0, pointer);
}

void vglTexCoordPointerMapped(const GLvoid *pointer) {
	qglTexCoordPointer(2, GL_FLOAT, 0, pointer);
}

void vglColorPointerMapped(GLenum type, const GLvoid *pointer) {
	qglColorPointer(4, type, 0, pointer);
}

void vglDrawObjects(GLenum mode, GLsizei count, GLboolean implicit_wvp) {
	qglDrawElements(mode, count, GL_UNSIGNED_SHORT, indices);
}

void vglTexCoordPointer(GLint size, GLenum type, GLsizei stride, GLuint count, const GLvoid *pointer) {
	vglTexCoordPointerMapped(pointer);
}

void vglVertexPointer(GLint size, GLenum type, GLsizei stride, GLuint count, const GLvoid *pointer) {
	vglVertexPointerMapped(pointer);
}

void vglColorPointer(GLint size, GLenum type, GLsizei stride, GLuint count, const GLvoid *pointer) {
	vglColorPointerMapped(type, pointer);
}

static retro_log_printf_t log_cb;
static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
retro_environment_t environ_cb;
static retro_input_poll_t poll_cb;
static retro_input_state_t input_cb;
static struct retro_rumble_interface rumble;
static bool libretro_supports_bitmasks = false;

static void audio_callback(void);

#define MAX_PADS 1
static unsigned quake_devices[1];

// System analog stick range is -0x8000 to 0x8000
#define ANALOG_RANGE 0x8000
// Default deadzone: 15%
static int analog_deadzone = (int)(0.15f * ANALOG_RANGE);

#define GP_MAXBINDS 32


typedef struct {
   struct retro_input_descriptor desc[GP_MAXBINDS];
   struct {
      char *key;
      char *com;
   } bind[GP_MAXBINDS];
} gp_layout_t;

gp_layout_t modern = {
   {
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "Swim Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "Strafe Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "Strafe Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Swim Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,     "Previous Weapon" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,     "Next Weapon" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,    "Jump" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,    "Fire" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,"Show Scores" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Menu" },
      { 0 },
   },
   {
      {"JOY_LEFT",  "+moveleft"},     {"JOY_RIGHT", "+moveright"},
      {"JOY_DOWN",  "+back"},         {"JOY_UP",    "+forward"},
      {"JOY_B",     "+movedown"},     {"JOY_A",     "+moveright"},
      {"JOY_X",     "+moveup"},       {"JOY_Y",     "+moveleft"},
      {"JOY_L",     "impulse 12"},    {"JOY_R",     "impulse 10"},
      {"JOY_L2",    "+jump"},         {"JOY_R2",    "+attack"},
      {"JOY_SELECT","+showscores"},   {"JOY_START", "togglemenu"},
      { 0 },
   },
};
gp_layout_t classic = {

   {
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "Jump" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "Cycle Weapon" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "Freelook" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Fire" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,     "Strafe Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,     "Strafe Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,    "Look Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,    "Look Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3,    "Move Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3,    "Swim Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,"Toggle Run Mode" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Menu" },
      { 0 },
   },
   {
      {"JOY_LEFT",  "+left"},         {"JOY_RIGHT", "+right"},
      {"JOY_DOWN",  "+back"},         {"JOY_UP",    "+forward"},
      {"JOY_B",     "+jump"} ,        {"JOY_A",     "impulse 10"},
      {"JOY_X",     "+klook"},        {"JOY_Y",     "+attack"},
      {"JOY_L",     "+moveleft"},     {"JOY_R",     "+moveright"},
      {"JOY_L2",    "+lookup"},       {"JOY_R2",    "+lookdown"},
      {"JOY_L3",    "+movedown"},     {"JOY_R3",    "+moveup"},
      {"JOY_SELECT","+togglewalk"},   {"JOY_START", "togglemenu"},
      { 0 },
   },
};
gp_layout_t classic_alt = {

   {
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "Look Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "Look Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "Look Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "Look Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,     "Jump" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,     "Fire" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,    "Run" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,    "Next Weapon" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3,    "Move Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3,    "Previous Weapon" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,"Toggle Run Mode" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Menu" },
      { 0 },
   },
   {
      {"JOY_LEFT",  "+moveleft"},     {"JOY_RIGHT", "+moveright"},
      {"JOY_DOWN",  "+back"},         {"JOY_UP",    "+forward"},
      {"JOY_B",     "+lookdown"},     {"JOY_A",     "+right"},
      {"JOY_X",     "+lookup"},       {"JOY_Y",     "+left"},
      {"JOY_L",     "+jump"},         {"JOY_R",     "+attack"},
      {"JOY_L2",    "+speed"},          {"JOY_R2",    "impulse 10"},
      {"JOY_L3",    "+movedown"},     {"JOY_R3",    "impulse 12"},
      {"JOY_SELECT","+togglewalk"},   {"JOY_START", "togglemenu"},
      { 0 },
   },
};

gp_layout_t *gp_layoutp = NULL;

/* sys.c */
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "errno.h"

#define RETRO_DEVICE_MODERN  RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_ANALOG, 2)
#define RETRO_DEVICE_JOYPAD_ALT  RETRO_DEVICE_SUBCLASS(RETRO_DEVICE_JOYPAD, 1)

static bool context_needs_reinit = true;

static bool initialize_gl()
{
	funcs[0].ptr  = qglTexImage2D         = hw_render.get_proc_address ("glTexImage2D");
	funcs[1].ptr  = qglTexSubImage2D      = hw_render.get_proc_address ("glTexSubImage2D");
	funcs[2].ptr  = qglTexParameteri      = hw_render.get_proc_address ("glTexParameteri");
	funcs[3].ptr  = qglBindFramebuffer    = hw_render.get_proc_address ("glBindFramebuffer");
	funcs[4].ptr  = qglGenerateMipmap     = hw_render.get_proc_address ("glGenerateMipmap");
	funcs[5].ptr  = qglBlendFunc          = hw_render.get_proc_address ("glBlendFunc");
	funcs[6].ptr  = qglTexSubImage2D      = hw_render.get_proc_address ("glTexSubImage2D");
	funcs[7].ptr  = qglDepthMask          = hw_render.get_proc_address ("glDepthMask");
	funcs[8].ptr  = qglPushMatrix         = hw_render.get_proc_address ("glPushMatrix");
	funcs[9].ptr  = qglRotatef            = hw_render.get_proc_address ("glRotatef");
	funcs[10].ptr = qglTranslatef         = hw_render.get_proc_address ("glTranslatef");
	funcs[11].ptr = qglDepthRange         = hw_render.get_proc_address ("glDepthRange");
	funcs[12].ptr = qglClear              = hw_render.get_proc_address ("glClear");
	funcs[13].ptr = qglCullFace           = hw_render.get_proc_address ("glCullFace");
	funcs[14].ptr = qglClearColor         = hw_render.get_proc_address ("glClearColor");
	funcs[15].ptr = qglEnable             = hw_render.get_proc_address ("glEnable");
	funcs[16].ptr = qglDisable            = hw_render.get_proc_address ("glDisable");
	funcs[17].ptr = qglEnableClientState  = hw_render.get_proc_address ("glEnableClientState");
	funcs[18].ptr = qglDisableClientState = hw_render.get_proc_address ("glDisableClientState");
	funcs[19].ptr = qglPopMatrix          = hw_render.get_proc_address ("glPopMatrix");
	funcs[20].ptr = qglGetFloatv          = hw_render.get_proc_address ("glGetFloatv");
	funcs[21].ptr = qglOrtho              = hw_render.get_proc_address ("glOrtho");
	funcs[22].ptr = qglFrustum            = hw_render.get_proc_address ("glFrustum");
	funcs[23].ptr = qglLoadMatrixf        = hw_render.get_proc_address ("glLoadMatrixf");
	funcs[24].ptr = qglLoadIdentity       = hw_render.get_proc_address ("glLoadIdentity");
	funcs[25].ptr = qglMatrixMode         = hw_render.get_proc_address ("glMatrixMode");
	funcs[26].ptr = qglBindTexture        = hw_render.get_proc_address ("glBindTexture");
	funcs[27].ptr = qglReadPixels         = hw_render.get_proc_address ("glReadPixels");
	funcs[28].ptr = qglPolygonMode        = hw_render.get_proc_address ("glPolygonMode");
	funcs[29].ptr = qglVertexPointer      = hw_render.get_proc_address ("glVertexPointer");
	funcs[30].ptr = qglTexCoordPointer    = hw_render.get_proc_address ("glTexCoordPointer");
	funcs[31].ptr = qglColorPointer       = hw_render.get_proc_address ("glColorPointer");
	funcs[32].ptr = qglDrawElements       = hw_render.get_proc_address ("glDrawElements");
	funcs[33].ptr = qglViewport           = hw_render.get_proc_address ("glViewport");
	funcs[34].ptr = qglDeleteTextures     = hw_render.get_proc_address ("glDeleteTextures");
	funcs[35].ptr = qglClearStencil       = hw_render.get_proc_address ("glClearStencil");
	funcs[36].ptr = qglColor4f            = hw_render.get_proc_address ("glColor4f");
	funcs[37].ptr = qglScissor            = hw_render.get_proc_address ("glScissor");
	funcs[38].ptr = qglStencilFunc        = hw_render.get_proc_address ("glStencilFunc");
	funcs[39].ptr = qglStencilOp          = hw_render.get_proc_address ("glStencilOp");
	funcs[40].ptr = qglScalef             = hw_render.get_proc_address ("glScalef");
	funcs[41].ptr = qglDepthFunc          = hw_render.get_proc_address ("glDepthFunc");
	funcs[42].ptr = qglTexEnvi            = hw_render.get_proc_address ("glTexEnvi");
	funcs[43].ptr = qglAlphaFunc          = hw_render.get_proc_address ("glAlphaFunc");
	funcs[44].ptr = qglClearDepth         = hw_render.get_proc_address ("glClearDepth");
	funcs[45].ptr = qglFinish             = hw_render.get_proc_address ("glFinish");
	funcs[46].ptr = qglGenTextures        = hw_render.get_proc_address ("glGenTextures");
	funcs[47].ptr = qglPolygonOffset      = hw_render.get_proc_address ("glPolygonOffset");
	funcs[48].ptr = qglClipPlane          = hw_render.get_proc_address ("glClipPlane");
	funcs[49].ptr = qglColorMask          = hw_render.get_proc_address ("glColorMask");
	funcs[50].ptr = qglLineWidth          = hw_render.get_proc_address ("glLineWidth");
	funcs[51].ptr = qglStencilMask        = hw_render.get_proc_address ("glStencilMask");
	
	if (log_cb) {
		int i;
		for (i = 0; i < GL_FUNCS_NUM; i++) {
			if (!funcs[i].ptr) log_cb(RETRO_LOG_ERROR, "vitaQuakeII: cannot get GL function #%d symbol.\n", i);
		}
	}
	
	return true;
}

static void context_destroy() 
{
	context_needs_reinit = true;
}

static void keyboard_cb(bool down, unsigned keycode, uint32_t character, uint16_t mod)
{
	// character-only events are discarded
	if (keycode != RETROK_UNKNOWN) {
		if (down)
			Sys_SetKeys((uint32_t) keycode, 1);
		else
			Sys_SetKeys((uint32_t) keycode, 0);
	}
}

static void context_reset() { 
	if (!context_needs_reinit)
		return;

	initialize_gl();
	/*if (!first_reset) {
		restore_textures();
	}
	first_reset = false;*/
	context_needs_reinit = false;
}

/* con.c */

/*
==================
CON_Shutdown
==================
*/
void CON_Shutdown(void) {
}

/*
==================
CON_Init
==================
*/
void CON_Init(void) {
}

/*
==================
CON_Input
==================
*/
char *CON_Input(void) {
    return NULL;
}

/*
==================
CON_Print
==================
*/
void CON_Print(const char *msg) {
#ifndef RELEASE
    printf(msg);
#endif
}

/* glimp_gamma.c */

void GLimp_SetGamma( unsigned char red[256], unsigned char green[256], unsigned char blue[256] )
{
}

/* sys_main.c */
static char binaryPath[MAX_OSPATH] = {0};
static char installPath[MAX_OSPATH] = {0};

/*
=================
Sys_SetBinaryPath
=================
*/
void Sys_SetBinaryPath(const char *path) {
    Q_strncpyz(binaryPath, path, sizeof(binaryPath));
}

/*
=================
Sys_BinaryPath
=================
*/
char *Sys_BinaryPath(void) {
    return binaryPath;
}

/*
=================
Sys_SetDefaultInstallPath
=================
*/
void Sys_SetDefaultInstallPath(const char *path) {
    Q_strncpyz(installPath, path, sizeof(installPath));
}

/*
=================
Sys_DefaultInstallPath
=================
*/
char *Sys_DefaultInstallPath(void) {
    if (*installPath)
        return installPath;
    else
        return Sys_Cwd();
}

/*
=================
Sys_DefaultAppPath
=================
*/
char *Sys_DefaultAppPath(void) {
    return Sys_BinaryPath();
}

/*
=================
Sys_In_Restart_f
Restart the input subsystem
=================
*/
void Sys_In_Restart_f(void) {
    IN_Restart();
}

/*
=================
Sys_ConsoleInput
Handle new console input
=================
*/
char *Sys_ConsoleInput(void) {
    return CON_Input();
}

/*
==================
Sys_GetClipboardData
==================
*/
char *Sys_GetClipboardData(void) {
#ifdef DEDICATED
    return NULL;
#else
    char *data = NULL;
    char *cliptext;

   /* if ((cliptext = SDL_GetClipboardText()) != NULL) {
        if (cliptext[0] != '\0') {
            size_t bufsize = strlen(cliptext) + 1;
            data = Z_Malloc(bufsize);
            Q_strncpyz(data, cliptext, bufsize);
            // find first listed char and set to '\0'
            strtok(data, "\n\r\b");
        }
        SDL_free(cliptext);
    }*/
    return data;
#endif
}

void Sys_InitPIDFile(const char *gamedir) {}

void Sys_RemovePIDFile(const char *gamedir) {}

/*
=================
Sys_Exit
Single exit point (regular exit or in case of error)
=================
*/
static __attribute__ ((noreturn)) void Sys_Exit(int exitCode) {
    CON_Shutdown();

    NET_Shutdown();

    Sys_PlatformExit();

    exit(exitCode);
}

/*
=================
Sys_Quit
=================
*/
void Sys_Quit(void) {
    Sys_Exit(0);
}

/*
=================
Sys_GetProcessorFeatures
=================
*/
cpuFeatures_t Sys_GetProcessorFeatures(void) {
    return (cpuFeatures_t) 0;
}

/*
=================
Sys_Init
=================
*/
void Sys_Init(void) {
    Cmd_AddCommand("in_restart", Sys_In_Restart_f);
    Cvar_Set("arch", OS_STRING " " ARCH_STRING);
    Cvar_Set("username", Sys_GetCurrentUser());
}

/*
=================
Sys_Print
=================
*/
void Sys_Print(const char *msg) {
    CON_Print(msg);
}

/*
=================
Sys_Error
=================
*/
void Sys_Error(const char *error, ...) {
    va_list argptr;
    char string[1024];

    va_start (argptr, error);
    Q_vsnprintf(string, sizeof(string), error, argptr);
    va_end (argptr);

    Sys_ErrorDialog(string);

    Sys_Exit(3);
}

/*
============
Sys_FileTime
returns -1 if not present
============
*/
int Sys_FileTime(char *path) {
    return -1;
}

/*
=================
Sys_UnloadDll
=================
*/
void Sys_UnloadDll( void *dllHandle )
{
}

/*
=================
Sys_LoadDll
First try to load library name from system library path,
from executable path, then fs_basepath.
=================
*/

void *Sys_LoadDll(const char *name, qboolean useSystemLib)
{
	return NULL;
}

/*
=================
Sys_LoadGameDll
Used to load a development dll instead of a virtual machine
=================
*/
void *Sys_LoadGameDll(const char *name,
	intptr_t (QDECL **entryPoint)(int, ...),
	intptr_t (*systemcalls)(intptr_t, ...))
{
	return NULL;
}

/*
=================
Sys_ParseArgs
=================
*/
void Sys_ParseArgs(int argc, char **argv) {
    if (argc == 2) {
        if (!strcmp(argv[1], "--version") ||
            !strcmp(argv[1], "-v")) {
            const char *date = PRODUCT_DATE;
#ifdef DEDICATED
            fprintf( stdout, Q3_VERSION " dedicated server (%s)\n", date );
#else
            fprintf(stdout, Q3_VERSION " client (%s)\n", date);
#endif
            Sys_Exit(0);
        }
    }
}

#ifndef DEFAULT_BASEDIR
#	define DEFAULT_BASEDIR Sys_BinaryPath()
#endif

/*
=================
Sys_SigHandler
=================
*/
void Sys_SigHandler(int signal) {
}

static void extract_directory(char *buf, const char *path, size_t size)
{
   char *base = NULL;

   strncpy(buf, path, size - 1);
   buf[size - 1] = '\0';

   base = strrchr(buf, '/');
   if (!base)
      base = strrchr(buf, '\\');

   if (base)
      *base = '\0';
   else
    {
       buf[0] = '.';
       buf[1] = '\0';
    }
}

void retro_init(void)
{
   struct retro_log_callback log;

   if(environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
      log_cb = log.log;
   else
      log_cb = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_INPUT_BITMASKS, NULL))
      libretro_supports_bitmasks = true;
}

void retro_deinit(void)
{
   libretro_supports_bitmasks = false;
}

void gp_layout_set_bind(gp_layout_t gp_layout)
{
   char buf[100];
   unsigned i;
   for (i=0; gp_layout.bind[i].key; ++i)
   {
      snprintf(buf, sizeof(buf), "bind %s \"%s\"\n", gp_layout.bind[i].key,
                                                   gp_layout.bind[i].com);
      Cbuf_AddText(buf);
   }
}

bool initial_resolution_set = false;
static void update_variables(bool startup)
{
	struct retro_variable var;
	
	var.key = "vitaquakeiii_framerate";
	var.value = NULL;
	
	if (startup)
	{
		if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var))
		{
			if (!strcmp(var.value, "auto"))
			{
				float target_framerate = 0.0f;
				if (!environ_cb(RETRO_ENVIRONMENT_GET_TARGET_REFRESH_RATE, &target_framerate))
					target_framerate = 60.0f;
				framerate = (unsigned)target_framerate;
			}
			else
				framerate = atoi(var.value);
		}
		else
			framerate    = 60;
	}
	
	var.key = "vitaquakeiii_resolution";
	var.value = NULL;
	
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && !initial_resolution_set)
	{
		char *pch;
		char str[100];
		snprintf(str, sizeof(str), "%s", var.value);

		pch = strtok(str, "x");
		if (pch)
			scr_width = strtoul(pch, NULL, 0);
		pch = strtok(NULL, "x");
		if (pch)
			scr_height = strtoul(pch, NULL, 0);

		if (log_cb)
			log_cb(RETRO_LOG_INFO, "Got size: %u x %u.\n", scr_width, scr_height);

		initial_resolution_set = true;
	}
   
	var.key = "vitaquakeiii_invert_y_axis";
	var.value = NULL;

	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
	{
		if (strcmp(var.value, "disabled") == 0)
			invert_y_axis = 1;
		else
			invert_y_axis = -1;
	}
	
	// We need setup sequence to be finished to change Cvar values
	if (!startup) {
		var.key = "vitaquakeiii_fps";
		var.value = NULL;

		if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
		{
			if (strcmp(var.value, "disabled") == 0)
				Cvar_SetValue("cg_drawFPS", 0);
			else
				Cvar_SetValue("cg_drawFPS", 1);
		}
		
		var.key = "vitaquakeiii_pickups";
		var.value = NULL;

		if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
		{
			if (strcmp(var.value, "disabled") == 0)
				Cvar_SetValue("cg_simpleItems", 0);
			else
				Cvar_SetValue("cg_simpleItems", 1);
		}
		
		var.key = "vitaquakeiii_weapon";
		var.value = NULL;

		if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
		{
			if (strcmp(var.value, "disabled") == 0)
				Cvar_SetValue("cg_drawgun", 0);
			else
				Cvar_SetValue("cg_drawgun", 1);
		}
		
		var.key = "vitaquakeiii_shadows";
		var.value = NULL;
		
		if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
		{
			if (strcmp(var.value, "disabled") == 0)
				Cvar_SetValue("cg_shadows", 0);
			else if (strcmp(var.value, "low") == 0)
				Cvar_SetValue("cg_shadows", 1);
			else if (strcmp(var.value, "high") == 0)
				Cvar_SetValue("cg_shadows", 3);
		}
		
		var.key = "vitaquakeiii_filter";
		var.value = NULL;
		
		if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
		{
			if (strcmp(var.value, "disabled") == 0)
				Cvar_Set("r_textureMode", "GL_NEAREST");
			else if (strcmp(var.value, "linear") == 0)
				Cvar_Set("r_textureMode", "GL_LINEAR");
			else if (strcmp(var.value, "bilinear") == 0)
				Cvar_Set("r_textureMode", "GL_LINEAR_MIPMAP_NEAREST");
			else
				Cvar_Set("r_textureMode", "GL_LINEAR_MIPMAP_LINEAR");
		}
	}
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
   if (port == 0)
   {
      switch (device)
      {
         case RETRO_DEVICE_JOYPAD:
            quake_devices[port] = RETRO_DEVICE_JOYPAD;
            environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, classic.desc);
            gp_layout_set_bind(classic);
            break;
         case RETRO_DEVICE_JOYPAD_ALT:
            quake_devices[port] = RETRO_DEVICE_JOYPAD;
            environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, classic_alt.desc);
            gp_layout_set_bind(classic_alt);
            break;
         case RETRO_DEVICE_MODERN:
            quake_devices[port] = RETRO_DEVICE_MODERN;
            environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, modern.desc);
            gp_layout_set_bind(modern);
            break;
         case RETRO_DEVICE_KEYBOARD:
            quake_devices[port] = RETRO_DEVICE_KEYBOARD;
            break;
         case RETRO_DEVICE_NONE:
         default:
            quake_devices[port] = RETRO_DEVICE_NONE;
            if (log_cb)
               log_cb(RETRO_LOG_ERROR, "[libretro]: Invalid device.\n");
      }
   }
}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = "vitaQuakeIII";
#ifndef GIT_VERSION
#define GIT_VERSION ""
#endif
   info->library_version  = "v0.8" ;
   info->need_fullpath    = true;
   info->valid_extensions = "pk3";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   info->timing.fps            = framerate;
   info->timing.sample_rate    = SAMPLE_RATE;

   info->geometry.base_width   = scr_width;
   info->geometry.base_height  = scr_height;
   info->geometry.max_width    = 3840;
   info->geometry.max_height   = 2160;
   info->geometry.aspect_ratio = (scr_width * 1.0f) / (scr_height * 1.0f);
}

void retro_set_environment(retro_environment_t cb)
{
   static const struct retro_controller_description port_1[] = {
      { "Gamepad Classic", RETRO_DEVICE_JOYPAD },
      { "Gamepad Classic Alt", RETRO_DEVICE_JOYPAD_ALT },
      { "Gamepad Modern", RETRO_DEVICE_MODERN },
      { "Keyboard + Mouse", RETRO_DEVICE_KEYBOARD },
   };

   static const struct retro_controller_info ports[] = {
      { port_1, 3 },
      { 0 },
   };

   environ_cb = cb;

   libretro_set_core_options(environ_cb);
   cb(RETRO_ENVIRONMENT_SET_CONTROLLER_INFO, (void*)ports);
}

void retro_reset(void)
{
}

void retro_set_rumble_strong(void)
{
}

void retro_unset_rumble_strong(void)
{
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   input_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

void retro_unload_game(void)
{
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
   (void)type;
   (void)info;
   (void)num;
   return false;
}

size_t retro_serialize_size(void)
{
   return 0;
}

bool retro_serialize(void *data_, size_t size)
{
   return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
   return false;
}

void *retro_get_memory_data(unsigned id)
{
   (void)id;
   return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
   (void)id;
   return 0;
}

void retro_cheat_reset(void)
{}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
   (void)index;
   (void)enabled;
   (void)code;
}

static void extract_basename(char *buf, const char *path, size_t size)
{
   char *ext        = NULL;
   const char *base = strrchr(path, '/');
   if (!base)
      base = strrchr(path, '\\');
   if (!base)
      base = path;

   if (*base == '\\' || *base == '/')
      base++;

   strncpy(buf, base, size - 1);
   buf[size - 1] = '\0';

   ext = strrchr(buf, '.');
   if (ext)
      *ext = '\0';
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

char full_game_dir[256];

static void audio_process(void)
{
}

static void audio_callback(void)
{
	unsigned read_first, read_second;
	float samples_per_frame = (2 * SAMPLE_RATE) / framerate;
	unsigned read_end = audio_buffer_ptr + samples_per_frame;

	if (read_end > BUFFER_SIZE)
		read_end = BUFFER_SIZE;

	read_first  = read_end - audio_buffer_ptr;
	read_second = samples_per_frame - read_first;

	audio_batch_cb(audio_buffer + audio_buffer_ptr, read_first / (dma.samplebits / 8));
	audio_buffer_ptr += read_first;
	if (read_second >= 1) {
		audio_batch_cb(audio_buffer, read_second / (dma.samplebits / 8));
		audio_buffer_ptr = read_second;
	}
}

bool is_missionpack = false;

bool retro_load_game(const struct retro_game_info *info)
{
	enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
	if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
	{
		if (log_cb)
			log_cb(RETRO_LOG_INFO, "XRGB8888 is not supported.\n");
		return false;
	}

	hw_render.context_type    = RETRO_HW_CONTEXT_OPENGL;
	hw_render.context_reset   = context_reset;
	hw_render.context_destroy = context_destroy;
	hw_render.bottom_left_origin = true;
	hw_render.depth = true;
	hw_render.stencil = true;

	if (!environ_cb(RETRO_ENVIRONMENT_SET_HW_RENDER, &hw_render))
	{
		if (log_cb)
			log_cb(RETRO_LOG_ERROR, "vitaQuakeIII: libretro frontend doesn't have OpenGL support.\n");
		return false;
	}
	
	int i;
	char *path_lower;
#if defined(_WIN32)
	char slash = '\\';
#else
	char slash = '/';
#endif
	bool use_external_savedir = false;
	const char *base_save_dir = NULL;
	struct retro_keyboard_callback cb = { keyboard_cb };

	if (!info)
		return false;
	
	path_lower = strdup(info->path);
	
	for (i=0; path_lower[i]; ++i)
		path_lower[i] = tolower(path_lower[i]);
	
//	environ_cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &cb);
	
	update_variables(true);

	extract_directory(g_rom_dir, info->path, sizeof(g_rom_dir));
	
	snprintf(g_pak_path, sizeof(g_pak_path), "%s", info->path);
	
	if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &base_save_dir) && base_save_dir)
	{
		if (strlen(base_save_dir) > 0)
		{
			// Get game 'name' (i.e. subdirectory)
			char game_name[1024];
			extract_basename(game_name, g_rom_dir, sizeof(game_name));
			
			// > Build final save path
			snprintf(g_save_dir, sizeof(g_save_dir), "%s%c%s", base_save_dir, slash, game_name);
			use_external_savedir = true;
			
			// > Create save directory, if required
			if (!path_is_directory(g_save_dir))
			{
				use_external_savedir = path_mkdir(g_save_dir);
			}
		}
	}
	
	// > Error check
	if (!use_external_savedir)
	{
		// > Use ROM directory fallback...
		snprintf(g_save_dir, sizeof(g_save_dir), "%s", g_rom_dir);
	}
	else
	{
		// > Final check: is the save directory the same as the 'rom' directory?
		//   (i.e. ensure logical behaviour if user has set a bizarre save path...)
		use_external_savedir = (strcmp(g_save_dir, g_rom_dir) != 0);
	}
	

	extract_directory(g_rom_dir, g_rom_dir, sizeof(g_rom_dir));
	
	if (strstr(path_lower, "missionpack"))
		is_missionpack = true;
	
	Sys_SetBinaryPath(g_rom_dir);
    Sys_SetDefaultInstallPath(g_rom_dir);

	return true;
}

bool first_boot = true;

void retro_run(void)
{
	qglBindFramebuffer(RARCH_GL_FRAMEBUFFER, hw_render.get_current_framebuffer());
	qglEnable(GL_TEXTURE_2D);
	qglEnableClientState(GL_VERTEX_ARRAY);
	
	if (first_boot) {
		char commandLine[MAX_STRING_CHARS] = {0};
		
		// Starting input
		IN_Init(NULL);
		
		Sys_PlatformInit();

		// Set the initial time base
		Sys_Milliseconds();
	
		CON_Init();
		if (is_missionpack)
			sprintf(commandLine, "+set fs_game missionpack");
		Com_Init(commandLine);
		NET_Init();
		update_variables(false);
		first_boot = false;
	}
	
	bool updated = false;
	if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
		update_variables(false);
	
	Com_Frame();
	
	audio_process();
	audio_callback();
}

/* sys_*.c */

void log2file(const char *format, ...) {
	__gnuc_va_list arg;
	int done;
	va_start(arg, format);
	char msg[512];
	done = vsprintf(msg, format, arg);
	va_end(arg);
	int i;
	sprintf(msg, "%s\n", msg);
	CON_Print(msg);
}

#ifndef RELEASE
# define printf log2file
#endif

// Used to determine where to store user-specific files
static char homePath[MAX_OSPATH] = {0};

// Used to store the Steam Quake 3 installation path
static char steamPath[MAX_OSPATH] = {0};

// Used to store the GOG Quake 3 installation path
static char gogPath[MAX_OSPATH] = {0};

#ifndef DEFAULT_BASEDIR
#	define DEFAULT_BASEDIR Sys_BinaryPath()
#endif

/*
==================
Sys_DefaultHomePath
==================
*/
char *Sys_DefaultHomePath(void) {
    return homePath;
}

/*
================
Sys_SteamPath
================
*/
char *Sys_SteamPath(void) {
    return steamPath;
}

/*
================
Sys_GogPath
================
*/
char *Sys_GogPath(void) {
    // GOG also doesn't let you install Quake 3 on Mac/Linux
    return gogPath;
}

/*
================
Sys_Milliseconds
================
*/
int curtime;
int Sys_Milliseconds(void) {
    static uint64_t	base;

	uint64_t time = cpu_features_get_time_usec() / 1000;
	
    if (!base) {
		base = time;
    }

    curtime = (int)(time - base);

    return curtime;
}

/*
==================
Sys_RandomBytes
==================
*/
qboolean Sys_RandomBytes(byte *string, int len) {
    return qfalse;
}

/*
==================
Sys_GetCurrentUser
==================
*/
char nick[128];
char *Sys_GetCurrentUser(void) {
	sprintf(nick, "username");
    return nick;
}

/*
==================
Sys_LowPhysicalMemory

TODO
==================
*/
qboolean Sys_LowPhysicalMemory(void) {
    return qfalse;
}

/*
==================
Sys_Basename
==================
*/
const char *Sys_Basename(char *path) {
    char *p = strrchr(path, '/');
    return p ? p + 1 : (char *) path;
}

/*
==================
Sys_Dirname
==================
*/
const char *Sys_Dirname(char *path) {
    static const char dot[] = ".";
    char *last_slash;

    /* Find last '/'.  */
    last_slash = path != NULL ? strrchr(path, '/') : NULL;

    if (last_slash != NULL && last_slash != path && last_slash[1] == '\0') {
        /* Determine whether all remaining characters are slashes.  */
        char *runp;

        for (runp = last_slash; runp != path; --runp)
            if (runp[-1] != '/')
                break;

        /* The '/' is the last character, we have to look further.  */
        if (runp != path) {
            //last_slash = strrchr(path, '/', runp - path);ù
			printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		}
    }

    if (last_slash != NULL) {
        /* Determine whether all remaining characters are slashes.  */
        char *runp;

        for (runp = last_slash; runp != path; --runp)
            if (runp[-1] != '/')
                break;

        /* Terminate the path.  */
        if (runp == path) {
            /* The last slash is the first character in the string.  We have to
            return "/".  As a special case we have to return "//" if there
            are exactly two slashes at the beginning of the string.  See
            XBD 4.10 Path Name Resolution for more information.  */
            if (last_slash == path + 1)
                ++last_slash;
            else
                last_slash = path + 1;
        } else
            last_slash = runp;

        last_slash[0] = '\0';
    } else
        /* This assignment is ill-designed but the XPG specs require to
        return a string containing "." in any case no directory part is
        found and so a static and constant string is required.  */
        path = (char *) dot;

    return path;
}

/*
==============
Sys_FOpen
==============
*/
FILE *Sys_FOpen(const char *ospath, const char *mode) {
    return fopen(ospath, mode);
}

/*
==================
Sys_Mkdir
==================
*/
qboolean Sys_Mkdir(const char *path) {

    path_mkdir(path);
    return qtrue;
}

/*
==================
Sys_Mkfifo
==================
*/
FILE *Sys_Mkfifo(const char *ospath) {
    return NULL;
}

/*
==================
Sys_Cwd
==================
*/
char *Sys_Cwd(void) {
    return DEFAULT_BASEDIR;
}

/*
==============================================================

DIRECTORY SCANNING

==============================================================
*/

#define MAX_FOUND_FILES 0x1000

/*
==================
Sys_ListFilteredFiles
==================
*/
void Sys_ListFilteredFiles(const char *basedir, char *subdirs, char *filter, char **list, int *numfiles) {
    char search[MAX_OSPATH], newsubdirs[MAX_OSPATH];
    char filename[MAX_OSPATH];
    RDIR	*fdir;

    if (*numfiles >= MAX_FOUND_FILES - 1) {
        return;
    }

    if (strlen(subdirs)) {
        Com_sprintf(search, sizeof(search), "%s/%s", basedir, subdirs);
    } else {
        Com_sprintf(search, sizeof(search), "%s", basedir);
    }

    if ((fdir = retro_opendir(search)) == NULL) {
        return;
    }

    while ((retro_readdir(fdir)) > 0) {
        Com_sprintf(filename, sizeof(filename), "%s/%s", search, retro_dirent_get_name(fdir));

        if (retro_dirent_is_dir(fdir, NULL)) {
            if (Q_stricmp(retro_dirent_get_name(fdir), ".") && Q_stricmp(retro_dirent_get_name(fdir), "..")) {
                if (strlen(subdirs)) {
                    Com_sprintf(newsubdirs, sizeof(newsubdirs), "%s/%s", subdirs, retro_dirent_get_name(fdir));
                } else {
                    Com_sprintf(newsubdirs, sizeof(newsubdirs), "%s", retro_dirent_get_name(fdir));
                }
                Sys_ListFilteredFiles(basedir, newsubdirs, filter, list, numfiles);
            }
        }
        if (*numfiles >= MAX_FOUND_FILES - 1) {
            break;
        }
        Com_sprintf(filename, sizeof(filename), "%s/%s", subdirs, retro_dirent_get_name(fdir));
        if (!Com_FilterPath(filter, filename, qfalse))
            continue;
        list[*numfiles] = CopyString(filename);
        (*numfiles)++;
    }

    retro_closedir(fdir);
}

/*
==================
Sys_ListFiles
==================
*/
char pak_fname[MAX_OSPATH];
char search[MAX_OSPATH];
char *list[MAX_FOUND_FILES];
RDIR	*fdir;
char **Sys_ListFiles(const char *directory, const char *extension, char *filter, int *numfiles, qboolean wantsubs) {
    
    qboolean dironly = wantsubs;
   
    int nfiles;
    char **listCopy;
    int i;

    int extLen;

    if (filter) {

        nfiles = 0;
        Sys_ListFilteredFiles(directory, "", filter, list, &nfiles);

        list[nfiles] = NULL;
        *numfiles = nfiles;

        if (!nfiles)
            return NULL;

        listCopy = Z_Malloc((nfiles + 1) * sizeof(*listCopy));
        for (i = 0; i < nfiles; i++) {
            listCopy[i] = list[i];
        }
        listCopy[i] = NULL;

        return listCopy;
    }

    if (!extension)
        extension = "";

    if (extension[0] == '/' && extension[1] == 0) {
        extension = "";
        dironly = qtrue;
    }

    extLen = strlen(extension);

    // search
    nfiles = 0;

	printf("Scanning %s folder\n", directory);
	
    if ((fdir = retro_opendir(directory)) == NULL) {
		printf("Failed to open %s folder\n", directory);
        *numfiles = 0;
        return NULL;
    }

    while ((retro_readdir(fdir)) > 0) {
		sprintf(pak_fname, retro_dirent_get_name(fdir));
        Com_sprintf(search, sizeof(search), "%s/%s", directory, pak_fname);
		printf("Detected %s file which %s a directory\n", search, retro_dirent_is_dir(fdir, NULL) ? "is" : "isn't");
        if ((dironly && !(retro_dirent_is_dir(fdir, NULL))) ||
            (!dironly && (retro_dirent_is_dir(fdir, NULL))))
            continue;

        if (*extension) {
			printf("Checking if %s is of %s extension\n", pak_fname, extension);
            if (strstr(pak_fname, extension) == NULL) {
                continue; // didn't match
            }
        }

        if (nfiles == MAX_FOUND_FILES - 1)
            break;
		
		printf("%s is a good candidate\n", pak_fname);
        list[nfiles] = CopyString(pak_fname);
        nfiles++;
    }
	
	printf("Found %d files in %s folder\n", nfiles, directory);
	
    list[nfiles] = NULL;

    retro_closedir(fdir);

    // return a copy of the list
    *numfiles = nfiles;

    if (!nfiles) {
        return NULL;
    }

    listCopy = Z_Malloc((nfiles + 1) * sizeof(*listCopy));
    for (i = 0; i < nfiles; i++) {
        listCopy[i] = list[i];
    }
    listCopy[i] = NULL;

    return listCopy;
}

/*
==================
Sys_FreeFileList
==================
*/
void Sys_FreeFileList(char **list) {
    int i;

    if (!list) {
        return;
    }

    for (i = 0; list[i]; i++) {
        Z_Free(list[i]);
    }

    Z_Free(list);
}

/*
==================
Sys_Sleep

Block execution for msec or until input is received.
==================
*/
void Sys_Sleep(int msec) {
    if (msec == 0)
        return;
}

/*
==============
Sys_ErrorDialog

Display an error message
==============
*/
void Sys_ErrorDialog(const char *error) {
    char buffer[1024];
    unsigned int size;
    int f = -1;
    const char *homepath = Cvar_VariableString("fs_homepath");
    const char *gamedir = Cvar_VariableString("fs_game");
    const char *fileName = "crashlog.txt";
    char *dirpath = FS_BuildOSPath(homepath, gamedir, "");
    char *ospath = FS_BuildOSPath(homepath, gamedir, fileName);

    Sys_Print(va("%s\n", error));

    // Make sure the write path for the crashlog exists...

    if (!Sys_Mkdir(homepath)) {
        Com_Printf("ERROR: couldn't create path '%s' for crash log.\n", homepath);
        return;
    }

    if (!Sys_Mkdir(dirpath)) {
        Com_Printf("ERROR: couldn't create path '%s' for crash log.\n", dirpath);
        return;
    }
}

/*
==============
Sys_GLimpSafeInit

Unix specific "safe" GL implementation initialisation
==============
*/
void Sys_GLimpSafeInit(void) {
    // NOP
}

/*
==============
Sys_GLimpInit

Unix specific GL implementation initialisation
==============
*/
void Sys_GLimpInit(void) {
    // NOP
}

void Sys_SetFloatEnv(void) {
    // rounding toward nearest
    // fesetround(FE_TONEAREST);
    // TODO: PSP2 ?
}

/*
==============
Sys_PlatformInit

Unix specific initialisation
==============
*/
void Sys_PlatformInit(void) {
}

/*
==============
Sys_PlatformExit

Unix specific deinitialisation
==============
*/
void Sys_PlatformExit(void) {
}

/*
==============
Sys_SetEnv

set/unset environment variables (empty value removes it)
==============
*/

void Sys_SetEnv(const char *name, const char *value) {
}

/*
==============
Sys_PID
==============
*/
int Sys_PID(void) {
    return 0;
}

/*
==============
Sys_PIDIsRunning
==============
*/
qboolean Sys_PIDIsRunning(int pid) {
    return true;
}

/*
=================
Sys_DllExtension

Check if filename should be allowed to be loaded as a DLL.
=================
*/
qboolean Sys_DllExtension(const char *name) {
    const char *p;
    char c = 0;

    if (COM_CompareExtension(name, DLL_EXT)) {
        return qtrue;
    }

    // Check for format of filename.so.1.2.3
    p = strstr(name, DLL_EXT ".");

    if (p) {
        p += strlen(DLL_EXT);

        // Check if .so is only followed for periods and numbers.
        while (*p) {
            c = *p;

            if (!isdigit(c) && c != '.') {
                return qfalse;
            }

            p++;
        }

        // Don't allow filename to end in a period. file.so., file.so.0., etc
        if (c != '.') {
            return qtrue;
        }
    }

    return qfalse;
}

/* snd_dma.c */

qboolean snd_inited = qfalse;
uint8_t psp2_inited = 0;

/*
===============
SNDDMA_Init
===============
*/
qboolean SNDDMA_Init(void)
{
	if (psp2_inited) return qtrue;
	psp2_inited = 1;
	dma.samplebits = 16;
	dma.speed = SAMPLE_RATE;
	dma.channels = 2;
	dma.samples = BUFFER_SIZE;
	dma.fullsamples = dma.samples / dma.channels;
	dma.submission_chunk = 1;
	dma.buffer = audio_buffer;
	dma.isfloat = 0;
	
	snd_inited = qtrue;
	
	return qtrue;
}

/*
===============
SNDDMA_GetDMAPos
===============
*/
int SNDDMA_GetDMAPos(void)
{
	if (!snd_inited) return 0;
	
	return audio_buffer_ptr;
}

/*
===============
SNDDMA_Shutdown
===============
*/
void SNDDMA_Shutdown(void)
{
}

/*
===============
SNDDMA_Submit

Send sound to device if buffer isn't really the dma buffer
===============
*/
void SNDDMA_Submit(void)
{
}

/*
===============
SNDDMA_BeginPainting
===============
*/
void SNDDMA_BeginPainting (void)
{
}

void GLimp_Shutdown( void )
{
	ri.IN_Shutdown();
}

/*
===============
GLimp_Minimize

Minimize the game so that user is back at the desktop
===============
*/
void GLimp_Minimize( void )
{
}


/*
===============
GLimp_LogComment
===============
*/
void GLimp_LogComment( char *comment )
{
}

#define R_MODE_FALLBACK 3 // 960 * 544

/*
===============
GLimp_Init

This routine is responsible for initializing the OS specific portions
of OpenGL
===============
*/
uint8_t inited = 0;

typedef struct vidmode_s
{
	const char *description;
	int width, height;
	float pixelAspect;		// pixel width / height
} vidmode_t;
extern vidmode_t r_vidModes[];

uint32_t cur_width;

void GLimp_Init( qboolean coreContext)
{
	int i;
	if (r_mode->integer < 0) r_mode->integer = 3;
	
	glConfig.vidWidth = scr_width;
	glConfig.vidHeight = scr_height;
	glConfig.colorBits = 32;
	glConfig.depthBits = 32;
	glConfig.stencilBits = 8;
	glConfig.displayFrequency = framerate;
	glConfig.stereoEnabled = qfalse;
	
	glConfig.driverType = GLDRV_ICD;
	glConfig.hardwareType = GLHW_GENERIC;
	glConfig.deviceSupportsGamma = qfalse;
	glConfig.textureCompression = TC_NONE;
	glConfig.textureEnvAddAvailable = qfalse;
	glConfig.windowAspect = ((float)scr_width) / ((float)scr_height);
	glConfig.isFullscreen = qtrue;
	
	if (!inited){
		inited = 1;
		cur_width = glConfig.vidWidth;
	}else if (glConfig.vidWidth != cur_width){ // Changed resolution in game, restarting the game

	}
	indices = (uint16_t*)malloc(sizeof(uint16_t*)*MAX_INDICES);
	for (i=0;i<MAX_INDICES;i++){
		indices[i] = i;
	}
	qglEnableClientState(GL_VERTEX_ARRAY);
	gVertexBufferPtr = (float*)malloc(0x400000);
	gColorBufferPtr = (uint8_t*)malloc(0x200000);
	gTexCoordBufferPtr = (float*)malloc(0x200000);
	gColorBuffer255 = (uint8_t*)malloc(0x3000);
	memset(gColorBuffer255, 0xFF, 0x3000);
	gVertexBuffer = gVertexBufferPtr;
	gColorBuffer = gColorBufferPtr;
	gTexCoordBuffer = gTexCoordBufferPtr;
	
	strncpy(glConfig.vendor_string, "unknown", sizeof(glConfig.vendor_string));
	strncpy(glConfig.renderer_string, "unknown", sizeof(glConfig.renderer_string));
	strncpy(glConfig.version_string, "unknown", sizeof(glConfig.version_string));
	strncpy(glConfig.extensions_string, "unknown", sizeof(glConfig.extensions_string));
	
	qglClearColor( 0, 0, 0, 1 );
	qglClear( GL_COLOR_BUFFER_BIT );
	
}


/*
===============
GLimp_EndFrame

Responsible for doing a swapbuffers
===============
*/
void GLimp_EndFrame( void )
{
	video_cb(RETRO_HW_FRAME_BUFFER_VALID, scr_width, scr_height, 0);
	gVertexBuffer = gVertexBufferPtr;
	gColorBuffer = gColorBufferPtr;
	gTexCoordBuffer = gTexCoordBufferPtr;
}

/* input.c */

/*
===============
IN_Frame
===============
*/
uint32_t oldkeys, oldanalogs;
void Key_Event(int key, int value, int time){
	Com_QueueEvent(time, SE_KEY, key, value, 0, NULL);
}

int16_t old_ret;
void Sys_SetKeys(int time){
	int port;

	if (!poll_cb)
		return;

	poll_cb();

	if (!input_cb)
		return;

	for (port = 0; port < MAX_PADS; port++)
	{
		if (!input_cb)
			break;

		switch (quake_devices[port])
		{
		case RETRO_DEVICE_JOYPAD:
		case RETRO_DEVICE_JOYPAD_ALT:
		case RETRO_DEVICE_MODERN:
		{
			unsigned i;
			int16_t ret    = 0;
			if (libretro_supports_bitmasks)
				ret = input_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
			else
			{
				for (i=RETRO_DEVICE_ID_JOYPAD_B; i <= RETRO_DEVICE_ID_JOYPAD_R3; ++i)
				{
					if (input_cb(port, RETRO_DEVICE_JOYPAD, 0, i))
						ret |= (1 << i);
				}
			}

			if ((ret & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) && !(old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_UP)))
				Key_Event(K_UPARROW, 1, time);
			else if (!(ret & (1 << RETRO_DEVICE_ID_JOYPAD_UP)) && (old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_UP)))
				Key_Event(K_UPARROW, 0, time);
			if ((ret & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) && !(old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)))
				Key_Event(K_DOWNARROW, 1, time);
			else if (!(ret & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)) && (old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN)))
				Key_Event(K_DOWNARROW, 0, time);
			if ((ret & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) && !(old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)))
				Key_Event(K_LEFTARROW, 1, time);
			else if (!(ret & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)) && (old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT)))
				Key_Event(K_LEFTARROW, 0, time);
			if ((ret & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) && !(old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
				Key_Event(K_RIGHTARROW, 1, time);
			else if (!(ret & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)) && (old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT)))
				Key_Event(K_RIGHTARROW, 0, time);
			if ((ret & (1 << RETRO_DEVICE_ID_JOYPAD_START)) && !(old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_START)))
				Key_Event(K_ESCAPE, 1, time);
			else if (!(ret & (1 << RETRO_DEVICE_ID_JOYPAD_START)) && (old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_START)))
				Key_Event(K_ESCAPE, 0, time);
			if ((ret & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)) && !(old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)))
				Key_Event(K_ENTER, 1, time);
			else if (!(ret & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)) && (old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT)))
				Key_Event(K_ENTER, 0, time);
			if ((ret & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) && !(old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_Y)))
				Key_Event(K_AUX3, 1, time);
			else if (!(ret & (1 << RETRO_DEVICE_ID_JOYPAD_Y)) && (old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_Y)))
				Key_Event(K_AUX3, 0, time);
			if ((ret & (1 << RETRO_DEVICE_ID_JOYPAD_X)) && !(old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_X)))
				Key_Event(K_AUX4, 1, time);
			else if (!(ret & (1 << RETRO_DEVICE_ID_JOYPAD_X)) && (old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_X)))
				Key_Event(K_AUX4, 0, time);
			if ((ret & (1 << RETRO_DEVICE_ID_JOYPAD_B)) && !(old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_B)))
				Key_Event(K_AUX1, 1, time);
			else if (!(ret & (1 << RETRO_DEVICE_ID_JOYPAD_B)) && (old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_B)))
				Key_Event(K_AUX1, 0, time);
			if ((ret & (1 << RETRO_DEVICE_ID_JOYPAD_A)) && !(old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_A)))
				Key_Event(K_AUX2, 1, time);
			else if (!(ret & (1 << RETRO_DEVICE_ID_JOYPAD_A)) && (old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_A)))
				Key_Event(K_AUX2, 0, time);
			if ((ret & (1 << RETRO_DEVICE_ID_JOYPAD_L)) && !(old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
				Key_Event(K_AUX5, 1, time);
			else if (!(ret & (1 << RETRO_DEVICE_ID_JOYPAD_L)) && (old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_L)))
				Key_Event(K_AUX5, 0, time);
			if ((ret & (1 << RETRO_DEVICE_ID_JOYPAD_R)) && !(old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_R)))
				Key_Event(K_AUX6, 1, time);
			else if (!(ret & (1 << RETRO_DEVICE_ID_JOYPAD_R)) && (old_ret & (1 << RETRO_DEVICE_ID_JOYPAD_R)))
				Key_Event(K_AUX6, 0, time);
			
			old_ret = ret;
		}
		break;
		/*
		case RETRO_DEVICE_KEYBOARD:
			if (input_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT))
				Sys_SetKeys(K_MOUSE1, 1);
			else
				Sys_SetKeys(K_MOUSE1, 0);
			if (input_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT))
				Sys_SetKeys(K_MOUSE2, 1);
			else
				Sys_SetKeys(K_MOUSE2, 0);
			if (input_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_MIDDLE))
				Sys_SetKeys(K_MOUSE3, 1);
			else
				Sys_SetKeys(K_MOUSE3, 0);
			if (input_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_WHEELUP))
				Sys_SetKeys(K_MOUSE4, 1);
			else
				Sys_SetKeys(K_MOUSE4, 0);
			if (input_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_WHEELDOWN))
				Sys_SetKeys(K_MOUSE5, 1);
			else
				Sys_SetKeys(K_MOUSE5, 0);
			if (input_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_HORIZ_WHEELUP))
				Sys_SetKeys(K_MOUSE6, 1);
			else
				Sys_SetKeys(K_MOUSE6, 0);
			if (input_cb(port, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_HORIZ_WHEELDOWN))
				Sys_SetKeys(K_MOUSE7, 1);
			else
				Sys_SetKeys(K_MOUSE7, 0);
			if (quake_devices[0] == RETRO_DEVICE_KEYBOARD) {
				if (input_cb(port, RETRO_DEVICE_KEYBOARD, 0, RETROK_UP))
					Sys_SetKeys(K_UPARROW, 1);
				else
					Sys_SetKeys(K_UPARROW, 0);
				if (input_cb(port, RETRO_DEVICE_KEYBOARD, 0, RETROK_DOWN))
					Sys_SetKeys(K_DOWNARROW, 1);
				else
					Sys_SetKeys(K_DOWNARROW, 0);
				if (input_cb(port, RETRO_DEVICE_KEYBOARD, 0, RETROK_LEFT))
					Sys_SetKeys(K_LEFTARROW, 1);
				else
					Sys_SetKeys(K_LEFTARROW, 0);
				if (input_cb(port, RETRO_DEVICE_KEYBOARD, 0, RETROK_RIGHT))
					Sys_SetKeys(K_RIGHTARROW, 1);
				else
					Sys_SetKeys(K_RIGHTARROW, 0);
			}
			break;
		*/
		case RETRO_DEVICE_NONE:
			break;
		}
	}
}

// Left analog virtual values
#define LANALOG_LEFT  0x01
#define LANALOG_RIGHT 0x02
#define LANALOG_UP    0x04
#define LANALOG_DOWN  0x08

int old_x = - 1, old_y;

void IN_Frame( void )
{
	int time = Sys_Milliseconds();
	Sys_SetKeys(time);
	int lsx, lsy, rsx, rsy;
	
	uint32_t virt_buttons = 0x00;
	
	if (quake_devices[0] != RETRO_DEVICE_NONE && quake_devices[0] != RETRO_DEVICE_KEYBOARD) {
      // Emulating keys with left analog (TODO: Replace this dirty hack with a serious implementation)
      lsx = input_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,
               RETRO_DEVICE_ID_ANALOG_X);
      lsy = input_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT,
               RETRO_DEVICE_ID_ANALOG_Y);

      if (lsx > analog_deadzone || lsx < -analog_deadzone) {
         if (lsx > analog_deadzone)
            virt_buttons += LANALOG_RIGHT;
         if (lsx < -analog_deadzone)
            virt_buttons += LANALOG_LEFT;
	  }
	  
	  if (lsy > analog_deadzone || lsy < -analog_deadzone) {
         if (lsy > analog_deadzone)
            virt_buttons += LANALOG_UP;
         if (lsy < -analog_deadzone)
            virt_buttons += LANALOG_DOWN;
      }
	
	  if (virt_buttons != oldanalogs){
		if((virt_buttons & LANALOG_LEFT) != (oldanalogs & LANALOG_LEFT))
			Key_Event(K_AUX7, (virt_buttons & LANALOG_LEFT) == LANALOG_LEFT, time);
		if((virt_buttons & LANALOG_RIGHT) != (oldanalogs & LANALOG_RIGHT))
			Key_Event(K_AUX8, (virt_buttons & LANALOG_RIGHT) == LANALOG_RIGHT, time);
		if((virt_buttons & LANALOG_UP) != (oldanalogs & LANALOG_UP))
			Key_Event(K_AUX10, (virt_buttons & LANALOG_UP) == LANALOG_UP, time);
		if((virt_buttons & LANALOG_DOWN) != (oldanalogs & LANALOG_DOWN))
			Key_Event(K_AUX9, (virt_buttons & LANALOG_DOWN) == LANALOG_DOWN, time);
	  }
	  oldanalogs = virt_buttons;
	  
	  int slowdown = 1024 * (framerate / 60.0f);
	  
	  // Right stick Look
      rsx = input_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT,
               RETRO_DEVICE_ID_ANALOG_X);
      rsy = invert_y_axis * input_cb(0, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT,
               RETRO_DEVICE_ID_ANALOG_Y);
			   
	  if (rsx > analog_deadzone || rsx < -analog_deadzone) {
         if (rsx > analog_deadzone)
            rsx = rsx - analog_deadzone;
         if (rsx < -analog_deadzone)
            rsx = rsx + analog_deadzone;
		
      } else rsx = 0;
	  if (rsy > analog_deadzone || rsy < -analog_deadzone) {
         if (rsy > analog_deadzone)
            rsy = rsy - analog_deadzone;
         if (rsy < -analog_deadzone)
            rsy = rsy + analog_deadzone;
      } else rsy = 0;
	  Com_QueueEvent(time, SE_MOUSE, rsx / slowdown, rsy / slowdown, 0, NULL);
	}
	
}

/*
===============
IN_Init
===============
*/
void IN_Init( void *windowData )
{
}

/*
===============
IN_Shutdown
===============
*/
void IN_Shutdown( void )
{
}

/*
===============
IN_Restart
===============
*/
void IN_Restart( void )
{
}

/* net.c */
/*
===================
NET_CompareBaseAdrMask
Compare without port, and up to the bit number given in netmask.
===================
*/
qboolean NET_CompareBaseAdrMask(netadr_t a, netadr_t b, int netmask)
{
	byte cmpmask, *addra, *addrb;
	int curbyte;

	if (a.type != b.type)
		return qfalse;

	if (a.type == NA_LOOPBACK)
		return qtrue;

	if(a.type == NA_IP)
	{
		addra = (byte *) &a.ip;
		addrb = (byte *) &b.ip;

		if(netmask < 0 || netmask > 32)
			netmask = 32;
	}
	else
	{
		Com_Printf ("NET_CompareBaseAdr: bad address type\n");
		return qfalse;
	}

	curbyte = netmask >> 3;

	if(curbyte && memcmp(addra, addrb, curbyte))
		return qfalse;

	netmask &= 0x07;
	if(netmask)
	{
		cmpmask = (1 << netmask) - 1;
		cmpmask <<= 8 - netmask;

		if((addra[curbyte] & cmpmask) == (addrb[curbyte] & cmpmask))
			return qtrue;
	}
	else
		return qtrue;

	return qfalse;
}

qboolean NET_CompareBaseAdr (netadr_t a, netadr_t b)
{
	return NET_CompareBaseAdrMask(a, b, -1);
}

const char	*NET_AdrToString (netadr_t a)
{
	static	char	s[NET_ADDRSTRMAXLEN];

	if (a.type == NA_LOOPBACK)
		Com_sprintf (s, sizeof(s), "loopback");
	else if (a.type == NA_BOT)
		Com_sprintf (s, sizeof(s), "bot");
	else if (a.type == NA_IP)
	{
	}

	return s;
}

const char	*NET_AdrToStringwPort (netadr_t a)
{
	static	char	s[NET_ADDRSTRMAXLEN];

	if (a.type == NA_LOOPBACK)
		Com_sprintf (s, sizeof(s), "loopback");
	else if (a.type == NA_BOT)
		Com_sprintf (s, sizeof(s), "bot");
	else if(a.type == NA_IP)
		printf("Online not supported\n");

	return s;
}

static int networkingEnabled = 0;

void NET_Init( void ) {
	NET_Config( qtrue );

	Cmd_AddCommand ("net_restart", NET_Restart_f);
}

void NET_Shutdown( void ) {
	if ( !networkingEnabled ) {
		return;
	}

	NET_Config( qfalse );
}

#ifndef IF_NAMESIZE
#define IF_NAMESIZE 16
#endif

#define MAX_IPS 32
static int numIP;

typedef uint8_t sa_family_t;

#ifndef sockaddr_in
typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;

struct in_addr {
	in_addr_t s_addr;
};

typedef struct sockaddr_in {
	uint8_t			sin_len;
	sa_family_t		sin_family;
	in_port_t		sin_port;
	struct in_addr		sin_addr;
	in_port_t		sin_vport;
	char			sin_zero[6];
} sockaddr_in;
#endif

#ifndef sockaddr_storage
#define	_SS_MAXSIZE	128U
#define	_SS_ALIGNSIZE	(sizeof(int64_t))
#define	_SS_PAD1SIZE	(_SS_ALIGNSIZE - sizeof(unsigned char) - sizeof(sa_family_t))
#define	_SS_PAD2SIZE	(_SS_MAXSIZE - sizeof(unsigned char) - sizeof(sa_family_t) - \
				_SS_PAD1SIZE - _SS_ALIGNSIZE)
				
struct sockaddr_storage {
	unsigned char		ss_len;		/* address length */
	sa_family_t	ss_family;	/* address family */
	char		__ss_pad1[_SS_PAD1SIZE];
	int64_t		__ss_align;	/* force desired structure storage alignment */
	char		__ss_pad2[_SS_PAD2SIZE];
};
#endif

typedef struct
{
	char ifname[IF_NAMESIZE];

	netadrtype_t type;
	sa_family_t family;
	struct sockaddr_storage addr;
	struct sockaddr_storage netmask;
} nip_localaddr_t;

static nip_localaddr_t localIP[MAX_IPS];

static cvar_t	*net_enabled;

static cvar_t	*net_socksEnabled;
static cvar_t	*net_socksServer;
static cvar_t	*net_socksPort;
static cvar_t	*net_socksUsername;
static cvar_t	*net_socksPassword;

static cvar_t	*net_ip;
static cvar_t	*net_port;

static cvar_t	*net_dropsim;

typedef int SOCKET;
#	define INVALID_SOCKET		-1
#	define SOCKET_ERROR			-1
#	define closesocket			close
#	define ioctlsocket			ioctl
typedef int	ioctlarg_t;
#	define socketError			errno

static SOCKET	ip_socket = INVALID_SOCKET;
static SOCKET	socks_socket = INVALID_SOCKET;

/*
====================
NET_GetCvars
====================
*/
static qboolean NET_GetCvars( void ) {
	int modified;

#ifdef DEDICATED
	// I want server owners to explicitly turn on ipv6 support.
	net_enabled = Cvar_Get( "net_enabled", "1", CVAR_LATCH | CVAR_ARCHIVE );
#else
	/* End users have it enabled so they can connect to ipv6-only hosts, but ipv4 will be
	 * used if available due to ping */
	net_enabled = Cvar_Get( "net_enabled", "3", CVAR_LATCH | CVAR_ARCHIVE );
#endif
	modified = net_enabled->modified;
	net_enabled->modified = qfalse;

	net_ip = Cvar_Get( "net_ip", "0.0.0.0", CVAR_LATCH );
	modified += net_ip->modified;
	net_ip->modified = qfalse;

	net_port = Cvar_Get( "net_port", va( "%i", PORT_SERVER ), CVAR_LATCH );
	modified += net_port->modified;
	net_port->modified = qfalse;

	net_socksEnabled = Cvar_Get( "net_socksEnabled", "0", CVAR_LATCH | CVAR_ARCHIVE );
	modified += net_socksEnabled->modified;
	net_socksEnabled->modified = qfalse;

	net_socksServer = Cvar_Get( "net_socksServer", "", CVAR_LATCH | CVAR_ARCHIVE );
	modified += net_socksServer->modified;
	net_socksServer->modified = qfalse;

	net_socksPort = Cvar_Get( "net_socksPort", "1080", CVAR_LATCH | CVAR_ARCHIVE );
	modified += net_socksPort->modified;
	net_socksPort->modified = qfalse;

	net_socksUsername = Cvar_Get( "net_socksUsername", "", CVAR_LATCH | CVAR_ARCHIVE );
	modified += net_socksUsername->modified;
	net_socksUsername->modified = qfalse;

	net_socksPassword = Cvar_Get( "net_socksPassword", "", CVAR_LATCH | CVAR_ARCHIVE );
	modified += net_socksPassword->modified;
	net_socksPassword->modified = qfalse;

	net_dropsim = Cvar_Get("net_dropsim", "", CVAR_TEMP);

	return modified ? qtrue : qfalse;
}

/*
====================
NET_Config
====================
*/
void NET_Config( qboolean enableNetworking ) {
	qboolean	modified;
	qboolean	stop;
	qboolean	start;

	// get any latched changes to cvars
	modified = NET_GetCvars();

	if( !net_enabled->integer ) {
		enableNetworking = 0;
	}

	// if enable state is the same and no cvars were modified, we have nothing to do
	if( enableNetworking == networkingEnabled && !modified ) {
		return;
	}

	if( enableNetworking == networkingEnabled ) {
		if( enableNetworking ) {
			stop = qtrue;
			start = qtrue;
		}
		else {
			stop = qfalse;
			start = qfalse;
		}
	}
	else {
		if( enableNetworking ) {
			stop = qfalse;
			start = qtrue;
		}
		else {
			stop = qtrue;
			start = qfalse;
		}
		networkingEnabled = enableNetworking;
	}

	if( stop ) {
		if ( ip_socket != INVALID_SOCKET ) {
			closesocket( ip_socket );
			ip_socket = INVALID_SOCKET;
		}

		if ( socks_socket != INVALID_SOCKET ) {
			closesocket( socks_socket );
			socks_socket = INVALID_SOCKET;
		}

	}

	if( start )
	{
		if (net_enabled->integer)
		{
			NET_OpenIP();
		}
	}
}

static void NET_GetLocalAddress( void ) {
	
}

void NET_Sleep(int msec)
{
}

qboolean Sys_StringToAdr( const char *s, netadr_t *a, netadrtype_t family ) {
	return qtrue;
}

void Sys_SendPacket( int length, const void *data, netadr_t to ) {
}

/*
==================
Sys_ShowIP
==================
*/
void Sys_ShowIP(void) {
}

qboolean	NET_CompareAdr (netadr_t a, netadr_t b)
{
	if(!NET_CompareBaseAdr(a, b))
		return qfalse;

	if (a.type == NA_IP)
	{
		if (a.port == b.port)
			return qtrue;
	}
	else
		return qtrue;

	return qfalse;
}

qboolean	NET_IsLocalAddress( netadr_t adr ) {
	return adr.type == NA_LOOPBACK;
}

void NET_JoinMulticast6(void) {}
void NET_LeaveMulticast6() {}

/*
====================
NET_OpenIP
====================
*/
void NET_OpenIP( void ) {
	int		i;
	int		err;
	int		port;
	int		port6;

	port = net_port->integer;

	NET_GetLocalAddress();

	// automatically scan for a valid port, so multiple
	// dedicated servers can be started without requiring
	// a different net_port for each one

	if(net_enabled->integer & NET_ENABLEV4)
	{
		for( i = 0 ; i < 10 ; i++ ) {
			//ip_socket = NET_IPSocket( net_ip->string, port + i, &err );
			if (ip_socket != INVALID_SOCKET) {
				Cvar_SetValue( "net_port", port + i );

				//if (net_socksEnabled->integer)
				//	NET_OpenSocks( port + i );

				break;
			}
			else
			{
				//if(err == SCE_NET_EAFNOSUPPORT)
					break;
			}
		}

		if(ip_socket == INVALID_SOCKET)
			Com_Printf( "WARNING: Couldn't bind to a v4 ip address.\n");
	}
}

/*
====================
NET_Restart_f
====================
*/
void NET_Restart_f(void)
{
	NET_Config(qtrue);
}

/*
==================
Sys_IsLANAddress
LAN clients will have their rate var ignored
==================
*/
qboolean Sys_IsLANAddress( netadr_t adr ) {
	int		index, run, addrsize;
	qboolean differed;
	byte *compareadr, *comparemask, *compareip;

	if( adr.type == NA_LOOPBACK ) {
		return qtrue;
	}

	if( adr.type == NA_IP )
	{
		// RFC1918:
		// 10.0.0.0        -   10.255.255.255  (10/8 prefix)
		// 172.16.0.0      -   172.31.255.255  (172.16/12 prefix)
		// 192.168.0.0     -   192.168.255.255 (192.168/16 prefix)
		if(adr.ip[0] == 10)
			return qtrue;
		if(adr.ip[0] == 172 && (adr.ip[1]&0xf0) == 16)
			return qtrue;
		if(adr.ip[0] == 192 && adr.ip[1] == 168)
			return qtrue;

		if(adr.ip[0] == 127)
			return qtrue;
	}

	// Now compare against the networks this computer is member of.
	for(index = 0; index < numIP; index++)
	{
		if(localIP[index].type == adr.type)
		{
			if(adr.type == NA_IP)
			{
				compareip = (byte *) &((struct sockaddr_in *) &localIP[index].addr)->sin_addr.s_addr;
				comparemask = (byte *) &((struct sockaddr_in *) &localIP[index].netmask)->sin_addr.s_addr;
				compareadr = adr.ip;

				addrsize = sizeof(adr.ip);
			}

			differed = qfalse;
			for(run = 0; run < addrsize; run++)
			{
				if((compareip[run] & comparemask[run]) != (compareadr[run] & comparemask[run]))
				{
					differed = qtrue;
					break;
				}
			}

			if(!differed)
				return qtrue;

		}
	}

	return qfalse;
}
