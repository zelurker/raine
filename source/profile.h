
#ifdef __cplusplus
extern "C" {
#endif
#include "deftypes.h"

// RDTSC Profiler
// --------------

// fps : frames per second. 60 normally. In a "good" implementation it should
// be declared read-only (by hiding the variable). But this is not c++, and
// I need to access this variable very quickly for the timers (see timer.c)
// If you want to change this variable, do it in the load_game function only !
extern float fps,default_fps;
#if SDL == 2
extern int profiler_mode;
#endif

#ifdef RDTSC_PROFILE

// This is one of the very rare code which remains used when NO_ASM is used...
#if defined(RAINE_DOS) || defined(RAINE_UNIX) || defined(__MINGW32__)
#define RDTSC_32(dest) ({				    \
   __asm__ __volatile__ (				    \
   " rdtsc "                                                \
   : "=a" (((UINT32 *)dest)[0])                             \
   :							    \
   : "%edx" ); })                                           \

#define RDTSC_64(dest) ({				    \
   __asm__ __volatile__ (				    \
   " rdtsc "                                                \
   : "=a" (((UINT32 *)dest)[0]), "=d" (((UINT32 *)dest)[1]) \
   : ); })						    \

#else

#undef UINT64
#define UINT64 __int64

DEF_INLINE UINT32 read_rdtsc_32(void)
{
	UINT32 time_rdtsc;

	__asm
	{
		push	eax
		push	edx
//		rdtsc
		_emit	0x0f
		_emit	0x31
		mov		time_rdtsc, eax
		pop		edx
		pop		eax
	}

	return time_rdtsc;
}

DEF_INLINE UINT64 read_rdtsc_64(void)
{
   UINT32 time_rdtsc_low,time_rdtsc_high;

   __asm
   {
      push	eax
      push	edx
    //rdtsc
      _emit	0x0f
      _emit	0x31
      mov	time_rdtsc_low, eax
      mov	time_rdtsc_high, edx
      pop	edx
      pop	eax
   }

   return ( (time_rdtsc_high<<32) | (time_rdtsc_low));
}

#define RDTSC_32(dest)		((UINT32 *)dest)[0] = read_rdtsc_32()

#define RDTSC_64(dest)		((UINT64 *)dest)[0] = read_rdtsc_64()

#endif
#endif // RDTSC_PROFILE

void switch_fps_mode(void);
void init_fps(void);

extern UINT32 timer_next_update, // timer_frame_count at next fps() update
  timer_start_count,	// timer_frame_count at fps() start
  render_start_count,	// render_frame_count at fps() start

  cpu_frame_count,		// number of frames of cpu emulation
  render_frame_count,	// number of frames of video emulation
  skip_frame_count;		// number of frames since last video emulation

extern UINT32 cycles_per_second; // cpu cycles per second

void start_ingame_timer(void);
void stop_ingame_timer(void);
double timer_get_time();
#ifdef RDTSC_PROFILE


extern UINT32 cycles_per_frame;			// cpu cycles per frame, or 0 if not computed

UINT32 read_ingame_cycles(UINT64 *last_rdtsc_counter);
void reset_rdtsc_timer(UINT64 *last_rdtsc_counter);
#endif
UINT32 read_ingame_timer(void);

void reset_ingame_timer(void);
extern int use_rdtsc; // Not sure it will be usefull outside allegro...

typedef struct PROFILE_RESULTS
{
   char *name;
   UINT32 percent;
   UINT32 cycles;
} PROFILE_RESULTS;

enum profile_section
{
   PRO_FRAME = 0,
   PRO_CPU,
   PRO_SOUND,
   PRO_RENDER,
   PRO_BLIT,
   PRO_PAL,
   PRO_FREE,
   PRO_COUNT,			// End marker and list size
};

void ProfileStart(UINT8 mode);
void ProfileStop(UINT8 mode);
void UpdateProfile(void);
extern void restore_ingame_timer(UINT32 timer);

extern struct PROFILE_RESULTS profile_results[PRO_COUNT];

#ifdef __cplusplus
}
#endif
