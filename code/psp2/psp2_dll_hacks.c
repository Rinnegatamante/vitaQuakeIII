#include <stdint.h>
#include <vitasdk.h>
#include <stdio.h>
#include <stdlib.h>

void* __dso_handle = (void*) &__dso_handle;

extern void _init_vita_reent( void );
extern void _free_vita_reent( void );
extern void _init_vita_heap( void );
extern void _free_vita_heap( void );

extern void __libc_init_array( void );
extern void __libc_fini_array( void );

void _init_vita_newlib( void )
{
	_init_vita_heap( );
	_init_vita_reent( );
}

void _free_vita_newlib( void )
{
	_free_vita_reent( );
	_free_vita_heap( );
}

void _fini( void ) { }
void _init( void ) { }

// small heap for internal libc use
unsigned int _newlib_heap_size_user = 2 * 1024 * 1024;

//////////////////////////////////////

typedef struct modarg_s
{
	sysfuncs_t imports;
	dllexport_t *exports;
} modarg_t;

extern void dllEntry( intptr_t (*syscallptr)( intptr_t arg,... ) );
extern intptr_t vmMain( intptr_t command, intptr_t arg0, intptr_t arg1, intptr_t arg2, intptr_t arg3, intptr_t arg4, intptr_t arg5, intptr_t arg6, intptr_t arg7, intptr_t arg8, intptr_t arg9, intptr_t arg10, intptr_t arg11 );

dllexport_t psp2_exports[] =
{
	{ "dllEntry", (void*)dllEntry },
	{ "vmMain", (void*)vmMain },
	{ NULL, NULL },
};

int module_stop( SceSize argc, const void *args )
{
	__libc_fini_array( );
	_free_vita_newlib( );
	return SCE_KERNEL_STOP_SUCCESS;
}

int module_exit( )
{
	__libc_fini_array( );
	_free_vita_newlib( );
	return SCE_KERNEL_STOP_SUCCESS;
}

sysfuncs_t g_engsysfuncs;

#ifdef GAMEDLL
typedef struct {
	char *funcStr;
	uint8_t *funcPtr;
} funcList_t;

extern funcList_t funcList[];
#endif

void _start() __attribute__ ((weak, alias ("module_start")));
int module_start( SceSize argc, void *args )
{
	_init_vita_newlib( );
	__libc_init_array( );

	modarg_t *arg = *(modarg_t **)args;
	arg->exports = psp2_exports;
	g_engsysfuncs = arg->imports;

#ifdef GAMEDLL
	// HACK
	funcList[14].funcPtr = (void*)g_engsysfuncs.pfnSnprintf;
#endif

	return SCE_KERNEL_START_SUCCESS;
}
