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

#include <signal.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifndef DEDICATED
#ifdef USE_LOCAL_HEADERS
#	include "SDL.h"
#	include "SDL_cpuinfo.h"
#else

#	include <SDL.h>
#	include <SDL_cpuinfo.h>
#include <vitasdk.h>

#endif
#endif

#include "sys_local.h"
#include "sys_loadlib.h"

#include "../qcommon/q_shared.h"
#include "../qcommon/qcommon.h"

int _newlib_heap_size_user = 192 * 1024 * 1024;

static char binaryPath[MAX_OSPATH] = {0};
static char installPath[MAX_OSPATH] = {0};

mode_t umask(mode_t mask) {
    return 0;
}

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

    if ((cliptext = SDL_GetClipboardText()) != NULL) {
        if (cliptext[0] != '\0') {
            size_t bufsize = strlen(cliptext) + 1;

            data = Z_Malloc(bufsize);
            Q_strncpyz(data, cliptext, bufsize);

            // find first listed char and set to '\0'
            strtok(data, "\n\r\b");
        }
        SDL_free(cliptext);
    }
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

    SDL_Quit();

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

    Cvar_Set("r_mode", "-1");
    Cvar_Set("r_customheight", "544");
    Cvar_Set("r_customwidth", "960");
}

/*
=================
Sys_Print
=================
*/
void Sys_Print(const char *msg) {
    //CON_LogWrite(msg);
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

#if 0
/*
=================
Sys_Warn
=================
*/
static __attribute__ ((format (printf, 1, 2))) void Sys_Warn( char *warning, ... )
{
    va_list argptr;
    char    string[1024];

    va_start (argptr,warning);
    Q_vsnprintf (string, sizeof(string), warning, argptr);
    va_end (argptr);

    CON_Print( va( "Warning: %s", string ) );
}
#endif

/*
============
Sys_FileTime

returns -1 if not present
============
*/
int Sys_FileTime(char *path) {
    struct stat buf;

    if (stat(path, &buf) == -1)
        return -1;

    return buf.st_mtime;
}

/*
=================
Sys_UnloadDll
=================
*/
void Sys_UnloadDll(void *dllHandle) {
    Com_Printf("Sys_UnloadDll: not implemented\n");
    return;
}

/*
=================
Sys_LoadDll

First try to load library name from system library path,
from executable path, then fs_basepath.
=================
*/

void *Sys_LoadDll(const char *name, qboolean useSystemLib) {
    Com_Printf("Sys_LoadDll: not implemented\n");
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
                      intptr_t (*systemcalls)(intptr_t, ...)) {
    Com_Printf("Sys_LoadGameDll: not implemented\n");
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
#	ifdef __APPLE__
#		define DEFAULT_BASEDIR Sys_StripAppBundle(Sys_BinaryPath())
#	else
#		define DEFAULT_BASEDIR Sys_BinaryPath()
#	endif
#endif

/*
=================
Sys_SigHandler
=================
*/
void Sys_SigHandler(int signal) {
    static qboolean signalcaught = qfalse;

    if (signalcaught) {
        fprintf(stderr, "DOUBLE SIGNAL FAULT: Received signal %d, exiting...\n",
                signal);
    } else {
        signalcaught = qtrue;
        VM_Forced_Unload_Start();
#ifndef DEDICATED
        CL_Shutdown(va("Received signal %d", signal), qtrue, qtrue);
#endif
        SV_Shutdown(va("Received signal %d", signal));
        VM_Forced_Unload_Done();
    }

    if (signal == SIGTERM || signal == SIGINT)
        Sys_Exit(1);
    else
        Sys_Exit(2);
}

/*
=================
main
=================
*/
int main(int argc, char **argv) {
    int i;
    char commandLine[MAX_STRING_CHARS] = {0};

    //extern void Sys_LaunchAutoupdater(int argc, char **argv);
    //Sys_LaunchAutoupdater(argc, argv);

#ifndef DEDICATED
    // SDL version check

    // Compile time
#	if !SDL_VERSION_ATLEAST(MINSDL_MAJOR, MINSDL_MINOR, MINSDL_PATCH)
#		error A more recent version of SDL is required
#	endif

    // Run time
    SDL_version ver;
    SDL_GetVersion(&ver);

#define MINSDL_VERSION \
    XSTRING(MINSDL_MAJOR) "." \
    XSTRING(MINSDL_MINOR) "." \
    XSTRING(MINSDL_PATCH)

    if (SDL_VERSIONNUM(ver.major, ver.minor, ver.patch) <
        SDL_VERSIONNUM(MINSDL_MAJOR, MINSDL_MINOR, MINSDL_PATCH)) {
        Sys_Dialog(DT_ERROR, va("SDL version " MINSDL_VERSION " or greater is required, "
                                        "but only version %d.%d.%d was found. You may be able to obtain a more recent copy "
                                        "from http://www.libsdl.org/.", ver.major, ver.minor, ver.patch),
                   "SDL Library Too Old");

        Sys_Exit(1);
    }
#endif

    Sys_PlatformInit();

    // Set the initial time base
    Sys_Milliseconds();

    //Sys_ParseArgs(argc, argv);
    sceIoMkdir(DEFAULT_BASEDIR, 777);
    Sys_SetBinaryPath(DEFAULT_BASEDIR);
    Sys_SetDefaultInstallPath(DEFAULT_BASEDIR);

    // Concatenate the command line for passing to Com_Init
    for (i = 1; i < argc; i++) {
        const qboolean containsSpaces = strchr(argv[i], ' ') != NULL;
        if (containsSpaces)
            Q_strcat(commandLine, sizeof(commandLine), "\"");

        Q_strcat(commandLine, sizeof(commandLine), argv[i]);

        if (containsSpaces)
            Q_strcat(commandLine, sizeof(commandLine), "\"");

        Q_strcat(commandLine, sizeof(commandLine), " ");
    }

    CON_Init();
    Com_Init(commandLine);
    NET_Init();

    signal(SIGILL, Sys_SigHandler);
    signal(SIGFPE, Sys_SigHandler);
    signal(SIGSEGV, Sys_SigHandler);
    signal(SIGTERM, Sys_SigHandler);
    signal(SIGINT, Sys_SigHandler);

    while (1) {
        Com_Frame();
    }

    return 0;
}

