#ifndef _EGOBOOTYPEDEF_H_
#define _EGOBOOTYPEDEF_H_

//#pragma warning(disable : 4305)                     // Turn off silly warnings
//#pragma warning(disable : 4244)                     //

#ifdef GP2X
#	define CONF_PATH       "./"
#	define DATA_PATH       "./"
#else
#	define CONF_PATH       "/etc/egoboo/"
#	define DATA_PATH       "/usr/share/games/egoboo/"
#endif
#define MAX_STR_SIZE    512

// LINUX typedef
#ifdef _LINUX
#include <SDL/SDL.h>
#include <endian.h>
// use SDL type 
typedef Uint32			BOOL;
typedef Sint32			LONG;
typedef Uint32			DWORD;
typedef struct lin_RECT { LONG left; LONG right; LONG top; LONG bottom; } RECT;

#if __BYTE_ORDER == __LITTLE_ENDIAN
# define LE32bitToHost( pData, pNumByte ) pData
# define BE32bitToHost( pData, pNumByte ) pData
# define LE16bitToHost( pData, pNumByte ) pData
# define BE16bitToHost( pData, pNumByte ) pData
# define EndianChange32bit( pData, pNumByte ) pData
# define EndianChange16bit( pData, pNumByte ) pData
# define _LITTLE_ENDIAN	1
#else
# define LE32bitToHost( pData, pNumByte ) pData
# define BE32bitToHost( pData, pNumByte ) pData
# define LE16bitToHost( pData, pNumByte ) _swab( (char *)(pData ), (char *)(pData ), (pNumByte) )
# define BE16bitToHost( pData, pNumByte ) pData
# define EndianChange32bit( pData, pNumByte ) pData
# define EndianChange16bit( pData, pNumByte ) _swab( (char *)(pData ), (char *)(pData ), (pNumByte) )
# define _BIG_ENDIAN	1
#endif

#endif	/* _LINUX */

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!FALSE)	// nv20001128 -- standard definition. This may save us grief later on
#endif




#endif // #ifndef _EGOBOOTYPEDEF_H_

