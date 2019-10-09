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

static int hires_x, hires_y;
/*
===============
IN_Frame
===============
*/
uint32_t oldkeys, oldanalogs;
void Key_Event(int key, int value, int time){
	Com_QueueEvent(time, SE_KEY, key, value, 0, NULL);
}

void Sys_SetKeys(uint32_t keys, int time){
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

			if (ret & (1 << RETRO_DEVICE_ID_JOYPAD_UP))
				Key_Event(K_UPARROW, 1, time);
			else
				Key_Event(K_UPARROW, 0);
			if (ret & (1 << RETRO_DEVICE_ID_JOYPAD_DOWN))
				Key_Event(K_DOWNARROW, 1, time);
			else
				Key_Event(K_DOWNARROW, 0, time);
			if (ret & (1 << RETRO_DEVICE_ID_JOYPAD_LEFT))
				Key_Event(K_LEFTARROW, 1, time);
			else
				Key_Event(K_LEFTARROW, 0, time);
			if (ret & (1 << RETRO_DEVICE_ID_JOYPAD_RIGHT))
				Key_Event(K_RIGHTARROW, 1), time);
			else
				Key_Event(K_RIGHTARROW, 0), time);
			if (ret & (1 << RETRO_DEVICE_ID_JOYPAD_START))
				Key_Event(K_ESCAPE, 1, time);
			else
				Key_Event(K_ESCAPE, 0, time);
			if (ret & (1 << RETRO_DEVICE_ID_JOYPAD_SELECT))
				Key_Event(K_ENTER, 1, time);
			else
				Key_Event(K_ENTER, 0, time);
			if (ret & (1 << RETRO_DEVICE_ID_JOYPAD_Y))
				Key_Event(K_AUX3, 1);
			else
				Key_Event(K_AUX3, 0, time);
			if (ret & (1 << RETRO_DEVICE_ID_JOYPAD_X))
				Key_Event(K_AUX4, 1, time);
			else
				Key_Event(K_AUX4, 0, time);
			if (ret & (1 << RETRO_DEVICE_ID_JOYPAD_B))
			{
				Key_Event(K_AUX1, 1, time);
			}
			else
			{
				Key_Event(K_AUX1, 0, time);
			}
			if (ret & (1 << RETRO_DEVICE_ID_JOYPAD_A))
				Key_Event(K_AUX2, 1, time);
			else
				Key_Event(K_AUX2, 0, time);
			if (ret & (1 << RETRO_DEVICE_ID_JOYPAD_L))
				Key_Event(K_AUX5, 1);
			else
				Key_Event(K_AUX5, 0, time);
			if (ret & (1 << RETRO_DEVICE_ID_JOYPAD_R))
				Key_Event(K_AUX6, 1, time);
			else
				Key_Event(K_AUX6, 0, time);
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
	SceCtrlData keys;
	sceCtrlPeekBufferPositive(0, &keys, 1);
	int time = Sys_Milliseconds();
	if(keys.buttons != oldkeys)
		Sys_SetKeys(keys.buttons, time);
	oldkeys = keys.buttons;
	
	// Emulating mouse with touch
	SceTouchData touch;
	sceTouchPeek(SCE_TOUCH_PORT_FRONT, &touch, 1);
	if (touch.reportNum > 0){
		if (old_x != -1) Com_QueueEvent(time, SE_MOUSE, (touch.report[0].x - old_x), (touch.report[0].y - old_y), 0, NULL);
		old_x = touch.report[0].x;
		old_y = touch.report[0].y;
	}else old_x = -1;
	
	// Emulating mouse with right analog
	int right_x = (keys.rx - 127) * 256;
	int right_y = (keys.ry - 127) * 256;
	IN_RescaleAnalog(&right_x, &right_y, 7680);
	hires_x += right_x;
	hires_y += right_y;
	if (hires_x != 0 || hires_y != 0) {
		// increase slowdown variable to slow down aiming, could be made user-adjustable
		int slowdown = 1024;
		Com_QueueEvent(time, SE_MOUSE, hires_x / slowdown, hires_y / slowdown, 0, NULL);
		hires_x %= slowdown;
		hires_y %= slowdown;
	}
	
	// Emulating keys with left analog (TODO: Replace this dirty hack with a serious implementation)
	uint32_t virt_buttons = 0x00;
	if (keys.lx < 80) virt_buttons += LANALOG_LEFT;
	else if (keys.lx > 160) virt_buttons += LANALOG_RIGHT;
	if (keys.ly < 80) virt_buttons += LANALOG_UP;
	else if (keys.ly > 160) virt_buttons += LANALOG_DOWN;
	if (virt_buttons != oldanalogs){
		if((virt_buttons & LANALOG_LEFT) != (oldanalogs & LANALOG_LEFT))
			Key_Event(K_AUX7, (virt_buttons & LANALOG_LEFT) == LANALOG_LEFT, time);
		if((virt_buttons & LANALOG_RIGHT) != (oldanalogs & LANALOG_RIGHT))
			Key_Event(K_AUX8, (virt_buttons & LANALOG_RIGHT) == LANALOG_RIGHT, time);
		if((virt_buttons & LANALOG_UP) != (oldanalogs & LANALOG_UP))
			Key_Event(K_AUX9, (virt_buttons & LANALOG_UP) == LANALOG_UP, time);
		if((virt_buttons & LANALOG_DOWN) != (oldanalogs & LANALOG_DOWN))
			Key_Event(K_AUX10, (virt_buttons & LANALOG_DOWN) == LANALOG_DOWN, time);
	}
	oldanalogs = virt_buttons;
	
}

/*
===============
IN_Init
===============
*/
void IN_Init( void *windowData )
{
	sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
	sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
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
