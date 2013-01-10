#ifndef SDL_OPENGL_H
#define SDL_OPENGL_H

#ifdef __cplusplus
extern "C" {
#endif

void opengl_reshape(int w, int h);
void get_ogl_infos();
void draw_opengl();
void opengl_text(char *msg, int x, int y);
void finish_opengl();
void opengl_done();

#ifdef __cplusplus
}
#endif

#endif
