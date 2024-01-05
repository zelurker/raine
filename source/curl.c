/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2018, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
/* <DESC>
 * Download a given URL into a local file named page.out.
 * </DESC>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <curl/curl.h>
#include <curl/curlver.h>
#include <string.h>
#include "gui.h" // load_progress
#include "files.h"
#include "version.h"
#include "compat.h"

static unsigned long total_size;

static int progress_callback(void *clientp,
                             curl_off_t dltotal,
                             curl_off_t dlnow,
                             curl_off_t ultotal,
                             curl_off_t ulnow) {
    if (dltotal == 0 && total_size == 0) return 0;
    if (!dltotal) dltotal = total_size;
    curl_progress_f(dlnow*100/dltotal);
    return 0; // non 0 to abort transfer
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

extern CURL *curl_handle;

int get_url(char *file, char *url)
{
  FILE *pagefile;
  int ret;
  char agent[80];
  snprintf(agent,80,"Raine " VERSION " using libcurl-%s",LIBCURL_VERSION);
  // The spaces are now forbidden in the url...
  char *s = strchr(url,' ');
  while (s) {
      *s = '+';
      s = strchr(s+1,' ');
  }

  // All the init stuff is taken care in raine.c, so that the connection can be re-used
  // internet archive is sadly regularly slow, it has a heavy traffic clearly so we use
  // all we can to speed up the transfer... !

  /* set URL to get here */
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);

  /* Switch on full protocol/debug output while testing */
  curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_XFERINFODATA, NULL);
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(curl_handle, CURLOPT_XFERINFOFUNCTION, progress_callback);
  curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 1);
  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, FALSE);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, agent);

  /* open the file */
  // Since the download from internet archive doesn't give the size before the download, I need to get it from the static index of their files...
  s = strrchr(url,'/');
  s++;
  char fname[30],fname2[30];
  strcpy(fname,"roms/");
  strncat(fname,s,20);
  snprintf(fname2,30,"Roms.zip/%s",s);
  total_size = 0;
  FILE *f = fopen(get_shared("index_roms.html"),"r");
  if (f) {
      char buff[256];
      while (!feof(f)) {
	  myfgets(buff,256,f);
	  char *s2 = strstr(buff,fname);
	  if (!s2) s2 = strstr(buff,fname2);
	  if (s2) {
	      s2 = strstr(s2+1,"size");
	      total_size = atol(s2+6);
	      break;
	  }
      }
      fclose(f);
  }
  if (total_size == 0) {
      printf("couldn't find zie for %s or %s\n",fname,fname2);
      return 0; // can't put a fatal error here because of all the possible names for the same rom
  }

  ret = curl_easy_perform(curl_handle);
  char *ct = "";
  if (ret == CURLE_OK)
      curl_easy_getinfo(curl_handle,CURLINFO_CONTENT_TYPE,&ct);
  if (strcmp(ct,"application/zip")) {
      printf("curl: didn't get application/zip, aborting... (%s) ret=%x\n",ct,ret);
      return 1;
  }
  curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 0);

  pagefile = fopen(file, "wb");
  if(pagefile) {

    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

    /* get it! */
    ret = curl_easy_perform(curl_handle);

    /* close the header file */
    fclose(pagefile);
    int size = size_file(file);
    if (size == 0) {
	unlink(file);
	ret = 1;
    }
  }

  return ret;
}

