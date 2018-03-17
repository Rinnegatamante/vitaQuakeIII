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

#include <vitasdk.h>
#include "vitaGL.h"

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
float *gColorBuffer;
float *gTexCoordBuffer;
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
	glConfig.textureCompression = TC_NONE;
	glConfig.textureEnvAddAvailable = qfalse;
	glConfig.windowAspect = ((float)r_vidModes[r_mode->integer].width) / ((float)r_vidModes[r_mode->integer].height);
	glConfig.isFullscreen = qtrue;
	
	if (!inited){
		vglInitExtended(0x800000, glConfig.vidWidth, glConfig.vidHeight);
		vglUseVram(GL_TRUE);
		inited = 1;
		cur_width = glConfig.vidWidth;
	}else if (glConfig.vidWidth != cur_width){ // Changed resolution in game, restarting the game
		sceAppMgrLoadExec("app0:/eboot.bin", NULL, NULL);
	}
	vglStartRendering();
	int i;
	indices = (uint16_t*)malloc(sizeof(uint16_t*)*MAX_INDICES);
	for (i=0;i<MAX_INDICES;i++){
		indices[i] = i;
	}
	vglIndexPointer(GL_SHORT, 0, MAX_INDICES, indices);
	glEnableClientState(GL_VERTEX_ARRAY);
	gVertexBuffer = (float*)malloc(sizeof(float)*VERTEXARRAYSIZE);
	gColorBuffer = (float*)malloc(sizeof(float)*VERTEXARRAYSIZE);
	gTexCoordBuffer = (float*)malloc(sizeof(float)*VERTEXARRAYSIZE);
	
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
void GLimp_EndFrame( void )
{
	vglStopRendering();
	vglStartRendering();
	vglIndexPointer(GL_SHORT, 0, MAX_INDICES, indices);
}
