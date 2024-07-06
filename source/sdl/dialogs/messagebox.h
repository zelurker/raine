#ifndef __MESSAGEBOX_H
#define __MESSAGEBOX_H

#define ErrorMsg(msg) raine_mbox("Error",msg,"OK")

#ifdef __cplusplus
extern "C" {

#if defined(raine_mbox)
#undef raine_mbox
#endif

int raine_mbox(char *title, char *content, char *buttons = NULL);

}
#else
int raine_mbox(char *title, char *content, char *buttons);
#endif

#endif

