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
#include "gui.h" // load_progress

static int check_header,header_error;

static int progress_callback(void *clientp,
                             curl_off_t dltotal,
                             curl_off_t dlnow,
                             curl_off_t ultotal,
                             curl_off_t ulnow) {
    if (dltotal == 0) return 0;
    curl_progress_f(dlnow*100/dltotal);
    return 0; // non 0 to abort transfer
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    if (check_header) {
	// nmemb < 2 should never happen, just print a warning
	if (nmemb < 2) printf("should check 2 bytes, got %d bytes\n",nmemb);
	check_header = 0;
	char *hdr = (char*)ptr;
	if (hdr[0] != 0x50 || hdr[1] != 0x4b) { // zip signature : PK
	    header_error = 1;
	    return -1;
	}
    }
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

int get_url(char *file, char *url)
{
  CURL *curl_handle;
  FILE *pagefile;
  int ret;

  curl_global_init(CURL_GLOBAL_ALL);

  /* init the curl session */
  curl_handle = curl_easy_init();

  /* set URL to get here */
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);

  /* Switch on full protocol/debug output while testing */
  curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);

  /* disable progress meter, set to 0L to enable and disable debug output */
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);

  /* send all data to this function  */
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);

  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_XFERINFODATA, NULL);
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
  curl_easy_setopt(curl_handle, CURLOPT_XFERINFOFUNCTION, progress_callback);

  check_header = 1;
  header_error = 0;
  /* open the file */
  pagefile = fopen(file, "wb");
  if(pagefile) {

    /* write the page body to this file handle */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);

    /* get it! */
    ret = curl_easy_perform(curl_handle);

    /* close the header file */
    fclose(pagefile);
    if (header_error) unlink(file);
  }

  /* cleanup curl stuff */
  curl_easy_cleanup(curl_handle);

  curl_global_cleanup();

  return ret;
}

