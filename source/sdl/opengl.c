#include <SDL.h>
#include <SDL_opengl.h>
#include "sdl/compat.h"
#include "blit.h"
#include "blit_sdl.h"
#include "games.h"

void opengl_reshape(int w, int h) {
    glViewport(0, 0, w, h);
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set the clipping volume
    gluOrtho2D(0.0f, (GLfloat) w, 0.0, (GLfloat) h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();    
    glPixelStorei(GL_UNPACK_ROW_LENGTH,GameScreen.xfull);
}

void draw_opengl() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Current Raster Position always at bottom left hand corner of window
    glRasterPos2i(area_overlay.x, area_overlay.y+area_overlay.h);
    glPixelZoom((GLfloat)area_overlay.w/(GLfloat)GameScreen.xview,
	    -(GLfloat)area_overlay.h/(GLfloat)GameScreen.yview);
    glDrawPixels(GameScreen.xview,GameScreen.yview,GL_RGB,GL_UNSIGNED_SHORT_5_6_5_REV,sdl_game_bitmap->pixels+current_game->video_info->border_size*2*(1+GameScreen.xfull));
    SDL_GL_SwapBuffers();

#ifdef RAINE_DEBUG
    /* Check for error conditions. */
    int gl_error = glGetError( );

    if( gl_error != GL_NO_ERROR ) {
	fprintf( stderr, "draw_opengl: OpenGL error: %d\n", gl_error );
    }

    char *sdl_error = SDL_GetError( );

    if( sdl_error[0] != '\0' ) {
	fprintf(stderr, "draw_opengl: SDL error '%s'\n", sdl_error);
	SDL_ClearError();
    }
#endif
}
