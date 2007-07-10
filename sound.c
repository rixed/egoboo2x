// sound.c

// Egoboo, Copyright (C) 2000 Aaron Bishop

#include "egoboo.h"
#include "sound.h"

#define MAX_SDL_SOUND 512 // Max of 512 loaded sound
#define MAX_SDL_MIXING_SOUND 16 // Max of simultaneous playing sounds

BOOL	gSoundOn = FALSE;
SDL_AudioSpec	*gHardwareSpec = NULL;
SDLSound		*gSDLSoundList = NULL;
SDLActiveSound	*gSDLSoundChannelList = NULL;
BOOL			gSoundMixingPaused = TRUE;
Uint32			gNumChannelToMix = 0;
float mainVolume = .8f;

Uint32	gSoundOutputSPS; // sample per second
Uint32	gSoundOutputMixBufferSize;
Uint32	gSoundMaxSoundChannel;


//------------------------------------------------------------------------------
//Sound Routines----------------------------------------------------------------
//------------------------------------------------------------------------------

/*----------------
 
 Returns the nunber of sample for the specified sound buffer
 Don't work on compressed sounds ( ADPCM )

prog:
	Germain Sauvé

----------------*/
Sint32	GetNumSamples( Uint16 format, Uint8 channels, Uint32 size )
{
	Uint8	lBytePerChan = 0;
	Sint32	lNumSample;

	switch( format )
	{
		case AUDIO_U8:
		case AUDIO_S8:
			lBytePerChan = 1;
			break;
		case AUDIO_U16LSB:
		case AUDIO_S16LSB:
		case AUDIO_U16MSB:
		case AUDIO_S16MSB:
			lBytePerChan = 2;
			break;
		default:
			return -1; // format undefined
			break;
	}
	lNumSample = size / (lBytePerChan * channels);
	return lNumSample;
	
}

/*----------------
 
 Returns the index of a free SDL_sound or -1 if any are available

prog:
	Germain Sauvé

----------------*/
Sint32	GetFreeSoundIdx()
{
	Sint32 lCheckIdx; 
	
	if ( gSDLSoundList == NULL )
	{
		return -1;
	}
	lCheckIdx = 0;
	while ( lCheckIdx < MAX_SDL_SOUND )
		{
		if ( gSDLSoundList[lCheckIdx].m_Free )
			{
			return lCheckIdx;
			}
		lCheckIdx++;
		}

	return -1;
}


/*----------------
 
 Returns the index of a free SDL_sound or -1 if any are available

prog:
	Germain Sauvé

----------------*/
Sint32	GetFreePlayingIdx()
{
	Sint32 lCheckIdx; 
	
	if ( gSDLSoundChannelList == NULL )
	{
		return -1;
	}
	lCheckIdx = 0;
	while ( lCheckIdx < (Sint32)gNumChannelToMix )
		{
		if ( !gSDLSoundChannelList[lCheckIdx].m_Active )
			{
			return lCheckIdx;
			}
		lCheckIdx++;
		}

	return -1;
}

/*----------------
 
 Main mixing callback function

 Doesn't support looping sounds.

prog:
	Germain Sauvé

----------------*/
void EgobooSoundMixCallback( void *userdata, Uint8 *stream, int len)
{
	(void)userdata;
	Uint32	lCompt;
	Uint32	lSizeToMix;

	if ( gSDLSoundChannelList == NULL )
		{
		return;
		}
	lCompt = 0;
	while (lCompt < gNumChannelToMix )
	{
		if ( gSDLSoundChannelList[lCompt].m_Active )
		{
			// check if the sound buffer still exist
			if ( gSDLSoundList[gSDLSoundChannelList[lCompt].m_SoundBufferIdx].m_Free == FALSE )
			{
				// mix the sound in the stream
				if ( (gSDLSoundChannelList[lCompt].m_CurPlayPos + len) < gSDLSoundList[gSDLSoundChannelList[lCompt].m_SoundBufferIdx].m_AudioLen )
					{
					SDL_MixAudio( stream, &gSDLSoundList[gSDLSoundChannelList[lCompt].m_SoundBufferIdx].m_AudioBuffer[gSDLSoundChannelList[lCompt].m_CurPlayPos], len, gSDLSoundChannelList[lCompt].m_Volume );
					gSDLSoundChannelList[lCompt].m_CurPlayPos += len;
					}
				else
					{
					lSizeToMix = gSDLSoundList[gSDLSoundChannelList[lCompt].m_SoundBufferIdx].m_AudioLen - gSDLSoundChannelList[lCompt].m_CurPlayPos;
					SDL_MixAudio( stream, &gSDLSoundList[gSDLSoundChannelList[lCompt].m_SoundBufferIdx].m_AudioBuffer[gSDLSoundChannelList[lCompt].m_CurPlayPos], lSizeToMix, gSDLSoundChannelList[lCompt].m_Volume );
					// no looping for now
					gSDLSoundChannelList[lCompt].m_Active = FALSE;
					}
				// update the sound channel
			}
			else
			{
				// stop this sound channel since it points to an unused sound
				gSDLSoundChannelList[lCompt].m_Active = FALSE;
			}
		}
		lCompt++;
	}
}

/*----------------
 
 Initialize SDL sound system

prog:
	Germain Sauvé

----------------*/
BOOL InitSound( Uint32 pBufferSize, Uint32 pFreq, Uint32 pMaxSoundChannel )
{
	int OpenErr;
	Sint32	lCompt;

	if ( pMaxSoundChannel > MAX_SDL_MIXING_SOUND)
	{
		pMaxSoundChannel = MAX_SDL_MIXING_SOUND;
	}
	gNumChannelToMix = pMaxSoundChannel;
	
	gHardwareSpec = (SDL_AudioSpec *)malloc( sizeof( SDL_AudioSpec ) );
	gHardwareSpec->freq = pFreq;
	gHardwareSpec->format = AUDIO_S16SYS;	// 16 bit 
	gHardwareSpec->channels = 1; // MONO for the beta
	gHardwareSpec->samples = pBufferSize; // in samples
	gHardwareSpec->size = 0; // will be set by samples
	gHardwareSpec->callback = EgobooSoundMixCallback;
	gHardwareSpec->userdata = NULL;

	OpenErr = SDL_OpenAudio(gHardwareSpec, NULL);
	if ( OpenErr < 0)
		{
		// error
		return FALSE;
		}

	// initialize the sound list and the sound channels
	gSDLSoundList = (SDLSound *)malloc( sizeof(SDLSound) * MAX_SDL_SOUND );
	gSDLSoundChannelList = (SDLActiveSound *)malloc( sizeof( SDLActiveSound ) * pMaxSoundChannel);
	if ( gSDLSoundList != NULL && gSDLSoundChannelList != NULL )
	{
		for ( lCompt = 0; lCompt < MAX_SDL_SOUND; lCompt++ )
		{
			gSDLSoundList[lCompt].m_Free = TRUE;
			gSDLSoundList[lCompt].m_AudioBuffer = NULL;
		}
		for ( lCompt = 0; lCompt < (Sint32)pMaxSoundChannel; lCompt++ )
		{
			gSDLSoundChannelList[lCompt].m_Active = FALSE;
		}
		gSoundOn = TRUE;
		SDL_PauseAudio( 0 ); // start mixing sounds
		gSoundMixingPaused = FALSE;
	}
	return TRUE;

}


/*----------------
 
 Shutdown SDL sound system

prog:
	Germain Sauvé

----------------*/
BOOL ShutdownSound( void )
{
	if ( gSoundOn == TRUE )
	{
		SDL_PauseAudio( 1 );
		gSoundMixingPaused = TRUE;
		SDL_CloseAudio();
		if ( gHardwareSpec != NULL )
		{
			free( gHardwareSpec );
		}
		gSoundOn = FALSE;
	}
	return TRUE;
}

/*----------------
 
 Looks for an inactive sound channel and plays the sound "index" with volume.
 Doesn't use pan and frequency

prog:
	Germain Sauvé
	prog X

----------------*/
void play_sound_pvf(int index, int pan, int volume, int frequency)
{
	(void)frequency;
    // ZZ> This function starts playing a sound
	Sint32	lFreeChannel;
	float	lCalcVolume;
/* 
pan goes from -10000 to 10000 and volume goes from 0 to -10000.
for now, only volume count and panning is simulated
*/
	if ( gSoundOn == FALSE || index < 0 || index >= MAX_SDL_SOUND )
		{
		return;
		}

	if ( gSDLSoundList[index].m_Free == TRUE)
		{
		// no sound at index
		return;
		}
	SDL_LockAudio();
	lFreeChannel = GetFreePlayingIdx();
	if ( lFreeChannel != -1 )
	{
		gSDLSoundChannelList[lFreeChannel].m_Active = TRUE;
		gSDLSoundChannelList[lFreeChannel].m_SoundBufferIdx = (Uint32)index;
		gSDLSoundChannelList[lFreeChannel].m_IsLooped = FALSE;
		gSDLSoundChannelList[lFreeChannel].m_CurPlayPos = 0;
		lCalcVolume = (128.0f + ((float)volume / 78.25 )) * mainVolume;
		if ( lCalcVolume <= 0.0f)
		{
			lCalcVolume = 0.0f;
		}
		else
		{
			// simulate panning
			if ( pan != 0 )
			{
				lCalcVolume -= (lCalcVolume / 2.0f) * (abs( pan ) / 10000.0f);
			}
		}
		gSDLSoundChannelList[lFreeChannel].m_Volume = (Uint32)lCalcVolume; // cheap for the moment
		//printf("Ask to play sound index %i\n", index);
	}
	SDL_UnlockAudio();
	
/*PORT
    if(index < numsound && index >= 0 && soundon)
    {
        lpDSBuffer[index]->Stop();
        lpDSBuffer[index]->SetCurrentPosition(0);
        lpDSBuffer[index]->SetPan(pan);
        lpDSBuffer[index]->SetVolume(volume);
        lpDSBuffer[index]->SetFrequency(frequency);
        lpDSBuffer[index]->Play(0, 0, 0);
    }
*/
}

//------------------------------------------------------------------------------
void play_sound_skip(int index, int skip)
{
	(void)index;
	(void)skip;
    // ZZ> This function starts playing a sound with padding
/*PORT
    if(index < numsound && index >= 0 && soundon)
    {
        lpDSBuffer[index]->Stop();
        lpDSBuffer[index]->SetCurrentPosition(skip);
        lpDSBuffer[index]->SetPan(PANMID);
        lpDSBuffer[index]->SetVolume(VOLMAX);
        lpDSBuffer[index]->SetFrequency(11025);
        lpDSBuffer[index]->Play(0, 0, 0);
    }
*/
}

//------------------------------------------------------------------------------
void play_sound_pvf_looped(int index, int pan, int volume, int frequency)
{
	(void)index;
	(void)pan;
	(void)volume;
	(void)frequency;
    // ZZ> This function starts playing a looped sound
/* PORT
    if(index < numsound && index >= 0 && soundon)
    {
        lpDSBuffer[index]->SetPan(pan);
        lpDSBuffer[index]->SetVolume(volume);
        lpDSBuffer[index]->SetFrequency(frequency);
        lpDSBuffer[index]->Play(0, 0, DSBPLAY_LOOPING);
    }
*/
}

/*----------------
 
 Stop all sound channels playing sound "index"

prog:
	Germain Sauvé
	prog X

----------------*/
void stop_sound(int index)
{
// since there isn't any concept of sound channel on DiretcX ( weird )
// stop_sound looks for any sound channel currently playing "index" and stops it
	Uint32	lCheckChan;
	if ( gSoundOn == FALSE || index < 0 || index >= MAX_SDL_SOUND )
	{
		return;
	}
	SDL_LockAudio(); 
	for ( lCheckChan = 0; lCheckChan < gNumChannelToMix/*MAX_SDL_MIXING_SOUND*/; lCheckChan++ )
	{
		if ( gSDLSoundChannelList[lCheckChan].m_Active )
		{
			if ( gSDLSoundChannelList[lCheckChan].m_SoundBufferIdx == (Uint32)index )
			{
				gSDLSoundChannelList[lCheckChan].m_Active = FALSE;
			}
		}
	}
	SDL_UnlockAudio();
}

/*----------------
 
 Stop a specific sound channel

prog:
	Germain Sauvé

----------------*/
void StopSoundChannel( Uint32 pChannel )
{
	if ( gSoundOn == FALSE || pChannel >= MAX_SDL_MIXING_SOUND )
	{
		return;
	}
	SDL_LockAudio(); 
	gSDLSoundChannelList[pChannel].m_Active = FALSE;
	SDL_UnlockAudio();
}

/*----------------
 
 Stop all sound channel

prog:
	Germain Sauvé

----------------*/
void StopAllSoundChannel( void )
{
	Uint32 lCompt;

	if ( gSoundOn == FALSE )
	{
		return;
	}
	printf("gona LockAudio\n");
glBindTexture(GL_TEXTURE_2D, 0);
	SDL_LockAudio(); 
	printf("gona Set some False\n");
glBindTexture(GL_TEXTURE_2D, 0);
	for ( lCompt = 0; lCompt < gNumChannelToMix/*MAX_SDL_MIXING_SOUND*/; lCompt++ )
	{
		gSDLSoundChannelList[lCompt].m_Active = FALSE;
	}
	printf("gona UnLockAudio\n");
glBindTexture(GL_TEXTURE_2D, 0);
	SDL_UnlockAudio();
}

/*----------------


prog:
	Germain Sauvé

----------------*/
void PauseMixingSound( void )
{
	if (!gSoundMixingPaused && gSoundOn)
	{
		SDL_PauseAudio( 1 );
		gSoundMixingPaused = TRUE;
	}
}

/*----------------


prog:
	Germain Sauvé

----------------*/
void ResumeMixingSound( void )
{
	if (gSoundMixingPaused && gSoundOn)
	{
		SDL_PauseAudio( 1 );
		gSoundMixingPaused = FALSE;
	}
}

/*----------------


prog:
	Germain Sauvé

----------------*/
BOOL IsMixingPaused( void )
{
	return gSoundMixingPaused;
}


/*----------------
 
 Remove the sound "index" from the list and from memory

prog:
	Germain Sauvé

----------------*/
void RemoveSound( Uint32 index )
{
	if ( gSoundOn == FALSE || index >= MAX_SDL_SOUND )
	{
		return;
	}
	if ( gSDLSoundList[index].m_Free )
	{
		return;
	}
	SDL_LockAudio(); 
	if ( gSDLSoundList[index].m_AudioBuffer != NULL )
	{
		free( gSDLSoundList[index].m_AudioBuffer );
		gSDLSoundList[index].m_AudioBuffer = NULL;
	}
	gSDLSoundList[index].m_Free = TRUE;
	SDL_UnlockAudio();
}

/*----------------
 
 Remove all sounds from the list

prog:
	Germain Sauvé
	prog X

----------------*/
void reset_sounds()
{
	Uint32 lCompt;

	if ( gSoundOn == FALSE )
	{
		return;
	}
	printf("gona StopAllSoundChannel\n");
glBindTexture(GL_TEXTURE_2D, 0);
	StopAllSoundChannel();
	printf("gona RemoveSound\n");
glBindTexture(GL_TEXTURE_2D, 0);
	for ( lCompt = 0; lCompt < MAX_SDL_SOUND; lCompt++ )
	{
		RemoveSound( lCompt );
	}
	printf("gona return\n");
glBindTexture(GL_TEXTURE_2D, 0);
/*PORT
    int cnt = 0;
    if(numsound == 0)
    {
        // Set 'em all to unused
        while(cnt < MAXSOUND)
        {
            lpDSBuffer[cnt] = NULL;
            cnt++;
        }
    }
    else
    {
        // Free 'em up
        while(cnt < numsound)
        {
            lpDSBuffer[cnt]->Stop();
            RELEASE(lpDSBuffer[cnt]);
            cnt++;
        }
    }
    numsound = 0;
*/
}

/*----------------
 
 Loads one .wav file and returns its index number or -1 if it failed

prog:
	Germain Sauvé
	prog X

----------------*/
int load_one_wave(char *szFileName)
{
	SDL_AudioCVT  wav_cvt;
	SDL_AudioSpec wav_spec;
	Uint32	wav_length;
	Uint32	wav_cvt_len;
	Uint8	*wav_buffer;
	Sint32	lSDLSoundIdx;
	int		ret;
	Sint32 lNumSample;

	if ( gSoundOn == FALSE ) { return -1; }
	
	lSDLSoundIdx = GetFreeSoundIdx();
	if ( lSDLSoundIdx == -1 ) { return -1; }

	/* Load the WAV */
	if( SDL_LoadWAV(szFileName, &wav_spec, &wav_buffer, &wav_length) == NULL ) { return -1; }

	//printf("loading sound %s at index %i\n", szFileName, lSDLSoundIdx);
	// convert the wav to 16 bit mono at hardware SPS
	ret = SDL_BuildAudioCVT( &wav_cvt, wav_spec.format, wav_spec.channels, wav_spec.freq,
			AUDIO_S16SYS, 1, gHardwareSpec->freq );
	if ( ret == -1 ) { SDL_FreeWAV(wav_buffer); return -1; }

	if ( gSDLSoundList[lSDLSoundIdx].m_AudioBuffer != NULL )
	{
	  free( gSDLSoundList[lSDLSoundIdx].m_AudioBuffer );
	  gSDLSoundList[lSDLSoundIdx].m_AudioBuffer = NULL;
	}

	lNumSample = GetNumSamples(wav_spec.format,wav_spec.channels,wav_length);
	if ( lNumSample == -1 ) { SDL_FreeWAV(wav_buffer); return -1; }

	wav_cvt_len = (Uint32)((float)(lNumSample * 2) * ( (float)gHardwareSpec->freq / (float)wav_spec.freq )); // 16 bit mono
	if ( wav_cvt_len < wav_length )
	{
		gSDLSoundList[lSDLSoundIdx].m_AudioBuffer = (Uint8 *)malloc( wav_length );
	}
	else
	{
		gSDLSoundList[lSDLSoundIdx].m_AudioBuffer = (Uint8 *)malloc( wav_cvt_len );
	}
	gSDLSoundList[lSDLSoundIdx].m_AudioLen = wav_cvt_len;
	wav_cvt.buf = gSDLSoundList[lSDLSoundIdx].m_AudioBuffer;
	wav_cvt.len = wav_length;

	memcpy( gSDLSoundList[lSDLSoundIdx].m_AudioBuffer, wav_buffer, wav_length );
	// Free memory used by WAV file
	SDL_FreeWAV(wav_buffer);
	
	ret = SDL_ConvertAudio(&wav_cvt);

	if ( ret == -1 ) { free( gSDLSoundList[lSDLSoundIdx].m_AudioBuffer ); gSDLSoundList[lSDLSoundIdx].m_AudioBuffer=NULL; return -1; }

	// weird bug!!! probably ADPCM
	if (wav_cvt_len != wav_cvt.len*wav_cvt.len_ratio)
	{
		if ( wav_cvt_len > wav_cvt.len*wav_cvt.len_ratio )
		{
			gSDLSoundList[lSDLSoundIdx].m_AudioLen = wav_cvt.len*wav_cvt.len_ratio;
		}
		else
		{
			// Major error
			// will probably crash anyway
			free( gSDLSoundList[lSDLSoundIdx].m_AudioBuffer );
                        gSDLSoundList[lSDLSoundIdx].m_AudioBuffer = NULL;
			return -1;
		}
	}
	gSDLSoundList[lSDLSoundIdx].m_Free = FALSE;
	return lSDLSoundIdx;

/* PORT
    DSBUFFERDESC    dsbd;
    LPVOID          pbData = NULL;
    LPVOID          pbData2 = NULL;
    DWORD           dwLength;
    DWORD           dwLength2;

    UINT            Size;
    DWORD           Samples;
    WAVEFORMATEX    *WaveFormat;
    BYTE            *Data;
    HRESULT         hr;


    // Only bother if sound is available
    if(soundon)
    {
        // Load the data from the file into memory ( at Data )
        hr = WaveLoadFile(szFileName, &Size, &Samples, &WaveFormat, &Data);
        if(hr || Size == 0)
        {
            // File not loaded, so just quit
            return -1;
        }


        // Set up the direct sound buffer. 
        memset(&dsbd, 0, sizeof(DSBUFFERDESC));
        dsbd.dwSize = sizeof(DSBUFFERDESC);
        dsbd.dwFlags = DSBCAPS_STATIC | DSBCAPS_CTRLDEFAULT | DSBCAPS_GETCURRENTPOSITION2;
        dsbd.dwBufferBytes = Size;
        dsbd.lpwfxFormat = WaveFormat;
        hr = lpDirectSound->CreateSoundBuffer(&dsbd, &lpDSBuffer[numsound], NULL);
        if(hr)
        {
            // Free the loadfile memory...
            if(WaveFormat!=NULL)  GlobalFreePtr(WaveFormat);
            if(Data!=NULL)  GlobalFreePtr(Data);
            return -1;
        }


        // Get ready to copy data
        hr = lpDSBuffer[numsound]->Lock(0, Size, &pbData, &dwLength, &pbData2, &dwLength2, 0L);
        if(hr)
        {
            // The buffer didn't get locked...  That's bad
            RELEASE(lpDSBuffer[numsound]);
            if(WaveFormat!=NULL)  GlobalFreePtr(WaveFormat);
            if(Data!=NULL)  GlobalFreePtr(Data);
            return -1;
        }


        // Copy the data to the sound buffer
        memcpy(pbData, Data, Size);


        // Set the buffer back to its unlocked state.
        hr = lpDSBuffer[numsound]->Unlock(pbData, Size, NULL, 0);
        if(hr)
        {
            // The buffer didn't get unlocked...  That's bad
            RELEASE(lpDSBuffer[numsound]);
            if(WaveFormat!=NULL)  GlobalFreePtr(WaveFormat);
            if(Data!=NULL)  GlobalFreePtr(Data);
            return -1;
        }


        // Set the pan, volume, and frequency
        lpDSBuffer[numsound]->SetPan(PANMID);
        lpDSBuffer[numsound]->SetVolume(VOLMAX);
        lpDSBuffer[numsound]->SetFrequency(FRQDEFAULT);
        if(Data!=NULL)  GlobalFreePtr(Data);
        numsound++;
        return numsound-1;
    }
    return -1;
*/
}

//------------------------------------------------------------------------------
//Music Stuff-------------------------------------------------------------------
// Everything under this can go away!!!!
//------------------------------------------------------------------------------
void load_all_music_sounds(char *modname)
{
    // ZZ> This function loads all of the music sounds
    char loadname[128];
    int cnt;

    // Reset instruments
    instrumentsloaded = FALSE;
    musicon = FALSE;
    cnt = 0;
    while(cnt < MAXINSTRUMENT)
    {
        instrumenttosound[cnt] = -1;
        cnt++;
    }
    // Load the data
    if(musicvalid)
    {
        cnt = 0;
        while(cnt < MAXINSTRUMENT)
        {
            sprintf(loadname, FILENAME("%s/music%02d.wav"), modname, cnt);
            instrumenttosound[cnt] = load_one_wave(loadname);
            if(instrumenttosound[cnt] != -1)  instrumentsloaded = TRUE;
            cnt++;
        }
    }
}

//------------------------------------------------------------------------------
void create_ig_trackmaster()
{
    // ZZ> This function creates a look-up table.  The table tells the
    //     interactive music code which music??.wav file to play next, based on
    //     what we want to play, and what we're are playing currently

    // Want to play a normal track...
    igtrackmaster[IGNORMAL][IGNM01] = IGNM01;
    igtrackmaster[IGNORMAL][IGNM02] = IGNM01;
    igtrackmaster[IGNORMAL][IGNM03] = IGNM01;
    igtrackmaster[IGNORMAL][IGSM01] = IGSM03;
    igtrackmaster[IGNORMAL][IGSM02] = IGSM03;
    igtrackmaster[IGNORMAL][IGSM03] = IGNM01;
    igtrackmaster[IGNORMAL][IGCM01] = IGCM03;
    igtrackmaster[IGNORMAL][IGCM02] = IGCM03;
    igtrackmaster[IGNORMAL][IGCM03] = IGNM01;

    // Want to play a secret track...
    igtrackmaster[IGSECRET][IGNM01] = IGSM01;
    igtrackmaster[IGSECRET][IGNM02] = IGSM01;
    igtrackmaster[IGSECRET][IGNM03] = IGSM01;
    igtrackmaster[IGSECRET][IGSM01] = IGSM02;
    igtrackmaster[IGSECRET][IGSM02] = IGSM02;
    igtrackmaster[IGSECRET][IGSM03] = IGSM01;
    igtrackmaster[IGSECRET][IGCM01] = IGCM03;
    igtrackmaster[IGSECRET][IGCM02] = IGCM03;
    igtrackmaster[IGSECRET][IGCM03] = IGSM01;

    // Want to play a combat track...
    igtrackmaster[IGCOMBAT][IGNM01] = IGCM01;
    igtrackmaster[IGCOMBAT][IGNM02] = IGCM01;
    igtrackmaster[IGCOMBAT][IGNM03] = IGCM01;
    igtrackmaster[IGCOMBAT][IGSM01] = IGSM03;
    igtrackmaster[IGCOMBAT][IGSM02] = IGSM03;
    igtrackmaster[IGCOMBAT][IGSM03] = IGCM01;
    igtrackmaster[IGCOMBAT][IGCM01] = IGCM02;
    igtrackmaster[IGCOMBAT][IGCM02] = IGCM02;
    igtrackmaster[IGCOMBAT][IGCM03] = IGCM01;
}

//------------------------------------------------------------------------------
void load_ig_length(char *filename)
{
    // ZZ> This function sets up interactive music...
    FILE* fileread;
    int iTmp;

    ignowplaying = IGNM01;
    igpasstime = 0;
    igfilesize = 87142;
    igstopped = TRUE;
    ignexttrack = IGNORMAL;
    igtrackcount = 1;
    fileread = fopen(FILENAME(filename), "r");
    if(fileread)
    {
        globalname = filename;
        fscanf(fileread, "%d", &iTmp);
        igfilesize = iTmp;
        fclose(fileread);
    }
    play_sound_skip(instrumenttosound[ignowplaying], 0);
}


//------------------------------------------------------------------------------
void check_ig_passage()
{
    // ZZ> This function checks the passages every .20 seconds to see if the track
    //     needs to be changed
    int cnt, x, y;


    if(igloaded)
    {
        if(igpasstime >= 10)
        {
            // Check the passages
            x = camtrackx;  x = x>>7;
            y = camtracky;  y = y>>7;
            cnt = 0;
            while(cnt < numpassage)
            {
                // Has the passage been setup for music?
                if(passtracktype[cnt] != IGNOTRACK)
                {
                    // Is the camera looking at the passage?
                    if(x >= passtlx[cnt] && x <= passbrx[cnt])
                    {
                        if(y >= passtly[cnt] && y <= passbry[cnt])
                        {
                            // Do the sound...
                            ignexttrack = passtracktype[cnt];
                            igtrackcount = passtrackcount[cnt];

                            // Reset the passage to normal
                            passtracktype[cnt] = IGNOTRACK;
                            cnt = numpassage;
                        }
                    }
                }
                cnt++;
            }
            igpasstime = 0;
        }
        else
        {
            // Don't need to yet
            igpasstime++;
        }
    }
}

//------------------------------------------------------------------------------
void check_ig_music()
{
    // ZZ> This function updates the igmusic counter, and plays the next sound
/* PORT
    int cnt;
    int skip;
    int stopped;
    int oldplaying;
    DWORD dTmp;


    if(igloaded)
    {
        //  See if we're ready...
        stopped = igstopped;
        lpDSBuffer[instrumenttosound[ignowplaying]]->GetCurrentPosition(&dTmp, NULL);
        cnt = dTmp;  if(cnt == 0)  cnt = igfilesize;
        skip = igfilesize-cnt;  // Bytes left to play
        if(skip < 0)  skip = 0;
        if(skip < 2205)  // 2205 bytes = 100 msec
        {
            // Pick a new track...
            oldplaying = ignowplaying;
            ignowplaying = igtrackmaster[ignexttrack][ignowplaying];
            if(ignowplaying == IGNM01)  ignowplaying = allclock%3;  // A random normal track
            if(oldplaying == ignowplaying && skip > 100)  return;  // Try in vain to fix problems...


            // Start it playing
            play_sound_skip(instrumenttosound[ignowplaying], 2205-skip);
            stopped = FALSE;


            // Down the counters
            if(ignowplaying == IGSM02 || ignowplaying == IGCM02)
            {
                igtrackcount--;
            }
            if(igtrackcount <= 0)  ignexttrack = IGNORMAL;
            skip = igfilesize;
        }
        if(skip < (igfilesize-4000) && igstopped==FALSE)
        {
            // Stop all old sounds before they start playing again
            cnt = 0;
            while(cnt < IGINSTRUMENTS)
            {
                if(cnt != ignowplaying)
                {
                    stop_sound(instrumenttosound[cnt]);
                }
                cnt++;
            }
            stopped = TRUE;
        }
        igstopped = stopped;
    }
*/
}


//------------------------------------------------------------------------------
void load_all_music_tracks(char *modname)
{
	(void)modname;
    // ZZ> This function loads the music track file
/*PORT
    char loadname[128];
    FILE* fileread;
    float fTmp;
    float playtime;
    unsigned int writehead;
    int reading;
    unsigned int iTmp;


    musicon = FALSE;
    numtrack = 0;
    writehead = 0;
    sprintf(loadname, DATA_PATH"modules\\%s\\music\\tracks.txt", modname);
    fileread = fopen(loadname, "r");
    if(fileread != NULL)
    {
        while(goto_colon_yesno(fileread))
        {
            fscanf(fileread, "%f", &playtime);
            trackheadstart[numtrack] = writehead;
            trackmaxtime[numtrack] = playtime*ONESECOND;
            reading = TRUE;
            while(reading)
            {
                goto_colon(fileread);
                fscanf(fileread, "%f", &fTmp);
                if(fTmp >= playtime)
                {
                    reading = FALSE;
                    trackheadend[numtrack] = writehead;
                }
                else
                {
                    trackplaytime[writehead] = fTmp*ONESECOND;
                    fscanf(fileread, "%d", &iTmp);  trackplaypan[writehead] = iTmp;
                    fscanf(fileread, "%d", &iTmp);  trackplayvol[writehead] = iTmp;
                    fscanf(fileread, "%d", &iTmp);  trackplayfrq[writehead] = iTmp;
                    fscanf(fileread, "%d", &iTmp);  trackplayinstrument[writehead] = iTmp;
                    writehead++;
                }
            }
            numtrack++;
        }
        fclose(fileread);
    }
*/
}

//------------------------------------------------------------------------------
void load_music_sequence(char *modname)
{
	(void)modname;
    // ZZ> This function loads the music sequence file
/*PORT
    char loadname[128];
    FILE* fileread;
    unsigned int iTmp;


    numsequence = 0;
    sequence[0] = 0;
    nextsequence = 0;


    sprintf(loadname, DATA_PATH"modules\\%s\\music\\sequence.txt", modname);
    fileread = fopen(loadname, "r");
    if(fileread)
    {
        while(goto_colon_yesno(fileread))
        {
            fscanf(fileread, "%d", &iTmp);
            sequence[numsequence] = iTmp;
            numsequence++;
        }
        fclose(fileread);
    }
*/
}

//------------------------------------------------------------------------------
void load_all_music_loops(char *modname)
{
	(void)modname;
    // ZZ> This function loads the music loop file and begins playing
/*PORT
    char loadname[128];
    FILE* fileread;
    int pan, vol, frq, sound;


    if(musicvalid)
    {
        sprintf(loadname, DATA_PATH"modules\\%s\\music\\loops.txt", modname);
        fileread = fopen(FILENAME(loadname), "r");
        if(fileread)
        {
            while(goto_colon_yesno(fileread))
            {
                fscanf(fileread, "%d", &pan);
                fscanf(fileread, "%d", &vol);
                fscanf(fileread, "%d", &frq);
                fscanf(fileread, "%d", &sound);
                sound = instrumenttosound[sound];
                pan = (PANRIGHT*pan/100) + (PANLEFT*(100-pan)/100);
                vol = (VOLMAX*vol/100) + (VOLMIN*(100-vol)/100);
                play_sound_pvf_looped(sound, pan, vol, frq);
            }
            fclose(fileread);
        }
    }
*/
}

//---------------------------------------------------------------------------------------------
void start_music_track(int track)
{
	(void)track;
    // ZZ> This function starts playing a new track
/*PORT
    if(track < numtrack && musicvalid)
    {
        trackhead = trackheadstart[track];
        tracktime = 0;
        playingtrack = track;
        musicon = TRUE;
    }
*/
}

//---------------------------------------------------------------------------------------------
void play_next_track()
{
/*PORT
    // ZZ> This function begins playing the next track in the sequence
    start_music_track(sequence[nextsequence]);
    nextsequence++;
    if(nextsequence >= numsequence)
    {
        nextsequence = 0;
    }
*/
}

//---------------------------------------------------------------------------------------------
void stop_music()
{
    // ZZ> This function stops the music
/*PORT
    musicon = FALSE;
*/
}

//---------------------------------------------------------------------------------------------
void change_music_track(int track)
{
	(void)track;
    // ZZ> This function starts a new track, but at the same time as the old one
    //     so we don't miss a beat
/*PORT
    int cnt;

    if(track < numtrack)
    {
        trackhead = trackheadstart[track];
        cnt = trackplaytime[trackhead];
        while(cnt < tracktime)
        {
            trackhead++;
            cnt = trackplaytime[trackhead];
        }
        tracktime = 0;
        playingtrack = track;
        musicon = TRUE;
    }
*/
}

//---------------------------------------------------------------------------------------------
void play_next_music_sound()
{
    // ZZ> This function plays the next sound if its time has come and advances the
    //     track head
/* PORT
    int     sound;
    int     pan;
    int     vol;
    int     frq;
    int     fallout;


    if(musicon)
    {
        // Play all the current sounds
        fallout = FALSE;
        while(trackplaytime[trackhead] == tracktime && fallout == FALSE)
        {
            if(trackhead < trackheadend[playingtrack])
            {
                sound = instrumenttosound[trackplayinstrument[trackhead]];
                pan = trackplaypan[trackhead];
                pan = (PANRIGHT*pan/100) + (PANLEFT*(100-pan)/100);
                vol = trackplayvol[trackhead];
                vol = (VOLMAX*vol/100) + (VOLMIN*(100-vol)/100);
                frq = trackplayfrq[trackhead];
                play_sound_pvf(sound, pan, vol, frq);
            }
            else
            {
                fallout = TRUE;
            }
            trackhead++;
        }
        // Start new track if finished
        tracktime++;
        if(tracktime >= trackmaxtime[playingtrack])
        {
            play_next_track();
        }
    }
*/
}

