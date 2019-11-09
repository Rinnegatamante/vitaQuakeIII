#pragma once

#ifndef _SYSFUNCS_H
#define _SYSFUNCS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

typedef struct sysfuncs_s
{
	// mem
	void* (*pfnSysMalloc)(size_t);
	void* (*pfnSysCalloc)(size_t, size_t);
	void* (*pfnSysRealloc)(void*, size_t);
	void  (*pfnSysFree)(void*);
	// i/o
	FILE* (*pfnSysFopen)(const char*, const char*);
	int (*pfnSysFclose)(FILE*);
	int (*pfnSysFseek)(FILE*, long int, int);
	long int (*pfnSysFtell)(FILE*);
	int (*pfnSysFprintf)(FILE*, const char*, ...);
	size_t (*pfnSysFread)(void*, size_t, size_t, FILE*);
	size_t (*pfnSysFwrite)(const void*, size_t, size_t, FILE*);
	// sprintf
	int (*pfnSprintf)(char*, const char*, ...);
	int (*pfnSnprintf)(char*, int, const char*, ...);
	int (*pfnVsnprintf)(char*, int, const char*, va_list);
} sysfuncs_t;

extern sysfuncs_t g_engsysfuncs;

// needed because we don't want to use a new heap for each dll
#define SYS_MALLOC (*g_engsysfuncs.pfnSysMalloc)
#define SYS_CALLOC (*g_engsysfuncs.pfnSysCalloc)
#define SYS_REALLOC (*g_engsysfuncs.pfnSysRealloc)
#define SYS_FREE (*g_engsysfuncs.pfnSysFree)

#define SYS_FOPEN (*g_engsysfuncs.pfnSysFopen)
#define SYS_FCLOSE (*g_engsysfuncs.pfnSysFclose)
#define SYS_FSEEK (*g_engsysfuncs.pfnSysFseek)
#define SYS_FTELL (*g_engsysfuncs.pfnSysFtell)
#define SYS_FPRINTF (*g_engsysfuncs.pfnSysFprintf)
#define SYS_FREAD (*g_engsysfuncs.pfnSysFread)
#define SYS_FWRITE (*g_engsysfuncs.pfnSysFwrite)

#define SYS_SPRINTF (*g_engsysfuncs.pfnSprintf)
#define SYS_SNPRINTF (*g_engsysfuncs.pfnSnprintf)
#define SYS_VSNPRINTF (*g_engsysfuncs.pfnVsnprintf)

#ifdef __cplusplus
}
#endif

#endif
