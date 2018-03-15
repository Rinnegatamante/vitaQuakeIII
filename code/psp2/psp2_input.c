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
#include <vitasdk.h>

#include "../client/client.h"
#include "../sys/sys_local.h"

/*
===============
IN_Frame
===============
*/
uint32_t oldkeys;
void Key_Event(int key, int value, int time){
	Com_QueueEvent(time, SE_KEY, key, value, 0, NULL);
}

void Sys_SetKeys(uint32_t keys){
	if((keys & SCE_CTRL_START) != (oldkeys & SCE_CTRL_START))
		Key_Event(K_ESCAPE, (keys & SCE_CTRL_START) == SCE_CTRL_START, Sys_Milliseconds());
	if((keys & SCE_CTRL_SELECT) != (oldkeys & SCE_CTRL_SELECT))
		Key_Event(K_ENTER, (keys & SCE_CTRL_SELECT) == SCE_CTRL_SELECT, Sys_Milliseconds());
	if((keys & SCE_CTRL_UP) != (oldkeys & SCE_CTRL_UP))
		Key_Event(K_UPARROW, (keys & SCE_CTRL_UP) == SCE_CTRL_UP, Sys_Milliseconds());
	if((keys & SCE_CTRL_DOWN) != (oldkeys & SCE_CTRL_DOWN))
		Key_Event(K_DOWNARROW, (keys & SCE_CTRL_DOWN) == SCE_CTRL_DOWN, Sys_Milliseconds());
	if((keys & SCE_CTRL_LEFT) != (oldkeys & SCE_CTRL_LEFT))
		Key_Event(K_LEFTARROW, (keys & SCE_CTRL_LEFT) == SCE_CTRL_LEFT, Sys_Milliseconds());
	if((keys & SCE_CTRL_RIGHT) != (oldkeys & SCE_CTRL_RIGHT))
		Key_Event(K_RIGHTARROW, (keys & SCE_CTRL_RIGHT) == SCE_CTRL_RIGHT, Sys_Milliseconds());
	if((keys & SCE_CTRL_TRIANGLE) != (oldkeys & SCE_CTRL_TRIANGLE))
		Key_Event(K_AUX4, (keys & SCE_CTRL_TRIANGLE) == SCE_CTRL_TRIANGLE, Sys_Milliseconds());
	if((keys & SCE_CTRL_SQUARE) != (oldkeys & SCE_CTRL_SQUARE))
		Key_Event(K_AUX3, (keys & SCE_CTRL_SQUARE) == SCE_CTRL_SQUARE, Sys_Milliseconds());
	if((keys & SCE_CTRL_CIRCLE) != (oldkeys & SCE_CTRL_CIRCLE))
		Key_Event(K_AUX2, (keys & SCE_CTRL_CIRCLE) == SCE_CTRL_CIRCLE, Sys_Milliseconds());
	if((keys & SCE_CTRL_CROSS) != (oldkeys & SCE_CTRL_CROSS))
		Key_Event(K_AUX1, (keys & SCE_CTRL_CROSS) == SCE_CTRL_CROSS, Sys_Milliseconds());
	if((keys & SCE_CTRL_LTRIGGER) != (oldkeys & SCE_CTRL_LTRIGGER))
		Key_Event(K_AUX5, (keys & SCE_CTRL_LTRIGGER) == SCE_CTRL_LTRIGGER, Sys_Milliseconds());
	if((keys & SCE_CTRL_RTRIGGER) != (oldkeys & SCE_CTRL_RTRIGGER))
		Key_Event(K_AUX7, (keys & SCE_CTRL_RTRIGGER) == SCE_CTRL_RTRIGGER, Sys_Milliseconds());
}

void IN_RescaleAnalog(int *x, int *y, int dead) {

	float analogX = (float) *x;
	float analogY = (float) *y;
	float deadZone = (float) dead;
	float maximum = 128.0f;
	float magnitude = sqrt(analogX * analogX + analogY * analogY);
	if (magnitude >= deadZone)
	{
		float scalingFactor = maximum / magnitude * (magnitude - deadZone) / (maximum - deadZone);
		*x = (int) (analogX * scalingFactor);
		*y = (int) (analogY * scalingFactor);
	} else {
		*x = 0;
		*y = 0;
	}
}

void IN_Frame( void )
{
	SceCtrlData keys;
	sceCtrlPeekBufferPositive(0, &keys, 1);
	if(keys.buttons != oldkeys)
		Sys_SetKeys(keys.buttons);
	oldkeys = keys.buttons;
	
	// Emulating mouse with right analog
	int right_x = keys.rx - 127;
	int right_y = keys.ry - 127;
	IN_RescaleAnalog(&right_x, &right_y, 30);
	if (right_x != 0 || right_y != 0)
		Com_QueueEvent(0, SE_MOUSE, right_x, right_y, 0, NULL);
	
}

/*
===============
IN_Init
===============
*/
void IN_Init( void *windowData )
{
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
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
