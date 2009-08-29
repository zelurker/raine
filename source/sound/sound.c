#ifdef RAINE_DOS
#include <dos.h>
#include <conio.h>
#include <time.h>
#endif

/* audio related stuff */
//HAC hVoice[MIXER_MAX_CHANNELS];
//LPAUDIOWAVE lpWave[MIXER_MAX_CHANNELS];
static int num_used_opl;
int nominal_sample_rate;
int soundcard,usestereo;
int attenuation = 0;

AUDIOINFO info;
AUDIOCAPS caps;

static int stream_playing[MIXER_MAX_CHANNELS];
static void *stream_cache_data[MIXER_MAX_CHANNELS];
static int stream_cache_len[MIXER_MAX_CHANNELS];
static int stream_cache_freq[MIXER_MAX_CHANNELS];
static int stream_cache_volume[MIXER_MAX_CHANNELS];
static int stream_cache_pan[MIXER_MAX_CHANNELS];
static int stream_cache_bits[MIXER_MAX_CHANNELS];


int msdos_init_seal (void)
{
	if (AInitialize() == AUDIO_ERROR_NONE)
		return 0;
	else
		return 1;
}

int msdos_init_sound(void)
{
	int i;
#if 0
	/* Ask the user if no soundcard was chosen */
	if (soundcard == -1)
	{
		unsigned int k;

		printf("\nSelect the audio device:\n");

		for (k = 0;k < AGetAudioNumDevs();k++)
		{
			/* don't show the AWE32, it's too slow, users must choose Sound Blaster */
			if (AGetAudioDevCaps(k,&caps) == AUDIO_ERROR_NONE &&
					strcmp(caps.szProductName,"Sound Blaster AWE32"))
				printf("  %2d. %s\n",k,caps.szProductName);
		}
		printf("\n");

		if (k < 10)
		{
			i = getch();
			soundcard = i - '0';
		}
		else
			scanf("%d",&soundcard);
	}
#endif
	/* initialize SEAL audio library */
	if (soundcard == 0)     /* silence */
	{
		/* update the Machine structure to show that sound is disabled */
		audio_sample_rate = 0;
		return 0;
	}

	/* open audio device */
	/*                              info.nDeviceId = AUDIO_DEVICE_MAPPER;*/
	info.nDeviceId = soundcard;
	/* always use 16 bit mixing if possible - better quality and same speed of 8 bit */
	info.wFormat = AUDIO_FORMAT_16BITS | AUDIO_FORMAT_MONO;

	/* use stereo output if supported */
	if (usestereo)
	{
		//if (Machine->drv->sound_attributes & SOUND_SUPPORTS_STEREO)
			info.wFormat = AUDIO_FORMAT_16BITS | AUDIO_FORMAT_STEREO;
	}

	info.nSampleRate = audio_sample_rate;
	if (AOpenAudio(&info) != AUDIO_ERROR_NONE)
	{
		print_debug("audio initialization failed\n");
		return 1;
	}

	AGetAudioDevCaps(info.nDeviceId,&caps);
	print_debug("Using %s at %d-bit %s %u Hz\n",
			caps.szProductName,
			info.wFormat & AUDIO_FORMAT_16BITS ? 16 : 8,
			info.wFormat & AUDIO_FORMAT_STEREO ? "stereo" : "mono",
			info.nSampleRate);

	/* open and allocate voices, allocate waveforms */
	if (AOpenVoices(MIXER_MAX_CHANNELS) != AUDIO_ERROR_NONE)
	{
		print_debug("voices initialization failed\n");
		return 1;
	}

	for (i = 0; i < MIXER_MAX_CHANNELS; i++)
	{
		if (ACreateAudioVoice(&hVoice[i]) != AUDIO_ERROR_NONE)
		{
			print_debug("voice #%d creation failed\n",i);
			return 1;
		}

		ASetVoicePanning(hVoice[i],128);

		lpWave[i] = 0;

		stream_playing[i] = 0;
		stream_cache_data[0] = 0;
		stream_cache_len[0] = 0;
		stream_cache_freq[0] = 0;
		stream_cache_volume[0] = 0;
		stream_cache_pan[0] = 0;
		stream_cache_bits[0] = 0;
	}

	/* update the Machine structure to reflect the actual sample rate */
	audio_sample_rate = info.nSampleRate;

	print_debug("set sample rate: %d\n",audio_sample_rate);
//#if 0
	{
		uclock_t a,b;
		INT32 start,end;


		if ((lpWave[0] = (LPAUDIOWAVE)malloc(sizeof(AUDIOWAVE))) == 0)
			return 1;

		lpWave[0]->wFormat = AUDIO_FORMAT_8BITS | AUDIO_FORMAT_MONO;
		lpWave[0]->nSampleRate = audio_sample_rate;
		lpWave[0]->dwLength = 3*audio_sample_rate;
		lpWave[0]->dwLoopStart = 0;
		lpWave[0]->dwLoopEnd = 3*audio_sample_rate;
		if (ACreateAudioData(lpWave[0]) != AUDIO_ERROR_NONE)
		{
			free(lpWave[0]);
			lpWave[0] = 0;

			return 1;
		}

		memset(lpWave[0]->lpData,0,3*audio_sample_rate);
		/* upload the data to the audio DRAM local memory */
		AWriteAudioData(lpWave[0],0,3*audio_sample_rate);
		APrimeVoice(hVoice[0],lpWave[0]);
		ASetVoiceFrequency(hVoice[0],audio_sample_rate);
		ASetVoiceVolume(hVoice[0],0);
		AStartVoice(hVoice[0]);

		a = uclock();
		/* wait some time to let everything stabilize */
		do
		{
			AUpdateAudioEx(audio_sample_rate / CPU_FPS);
			b = uclock();
		} while (b-a < UCLOCKS_PER_SEC/10);

		a = uclock();
		AGetVoicePosition(hVoice[0],&start);
		do
		{
			AUpdateAudioEx(audio_sample_rate / CPU_FPS);
			b = uclock();
		} while (b-a < UCLOCKS_PER_SEC);
		AGetVoicePosition(hVoice[0],&end);

		nominal_sample_rate = audio_sample_rate;
		//audio_sample_rate = end - start;
		audio_sample_rate = 22048;
		print_debug("actual sample rate: %d\n",audio_sample_rate);

		AStopVoice(hVoice[0]);
		ADestroyAudioData(lpWave[0]);
		free(lpWave[0]);
		lpWave[0] = 0;
	}
//#endif

#if 0
	{
		char *blaster_env;
		/* Get Soundblaster base address from environment variabler BLASTER   */
		/* Soundblaster OPL base port, at some compatibles this must be 0x388 */

		if(!getenv("BLASTER"))
		{
			printf("\nBLASTER variable not found, disabling fm sound!\n");
                        No_OPL = options.no_fm = 1;
		}
		else
		{
			blaster_env = getenv("BLASTER");
			BaseSb = i = 0;
			while ((blaster_env[i] & 0x5f) != 0x41) i++;        /* Look for 'A' char */
			while (blaster_env[++i] != 0x20) {
				BaseSb = (BaseSb << 4) + (blaster_env[i]-0x30);
			}
		}
	}
#endif
	num_used_opl = 0;

	osd_set_mastervolume(attenuation);	/* set the startup volume */

	return 0;
}

void msdos_shutdown_sound(void)
{
	if (audio_sample_rate != 0)
	{
		int chip,n;

		for(chip=0;chip<num_used_opl;chip++)
		{
			/* silence the OPL */
			for (n = 0x40;n <= 0x55;n++)
			{
				osd_opl_control(chip,n);
				osd_opl_write(chip,0x3f);
			}
			for (n = 0x60;n <= 0x95;n++)
			{
				osd_opl_control(chip,n);
				osd_opl_write(chip,0xff);
			}
			for (n = 0xa0;n <= 0xb0;n++)
			{
				osd_opl_control(chip,n);
				osd_opl_write(chip,0);
			}
		}

		/* stop and release voices */
		for (n = 0; n < MIXER_MAX_CHANNELS; n++)
		{
			AStopVoice(hVoice[n]);
			ADestroyAudioVoice(hVoice[n]);
			if (lpWave[n])
			{
				ADestroyAudioData(lpWave[n]);
				free(lpWave[n]);
				lpWave[n] = 0;
			}
		}
		ACloseVoices();
		ACloseAudio();
	}
}



static void playsample(int channel,signed char *data,int len,int freq,int volume,int loop,int bits)
{
  if (audio_sample_rate == 0 || channel >= MIXER_MAX_CHANNELS) return;
	/* backwards compatibility with old 0-255 volume range */
	if (volume > 100) volume = volume * 25 / 255;

	if (lpWave[channel] && lpWave[channel]->dwLength != len)
	{
		AStopVoice(hVoice[channel]);
		ADestroyAudioData(lpWave[channel]);
		free(lpWave[channel]);
		lpWave[channel] = 0;
	}

	if (lpWave[channel] == 0)
	{
		if ((lpWave[channel] = (LPAUDIOWAVE)malloc(sizeof(AUDIOWAVE))) == 0)
			return;

		if (loop) lpWave[channel]->wFormat = (bits == 8 ? AUDIO_FORMAT_8BITS : AUDIO_FORMAT_16BITS)
				| AUDIO_FORMAT_MONO | AUDIO_FORMAT_LOOP;
		else lpWave[channel]->wFormat = (bits == 8 ? AUDIO_FORMAT_8BITS : AUDIO_FORMAT_16BITS)
				| AUDIO_FORMAT_MONO;
		lpWave[channel]->nSampleRate = nominal_sample_rate;
		lpWave[channel]->dwLength = len;
		lpWave[channel]->dwLoopStart = 0;
		lpWave[channel]->dwLoopEnd = len;
		if (ACreateAudioData(lpWave[channel]) != AUDIO_ERROR_NONE)
		{
			free(lpWave[channel]);
			lpWave[channel] = 0;

			return;
		}
	}
	else
	{
		if (loop) lpWave[channel]->wFormat = (bits == 8 ? AUDIO_FORMAT_8BITS : AUDIO_FORMAT_16BITS)
				| AUDIO_FORMAT_MONO | AUDIO_FORMAT_LOOP;
		else lpWave[channel]->wFormat = (bits == 8 ? AUDIO_FORMAT_8BITS : AUDIO_FORMAT_16BITS)
				| AUDIO_FORMAT_MONO;
	}

	memcpy(lpWave[channel]->lpData,data,len);
	/* upload the data to the audio DRAM local memory */
	AWriteAudioData(lpWave[channel],0,len);
	APrimeVoice(hVoice[channel],lpWave[channel]);
	/* need to cast to double because freq*nominal_sample_rate can exceed the size of an int */
	ASetVoiceFrequency(hVoice[channel],(double)freq*nominal_sample_rate/audio_sample_rate);
	ASetVoiceVolume(hVoice[channel],volume * 64 / 100);
	AStartVoice(hVoice[channel]);
}

void osd_play_sample(int channel,signed char *data,int len,int freq,int volume,int loop)
{
	playsample(channel,data,len,freq,volume,loop,8);
}

void osd_play_sample_16(int channel,signed short *data,int len,int freq,int volume,int loop)
{
	playsample(channel,(signed char *)data,len,freq,volume,loop,16);
}


#define NUM_BUFFERS 3	/* raising this number should improve performance with frameskip, */
						/* but also increases the latency. */
static int streams_are_playing;

static int playstreamedsample(int channel,signed char *data,int len,int freq,int volume,int pan,int bits)
{
	static int c[MIXER_MAX_CHANNELS];


	/* backwards compatibility with old 0-255 volume range */
	if (volume > 100) volume = volume * 25 / 255;

	/* SEAL double volume for panned channels, so we have to compensate */
	if (pan != MIXER_PAN_CENTER) volume /= 2;

	if (audio_sample_rate == 0 || channel >= MIXER_MAX_CHANNELS) return 1;

	if (!stream_playing[channel])
	{
		if (lpWave[channel])
		{
			AStopVoice(hVoice[channel]);
			ADestroyAudioData(lpWave[channel]);
			free(lpWave[channel]);
			lpWave[channel] = 0;
		}

		if ((lpWave[channel] = (LPAUDIOWAVE)malloc(sizeof(AUDIOWAVE))) == 0)
			return 1;

		lpWave[channel]->wFormat = (bits == 8 ? AUDIO_FORMAT_8BITS : AUDIO_FORMAT_16BITS)
				| AUDIO_FORMAT_MONO | AUDIO_FORMAT_LOOP;
		lpWave[channel]->nSampleRate = nominal_sample_rate;
		lpWave[channel]->dwLength = NUM_BUFFERS*len;
		lpWave[channel]->dwLoopStart = 0;
		lpWave[channel]->dwLoopEnd = NUM_BUFFERS*len;
		if (ACreateAudioData(lpWave[channel]) != AUDIO_ERROR_NONE)
		{
			free(lpWave[channel]);
			lpWave[channel] = 0;

			return 1;
		}

		memset(lpWave[channel]->lpData,0,NUM_BUFFERS*len);
		memcpy(lpWave[channel]->lpData,data,len);
		/* upload the data to the audio DRAM local memory */
		AWriteAudioData(lpWave[channel],0,NUM_BUFFERS*len);
		APrimeVoice(hVoice[channel],lpWave[channel]);
	/* need to cast to double because freq*nominal_sample_rate can exceed the size of an int */
		ASetVoiceFrequency(hVoice[channel],(double)freq*nominal_sample_rate/audio_sample_rate);
		AStartVoice(hVoice[channel]);
		stream_playing[channel] = 1;
		c[channel] = 1;

		streams_are_playing = 1;
	}
	else
	{
		INT32 pos;
		extern int throttle;

//#if 0
		if (throttle)   /* sync with audio only when speed throttling is on */
		{
			AGetVoicePosition(hVoice[channel],&pos);
			if (bits == 16) pos *= 2;
			if (pos >= c[channel] * len && pos < (c[channel]+1)*len) return 0;
		}
//#endif
		memcpy(&lpWave[channel]->lpData[len * c[channel]],data,len);
		AWriteAudioData(lpWave[channel],len*c[channel],len);
		c[channel] = (c[channel]+1) % NUM_BUFFERS;

		streams_are_playing = 1;
	}


	ASetVoiceVolume(hVoice[channel],volume * 64 / 100);
	if (pan == MIXER_PAN_CENTER)
		ASetVoicePanning(hVoice[channel],128);
	else if (pan == MIXER_PAN_LEFT)
		ASetVoicePanning(hVoice[channel],0);
	else if (pan == MIXER_PAN_RIGHT)
		ASetVoicePanning(hVoice[channel],255);

	return 1;
}

void osd_play_streamed_sample(int channel,signed char *data,int len,int freq,int volume,int pan)
{
	stream_cache_data[channel] = data;
	stream_cache_len[channel] = len;
	stream_cache_freq[channel] = freq;
	stream_cache_volume[channel] = volume;
	stream_cache_pan[channel] = pan;
	stream_cache_bits[channel] = 8;
}

void osd_play_streamed_sample_16(int channel,signed short *data,int len,int freq,int volume,int pan)
{
	stream_cache_data[channel] = data;
	stream_cache_len[channel] = len;
	stream_cache_freq[channel] = freq;
	stream_cache_volume[channel] = volume;
	stream_cache_pan[channel] = pan;
	stream_cache_bits[channel] = 16;
}



void osd_set_sample_freq(int channel,int freq)
{
	if (audio_sample_rate == 0 || channel >= MIXER_MAX_CHANNELS) return;

	/* need to cast to double because freq*nominal_sample_rate can exceed the size of an int */
	ASetVoiceFrequency(hVoice[channel],(double)freq*nominal_sample_rate/audio_sample_rate);
}

void osd_set_sample_volume(int channel,int volume)
{
	if (audio_sample_rate == 0 || channel >= MIXER_MAX_CHANNELS) return;

	/* backwards compatibility with old 0-255 volume range */
	if (volume > 100) volume = volume * 25 / 255;

	ASetVoiceVolume(hVoice[channel],volume * 64 / 100);
}



void osd_stop_sample(int channel)
{
	if (audio_sample_rate == 0 || channel >= MIXER_MAX_CHANNELS) return;

	AStopVoice(hVoice[channel]);
}


void osd_restart_sample(int channel)
{
	if (audio_sample_rate == 0 || channel >= MIXER_MAX_CHANNELS) return;

	AStartVoice(hVoice[channel]);
}


int osd_get_sample_status(int channel)
{
	int stopped=0;
	if (audio_sample_rate == 0 || channel >= MIXER_MAX_CHANNELS) return -1;

	AGetVoiceStatus(hVoice[channel], &stopped);
	return stopped;
}



static int update_streams(void)
{
	int channel;
	int res = 1;


	streams_are_playing = 0;

	for (channel = 0;channel < MIXER_MAX_CHANNELS;channel++)
	{
		if (stream_cache_data[channel])
		{
			if (playstreamedsample(
					channel,
					stream_cache_data[channel],
					stream_cache_len[channel],
					stream_cache_freq[channel],
					stream_cache_volume[channel],
					stream_cache_pan[channel],
					stream_cache_bits[channel]))
				stream_cache_data[channel] = 0;
			else
				res = 0;
		}
	}

	return res;
}

int msdos_update_audio(void)
{
	if (audio_sample_rate == 0) return 0;

	//profiler_mark(PROFILER_SOUND);
	AUpdateAudioEx(audio_sample_rate / CPU_FPS);
	//profiler_mark(PROFILER_END);

	//profiler_mark(PROFILER_IDLE);
	while (update_streams() == 0)
		AUpdateAudioEx(audio_sample_rate / CPU_FPS);
	//profiler_mark(PROFILER_END);

	return streams_are_playing;
}





static int master_volume = 256;

/* attenuation in dB */
void osd_set_mastervolume(int _attenuation)
{
	float volume;


	if (_attenuation > 0) _attenuation = 0;
	if (_attenuation < -32) _attenuation = -32;

	attenuation = _attenuation;

 	volume = 256.0;	/* range is 0-256 */
	while (_attenuation++ < 0)
		volume /= 1.122018454;	/* = (10 ^ (1/20)) = 1dB */

	master_volume = volume;

	ASetAudioMixerValue(AUDIO_MIXER_MASTER_VOLUME,master_volume);
}

int osd_get_mastervolume(void)
{
	return attenuation;
}

void osd_sound_enable(int enable_it)
{
	if (enable_it)
	{
		ASetAudioMixerValue(AUDIO_MIXER_MASTER_VOLUME,master_volume);
	}
	else
	{
		ASetAudioMixerValue(AUDIO_MIXER_MASTER_VOLUME,0);
	}
}



/* linux sound driver opl3.c does a so called tenmicrosec() delay */
static void tenmicrosec(void)
{
    int i;
    for (i = 0; i < 16; i++)
        inportb(0x80);
}

//#define MAX_OPLCHIP 2  /* SOUND BLASTER 16 or compatible ?? */
#define MAX_OPLCHIP 1  /* SOUND BLASTER pro compatible ??  */

void osd_opl_control(int chip,int reg)
{
    if (audio_sample_rate == 0) return;

    if (chip >= MAX_OPLCHIP ) return;
    tenmicrosec();
    outportb(0x388+chip*2,reg);
}

void osd_opl_write(int chip,int data)
{
    if (audio_sample_rate == 0) return;

    if (chip >=MAX_OPLCHIP ) return;
    tenmicrosec();
    outportb(0x389+chip*2,data);

	if(chip >= num_used_opl) num_used_opl = chip+1;
}
