// Can't include curl.h here because of a header conflict in windows !
// so curl_handle must stay out of this !

int get_url(char *file, char *url);

#ifndef CURLE_ABORTED_BY_CALLBACK
// For super stupid windows which makes a mega conflict when trying to include curl.h at the top of loadroms.c
// should really investigate if there is not another way to define these types
#define CURLE_ABORTED_BY_CALLBACK 42
#endif
