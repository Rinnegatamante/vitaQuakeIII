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

#include <stdlib.h>
#include <stdio.h>
#include <vitasdk.h>

#include "../qcommon/q_shared.h"
#include "../client/snd_local.h"

qboolean snd_inited = qfalse;

#define SAMPLE_RATE   48000
#define AUDIOSIZE 16384

SceRtcTick initial_tick;
float tickRate;
int chn = -1;
qboolean stop_audio = qfalse;
uint8_t *audiobuffer;

static int audio_thread(int args, void *argp)
{
	chn = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_MAIN, AUDIOSIZE / 2, SAMPLE_RATE, SCE_AUDIO_OUT_MODE_MONO);
	sceAudioOutSetConfig(chn, -1, -1, -1);
	int vol[] = {32767, 32767};
	sceAudioOutSetVolume(chn, SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH, vol);
	
	while (!stop_audio)
	{
		sceAudioOutOutput(chn, audiobuffer);
	}
	 
	sceAudioOutReleasePort(chn);
	free(audiobuffer);

	sceKernelExitDeleteThread(0);
	return 0;
}

/*
===============
SNDDMA_Init
===============
*/
qboolean SNDDMA_Init(void)
{
	Com_Printf("Initializing audio device.\n");
	dma.samplebits = 16;
	dma.speed = SAMPLE_RATE;
	dma.channels = 1;
	dma.samples = AUDIOSIZE / 2;
	dma.fullsamples = dma.samples / dma.channels;
	dma.submission_chunk = 1;
	dma.buffer = audiobuffer = malloc(AUDIOSIZE);
	dma.isfloat = 0;
	
	tickRate = 1.0f / sceRtcGetTickResolution();
	
	SceUID audiothread = sceKernelCreateThread("Audio Thread", (void*)&audio_thread, 0x10000100, 0x10000, 0, 0, NULL);
	int res = sceKernelStartThread(audiothread, sizeof(audiothread), &audiothread);
	if (res != 0){
		Com_Printf("Failed to init audio thread (0x%x)\n", res);
		return qfalse;
	}

	sceRtcGetCurrentTick(&initial_tick);
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
	
	SceRtcTick tick;
	sceRtcGetCurrentTick(&tick);
	const unsigned int deltaTick  = tick.tick - initial_tick.tick;
	const float deltaSecond = deltaTick * tickRate;
	uint64_t samplepos = deltaSecond * SAMPLE_RATE;
	return samplepos;
}

/*
===============
SNDDMA_Shutdown
===============
*/
void SNDDMA_Shutdown(void)
{
	Com_Printf("Closing audio device...\n");
	if(snd_inited){
		stop_audio = qtrue;
		chn = -1;
	}
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
