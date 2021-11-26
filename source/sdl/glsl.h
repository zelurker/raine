#ifndef SDL_OPENGL_H
#define SDL_OPENGL_H

#ifdef __cplusplus
extern "C" {
#endif

void read_shader(char *shader);
void delete_shaders();
void draw_shader(int linear);
void reset_shaders();
void init_glsl();

#ifdef __cplusplus
}
#endif
#endif
