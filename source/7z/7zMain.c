/* 7zMain.c - Test application for 7z Decoder
2021-04-29 : Igor Pavlov : Public domain */

#include "loadroms.h"
#include "files.h"
#include "newmem.h"
#include "compat.h"
#include "debug.h"

#include "Precomp.h"

#include <stdio.h>
#include <string.h>

#include "CpuArch.h"

#include "7z.h"
#include "7zAlloc.h"
#include "7zBuf.h"
#include "7zCrc.h"
#include "7zFile.h"
#include "7zVersion.h"

#ifndef USE_WINDOWS_FILE
/* for mkdir */
#ifdef _WIN32
#include <direct.h>
#else
#include <stdlib.h>
#include <time.h>
#ifdef __GNUC__
#include <sys/time.h>
#endif
#include <fcntl.h>
// #include <utime.h>
#include <sys/stat.h>
#include <errno.h>
#endif
#endif

#ifdef __GLIBC__
#if __GLIBC__ >= 2
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif
#elif defined(DARWIN) || defined(ANDROID)
#define stricmp strcasecmp
#define strnicmp strncasecmp
#endif

#define kInputBufSize ((size_t)1 << 18)

static const ISzAlloc g_Alloc = { SzAlloc, SzFree };

static int Buf_EnsureSize(CBuf *dest, size_t size)
{
  if (dest->size >= size)
    return 1;
  Buf_Free(dest, &g_Alloc);
  return Buf_Create(dest, size, &g_Alloc);
}

#ifndef _WIN32
#define _USE_UTF8
#endif

/* #define _USE_UTF8 */

#ifdef _USE_UTF8

#define _UTF8_START(n) (0x100 - (1 << (7 - (n))))

#define _UTF8_RANGE(n) (((UInt32)1) << ((n) * 5 + 6))

#define _UTF8_HEAD(n, val) ((Byte)(_UTF8_START(n) + (val >> (6 * (n)))))
#define _UTF8_CHAR(n, val) ((Byte)(0x80 + (((val) >> (6 * (n))) & 0x3F)))

static size_t Utf16_To_Utf8_Calc(const UInt16 *src, const UInt16 *srcLim)
{
  size_t size = 0;
  for (;;)
  {
    UInt32 val;
    if (src == srcLim)
      return size;

    size++;
    val = *src++;

    if (val < 0x80)
      continue;

    if (val < _UTF8_RANGE(1))
    {
      size++;
      continue;
    }

    if (val >= 0xD800 && val < 0xDC00 && src != srcLim)
    {
      UInt32 c2 = *src;
      if (c2 >= 0xDC00 && c2 < 0xE000)
      {
        src++;
        size += 3;
        continue;
      }
    }

    size += 2;
  }
}

static Byte *Utf16_To_Utf8(Byte *dest, const UInt16 *src, const UInt16 *srcLim)
{
  for (;;)
  {
    UInt32 val;
    if (src == srcLim)
      return dest;

    val = *src++;

    if (val < 0x80)
    {
      *dest++ = (Byte)val;
      continue;
    }

    if (val < _UTF8_RANGE(1))
    {
      dest[0] = _UTF8_HEAD(1, val);
      dest[1] = _UTF8_CHAR(0, val);
      dest += 2;
      continue;
    }

    if (val >= 0xD800 && val < 0xDC00 && src != srcLim)
    {
      UInt32 c2 = *src;
      if (c2 >= 0xDC00 && c2 < 0xE000)
      {
        src++;
        val = (((val - 0xD800) << 10) | (c2 - 0xDC00)) + 0x10000;
        dest[0] = _UTF8_HEAD(3, val);
        dest[1] = _UTF8_CHAR(2, val);
        dest[2] = _UTF8_CHAR(1, val);
        dest[3] = _UTF8_CHAR(0, val);
        dest += 4;
        continue;
      }
    }

    dest[0] = _UTF8_HEAD(2, val);
    dest[1] = _UTF8_CHAR(1, val);
    dest[2] = _UTF8_CHAR(0, val);
    dest += 3;
  }
}

static SRes Utf16_To_Utf8Buf(CBuf *dest, const UInt16 *src, size_t srcLen)
{
  size_t destLen = Utf16_To_Utf8_Calc(src, src + srcLen);
  destLen += 1;
  if (!Buf_EnsureSize(dest, destLen))
    return SZ_ERROR_MEM;
  *Utf16_To_Utf8(dest->data, src, src + srcLen) = 0;
  return SZ_OK;
}

#endif

static SRes Utf16_To_Char(CBuf *buf, const UInt16 *s
    #ifndef _USE_UTF8
    , UINT codePage
    #endif
    )
{
  unsigned len = 0;
  for (len = 0; s[len] != 0; len++) {}

  #ifndef _USE_UTF8
  {
    const unsigned size = len * 3 + 100;
    if (!Buf_EnsureSize(buf, size))
      return SZ_ERROR_MEM;
    {
      buf->data[0] = 0;
      if (len != 0)
      {
        const char defaultChar = '_';
        BOOL defUsed;
        const unsigned numChars = (unsigned)WideCharToMultiByte(
            codePage, 0, (LPCWSTR)s, (int)len, (char *)buf->data, (int)size, &defaultChar, &defUsed);
        if (numChars == 0 || numChars >= size)
          return SZ_ERROR_FAIL;
        buf->data[numChars] = 0;
      }
      return SZ_OK;
    }
  }
  #else
  return Utf16_To_Utf8Buf(buf, s, len);
  #endif
}

#ifdef _WIN32
  #ifndef USE_WINDOWS_FILE
    static UINT g_FileCodePage = CP_ACP;
    #define MY_FILE_CODE_PAGE_PARAM ,g_FileCodePage
  #endif
#else
  #define MY_FILE_CODE_PAGE_PARAM
#endif

#define PERIOD_4 (4 * 365 + 1)
#define PERIOD_100 (PERIOD_4 * 25 - 1)
#define PERIOD_400 (PERIOD_100 * 4 + 1)



#ifndef _WIN32

// MS uses long for BOOL, but long is 32-bit in MS. So we use int.
// typedef long BOOL;
#ifndef BOOL
typedef int BOOL;
#endif

typedef struct _FILETIME
{
  DWORD dwLowDateTime;
  DWORD dwHighDateTime;
} FILETIME;

#define TICKS_PER_SEC 10000000

#define GET_TIME_64(pft) ((pft)->dwLowDateTime | ((UInt64)(pft)->dwHighDateTime << 32))

#define SET_FILETIME(ft, v64) \
   (ft)->dwLowDateTime = (DWORD)v64; \
   (ft)->dwHighDateTime = (DWORD)(v64 >> 32);

#if defined(_AIX)
  #define MY_ST_TIMESPEC st_timespec
#else
  #define MY_ST_TIMESPEC timespec
#endif

#endif

// Returns the size of the file in case of success, 0 if error
int load_7z(char *zipfile, char *name, unsigned int offs, unsigned int size, int crc32, unsigned char *dest, int actual_load)
{
  static ISzAlloc allocImp;
  static ISzAlloc allocTempImp;

  static CFileInStream archiveStream;
  static CLookToRead2 lookStream;
  static CSzArEx db;
  SRes res;
  static char oldfile[FILENAME_MAX];
  static UInt16 *temp = NULL;
  static size_t tempSize = 0;
      /*
      if you need cache, use these 3 variables.
      if you use external function, you can make these variable as static.
      */
  static UInt32 blockIndex = 0xFFFFFFFF; /* it can have any value before first call (if outBuffer = 0) */
  static Byte *outBuffer = 0; /* it must be 0 before first call for each new archive. */
  static size_t outBufferSize = 0;  /* it can have any value before first call (if outBuffer = 0) */
  static size_t outSizeProcessed = 0;
  size_t len;
  UInt32 i;

  #if defined(_WIN32) && !defined(USE_WINDOWS_FILE) && !defined(UNDER_CE)
  g_FileCodePage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;
  #endif


  if (!*oldfile) {
      allocImp = g_Alloc;
      allocTempImp = g_Alloc;
  }

  if (strcmp(oldfile,zipfile))
  {
      if (!exists(zipfile))
	  return 0;
      if (*oldfile && outBuffer) {
	  ISzAlloc_Free(&allocImp, outBuffer);
	  SzFree(NULL, temp);
	  tempSize = 0;
	  temp = NULL;
	  SzArEx_Free(&db, &allocImp);
	  ISzAlloc_Free(&allocImp, lookStream.buf);

	  File_Close(&archiveStream.file);
	  outBuffer = 0;
	  *oldfile = 0;
      }
      WRes wres =
#ifdef UNDER_CE
	  InFile_OpenW(&archiveStream.file, L"\test.7z"); // change it
#else
      InFile_Open(&archiveStream.file, zipfile);
#endif
      if (wres != 0)
      {
	  // We don't display the error here, raine has something specific
	  return 0;
      }

      FileInStream_CreateVTable(&archiveStream);
      archiveStream.wres = 0;
      LookToRead2_CreateVTable(&lookStream, False);
      lookStream.buf = NULL;

      res = SZ_OK;

      {
	  lookStream.buf = (Byte *)ISzAlloc_Alloc(&allocImp, kInputBufSize);
	  if (!lookStream.buf)
	      res = SZ_ERROR_MEM;
	  else
	  {
	      lookStream.bufSize = kInputBufSize;
	      lookStream.realStream = &archiveStream.vt;
	      LookToRead2_Init(&lookStream);
	  }
      }

      CrcGenerateTable();

      SzArEx_Init(&db);

      if (res == SZ_OK)
      {
	  res = SzArEx_Open(&db, &lookStream.vt, &allocImp, &allocTempImp);
      }
      strncpy(oldfile,zipfile,FILENAME_MAX);
      if (res != SZ_OK) return 0;
  }

  {

      for (i = 0; i < db.NumFiles; i++)
      {
	  size_t offset = 0;
	  const BoolInt isDir = SzArEx_IsDir(&db, i);
	  if (isDir)
	      continue;
	  len = SzArEx_GetFileNameUtf16(&db, i, NULL);

	  if (len > tempSize)
	  {
	      if (temp) SzFree(NULL, temp);
	      tempSize = len;
	      temp = (UInt16 *)SzAlloc(NULL, tempSize * sizeof(temp[0]));
	      if (!temp)
	      {
		  res = SZ_ERROR_MEM;
		  break;
	      }
	  }

	  SzArEx_GetFileNameUtf16(&db, i, temp);
	  CBuf buf;
	  Buf_Init(&buf);
	  Utf16_To_Char(&buf,temp);

	  if (db.CRCs.Vals[i] == crc32 || !stricmp(name,(char*)buf.data))
	  {
	      if (actual_load) {
		  res = SzArEx_Extract(&db, &lookStream.vt, i,
			  &blockIndex, &outBuffer, &outBufferSize,
			  &offset, &outSizeProcessed,
			  &allocImp, &allocTempImp);
		  if (outSizeProcessed >= size) {
		      memcpy(dest,outBuffer+offset+offs,size);
		      if (outSizeProcessed > size) {
			  if (remaining_b) {
			      FreeMem(remaining_b);
			      remaining_b = NULL;
			  }
			  remaining_size = outSizeProcessed - size;
			  // Shouldn't happen, but it does, found out thanks to
			  // efence. Apparently it's just the remaining_size
			  // which is too big, the buffer is not overloaded at
			  // this point
			  if (remaining_size+offset+offs+size > outBufferSize)
			      remaining_size = outBufferSize-(offset+offs+size);
			  remaining_b = AllocateMem(remaining_size);
			  memcpy(remaining_b,outBuffer+offset+offs+size,remaining_size);
		      } else if (remaining_b) {
			  FreeMem(remaining_b);
			  remaining_size = 0;
			  remaining_b = NULL;
		      }
		  } else if (outSizeProcessed)
		      memcpy(dest,outBuffer+offset+offs,outSizeProcessed);
		  if (res == SZ_ERROR_CRC) {
		      load_error |= LOAD_WARNING;

		      if (load_debug)
			  sprintf(load_debug+strlen(load_debug),
				  "Got a bad CRC for ROM %s (%x)\n",name,crc32);
		  }
	      } else
		  res = SZ_OK;
	      Buf_Free(&buf, &g_Alloc);
	      break;
	  }

	  Buf_Free(&buf, &g_Alloc);
      }
      if (i == db.NumFiles)
	  return 0; // not found
  }

  if (res == SZ_OK)
  {
    // Print("\nEverything is Ok\n");
      print_debug("load_7z %s from %s ok\n",name,zipfile);
    return SzArEx_GetFileSize(&db,i);
  }

  return 0;
}
