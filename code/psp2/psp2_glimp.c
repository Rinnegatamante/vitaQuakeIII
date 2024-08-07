/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Quake III Arena source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../renderercommon/tr_common.h"
#include "../sys/sys_local.h"
#include "../client/keycodes.h"

#include <vitasdk.h>
#include <vitaGL.h>

/*
===============
GLimp_Shutdown
===============
*/
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
extern void log2file(const char *format, ...);
void GLimp_LogComment( char *comment )
{
#ifndef RELEASE
	log2file(comment);
#endif
}

#define R_MODE_FALLBACK 3 // 640 * 480

/*
===============
GLimp_Init

This routine is responsible for initializing the OS specific portions
of OpenGL
===============
*/
uint16_t* indices;
float *gVertexBuffer;
uint8_t *gColorBuffer;
float *gTexCoordBuffer;
float *gVertexBufferPtr;
uint8_t *gColorBufferPtr;
float *gTexCoordBufferPtr;
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
	
	if (r_mode->integer < 0) r_mode->integer = 3;
	
	glConfig.vidWidth = r_vidModes[r_mode->integer].width;
	glConfig.vidHeight = r_vidModes[r_mode->integer].height;
	glConfig.colorBits = 32;
	glConfig.depthBits = 32;
	glConfig.stencilBits = 8;
	glConfig.displayFrequency = 60;
	glConfig.stereoEnabled = qfalse;
	
	glConfig.driverType = GLDRV_ICD;
	glConfig.hardwareType = GLHW_GENERIC;
	glConfig.deviceSupportsGamma = qfalse;
	glConfig.textureCompression = TC_S3TC;
	glConfig.textureEnvAddAvailable = qtrue;
	glConfig.windowAspect = ((float)r_vidModes[r_mode->integer].width) / ((float)r_vidModes[r_mode->integer].height);
	glConfig.isFullscreen = qtrue;
	
	if (!inited){
#ifdef URBANTERROR
		vglUseExtraMem(GL_FALSE);
		vglInitExtended(0, glConfig.vidWidth, glConfig.vidHeight, 0x1000000, SCE_GXM_MULTISAMPLE_NONE);
#else
		vglInitExtended(0, glConfig.vidWidth, glConfig.vidHeight, 0x1800000, SCE_GXM_MULTISAMPLE_4X);
#endif
		vglUseVram(GL_TRUE);

		inited = 1;
		cur_width = glConfig.vidWidth;
	}else if (glConfig.vidWidth != cur_width){ // Changed resolution in game, restarting the game
#ifdef URBANTERROR
		sceAppMgrLoadExec("app0:/urbanterror.bin", NULL, NULL);
#elif defined(OPENARENA)
		sceAppMgrLoadExec("app0:/openarena.bin", NULL, NULL);
#else
		sceAppMgrLoadExec("app0:/eboot.bin", NULL, NULL);
#endif
	}
	int i;
	indices = (uint16_t*)malloc(sizeof(uint16_t)*MAX_INDICES);
	for (i=0;i<MAX_INDICES;i++){
		indices[i] = i;
	}
	vglIndexPointerMapped(indices);
	glEnableClientState(GL_VERTEX_ARRAY);
	gVertexBufferPtr = (float*)malloc(0x100000);
	gColorBufferPtr = (uint8_t*)malloc(0x100000);
	gTexCoordBufferPtr = (float*)malloc(0x100000);
	gVertexBuffer = gVertexBufferPtr;
	gColorBuffer = gColorBufferPtr;
	gTexCoordBuffer = gTexCoordBufferPtr;
	
	strncpy(glConfig.vendor_string, glGetString(GL_VENDOR), sizeof(glConfig.vendor_string));
	strncpy(glConfig.renderer_string, glGetString(GL_RENDERER), sizeof(glConfig.renderer_string));
	strncpy(glConfig.version_string, glGetString(GL_VERSION), sizeof(glConfig.version_string));
	strncpy(glConfig.extensions_string, glGetString(GL_EXTENSIONS), sizeof(glConfig.extensions_string));
	
	qglClearColor( 0, 0, 0, 1 );
	qglClear( GL_COLOR_BUFFER_BIT );
	
}


/*
===============
GLimp_EndFrame

Responsible for doing a swapbuffers
===============
*/
extern uint8_t is_ime_up;

static uint16_t title[SCE_IME_DIALOG_MAX_TITLE_LENGTH];
static uint16_t initial_text[SCE_IME_DIALOG_MAX_TEXT_LENGTH];
static uint16_t input_text[SCE_IME_DIALOG_MAX_TEXT_LENGTH + 1];
char title_keyboard[256];

void ascii2utf(uint16_t* dst, char* src){
	if(!src || !dst)return;
	while(*src)*(dst++)=(*src++);
	*dst=0x00;
}

void utf2ascii(char* dst, uint16_t* src){
	if(!src || !dst)return;
	while(*src)*(dst++)=(*(src++))&0xFF;
	*dst=0x00;
}

void GLimp_EndFrame( void )
{
	switch (is_ime_up) {
	case 1: // New IME request
		{
			memset(input_text, 0, (SCE_IME_DIALOG_MAX_TEXT_LENGTH + 1) << 1);
			memset(initial_text, 0, (SCE_IME_DIALOG_MAX_TEXT_LENGTH) << 1);
			sprintf(title_keyboard, "Keyboard Input");
			ascii2utf(title, title_keyboard);
			SceImeDialogParam param;
			sceImeDialogParamInit(&param);
			param.supportedLanguages = 0x0001FFFF;
			param.languagesForced = SCE_TRUE;
			param.type = SCE_IME_TYPE_BASIC_LATIN;
			param.title = title;
			param.maxTextLength = SCE_IME_DIALOG_MAX_TEXT_LENGTH;
			param.initialText = initial_text;
			param.inputTextBuffer = input_text;
			sceImeDialogInit(&param);
			is_ime_up = 2;
			vglSwapBuffers(GL_TRUE);
		}
		break;
	case 2: // IME running
		{
			SceCommonDialogStatus status = sceImeDialogGetStatus();
			if (status == SCE_COMMON_DIALOG_STATUS_FINISHED) {
				SceImeDialogResult result;
				memset(&result, 0, sizeof(SceImeDialogResult));
				sceImeDialogGetResult(&result);
				if (result.button == SCE_IME_DIALOG_BUTTON_ENTER) {
					utf2ascii(title_keyboard, input_text);
				} else {
					title_keyboard[0] = 0;
				}
				sceImeDialogTerm();
				is_ime_up = 0;
			}
			vglSwapBuffers(GL_TRUE);
		}
		break;
	default:
		vglSwapBuffers(GL_FALSE);
		break;
	}
	vglIndexPointerMapped(indices);
	gVertexBuffer = gVertexBufferPtr;
	gColorBuffer = gColorBufferPtr;
	gTexCoordBuffer = gTexCoordBufferPtr;
}

/*
===========================================================
SMP acceleration
===========================================================
*/

SceUID renderCommandsEvent, renderCompletedEvent, renderActiveEvent;
void ( *glimpRenderThread )( void );

static int GLimp_RenderThreadWrapper(unsigned int args, void* arg) {
	glimpRenderThread();
	return sceKernelExitDeleteThread(0);
}


/*
=======================
GLimp_SpawnRenderThread
=======================
*/
SceUID renderThreadHandle;
qboolean GLimp_SpawnRenderThread( void ( *function )( void ) ) {
	
	
	renderCommandsEvent = sceKernelCreateSema("renderCommandsEvent", 0, 0, 1, NULL);
	renderCompletedEvent = sceKernelCreateSema("renderCompletedEvent", 0, 0, 1, NULL);
	renderActiveEvent = sceKernelCreateSema("renderActiveEvent", 0, 0, 1, NULL);
	
	glimpRenderThread = function;
	
	renderThreadHandle = sceKernelCreateThread("Renderer Thread", &GLimp_RenderThreadWrapper, 0x10000100, 0x200000, 0, 0, NULL);
	
	if (sceKernelStartThread(renderThreadHandle, 0, NULL)) {
		return qfalse;
	}

	return qtrue;
}

static volatile void *smpData = NULL;

void *GLimp_RendererSleep( void ) {
	//printf("entering RendererSleep\n");
	void  *data;
	
	int dummy;

	// after this, the front end can exit GLimp_FrontEndSleep
	//sceKernelSignalSema(renderCompletedEvent, 1);
	sceKernelWaitSema(renderCommandsEvent, 1, NULL);
	
	data = smpData;
	
	sceKernelSignalSema(renderActiveEvent, 1);

	return data;
}


void GLimp_FrontEndSleep( void ) {
	//printf("entering FrontEndSleep\n");
	//sceKernelWaitSema(renderCompletedEvent, 1, NULL);
}


void GLimp_WakeRenderer( void *data ) {
	//printf("entering WakeRenderer\n");
	smpData = data;

	sceKernelSignalSema(renderCommandsEvent, 1);
	sceKernelWaitSema(renderActiveEvent, 1, NULL);
}