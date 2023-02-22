#pragma once

#ifndef _VITA_DEFS_H
#define _VITA_DEFS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <vitasdk.h>

#include "psp2_dll_imports.h"

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

#undef mkdir
#define mkdir( path, mode ) sceIoMkdir( (path), (mode) )
#define _snprintf snprintf
#define _sprintf sprintf

// HACK: don't use actual libc functions for mem and io
#undef malloc
#undef calloc
#undef realloc
#undef free
#undef fopen
#undef fclose
#undef fseek
#undef ftell
#undef fread
#undef fwrite
#undef fprintf
#undef sprintf
#undef snprintf
#undef vsnprintf
#undef _vsnprintf

#define malloc SYS_MALLOC
#define calloc SYS_CALLOC
#define realloc SYS_REALLOC
#define free( x ) SYS_FREE( x )
#define fopen SYS_FOPEN
#define fclose SYS_FCLOSE
#define fseek SYS_FSEEK
#define ftell SYS_FTELL
#define fread SYS_FREAD
#define fwrite SYS_FWRITE
#define fprintf SYS_FPRINTF
#define sprintf SYS_SPRINTF
#define snprintf SYS_SNPRINTF
#define vsnprintf SYS_VSNPRINTF
#define _vsnprintf vsnprintf

typedef struct dllexport_s
{
	const char *name;
	void *func;
} dllexport_t;

extern dllexport_t psp2_exports[];

#ifdef __cplusplus
}
#endif

#endif
