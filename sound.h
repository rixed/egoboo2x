// sound.h

#ifndef _SOUND_H_
#define _SOUND_H_


#include "egobootypedef.h"

extern SDL_AudioSpec *gHardwareSpec;

typedef struct
{
	BOOL			m_Free;
	SDL_AudioSpec	m_AudioSpec;
	Uint8			*m_AudioBuffer;
	Uint32			m_AudioLen;
} SDLSound;

typedef struct 
{	
	BOOL	m_Active;
	Uint32	m_SoundBufferIdx;
	BOOL	m_IsLooped;
	Uint32	m_CurPlayPos;
	Uint32	m_Volume;
} SDLActiveSound;


extern BOOL InitSound( Uint32 pBufferSize, Uint32 pFreq, Uint32 pMaxSoundChannel );
extern BOOL ShutdownSound( void );

extern Uint32	gSoundOutputSPS; // sample per second
extern Uint32	gSoundOutputMixBufferSize;
extern Uint32	gSoundMaxSoundChannel;

#endif // #ifndef _SOUND_H_
