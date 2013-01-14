#include <SDL.h>
#include <SDL_opengl.h>
#include "sdl/compat.h"
#include "blit.h"
#include "blit_sdl.h"
#include "games.h"
#include "sdl/display_sdl.h"
#include "files.h"
#include "newmem.h"
#include "raine.h"

static UINT8 *font;

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

void get_ogl_infos() {
    ogl.info = 1;
    // Slight optimization
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    if (ogl.vendor) {
	free(ogl.vendor);
	free(ogl.renderer);
	free(ogl.version);
    }
    ogl.vendor = strdup( (char*)glGetString( GL_VENDOR ) );
    ogl.renderer = strdup( (char*)glGetString( GL_RENDERER ) );
    ogl.version = strdup( (char*)glGetString( GL_VERSION ) );
    SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &ogl.infos.dbuf );
    SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &ogl.infos.fsaa_buffers );
    SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &ogl.infos.fsaa_samples );
    SDL_GL_GetAttribute( SDL_GL_ACCELERATED_VISUAL, &ogl.infos.accel );
    SDL_GL_GetAttribute( SDL_GL_SWAP_CONTROL, &ogl.infos.vbl );
}

void draw_opengl() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Current Raster Position always at bottom left hand corner of window
    glRasterPos2i(area_overlay.x, area_overlay.y+area_overlay.h-1);
    glPixelZoom((GLfloat)area_overlay.w/(GLfloat)GameScreen.xview,
	    -(GLfloat)area_overlay.h/(GLfloat)GameScreen.yview);
    glDrawPixels(GameScreen.xview,GameScreen.yview,GL_RGB,GL_UNSIGNED_SHORT_5_6_5_REV,sdl_game_bitmap->pixels+current_game->video_info->border_size*2*(1+GameScreen.xfull));

}

void opengl_text(char *msg, int x, int y) {
    // Display the text using an opengl bitmap based on an sdl_gfx font
    // the coordinates are in characters - 0,0 being bottom left now
    // -x is right alignment
    // -y is top alignment
    // and -1000,-1000 is center text

    if (!font) {
	char *name= get_shared("fonts/10x20.fnt");
	UINT32 size = size_file(name);
	font = malloc(size);
	load_file(name,font,size);
	int i,j;
	char tmp[20*2];
	// Maybe the font should also be unpacked to 4 bytes boundary ?
	// Not sure it would make a big difference...
	for (i=0; i<256; i++) {
	    for (j=0; j<20; j++) {
		WriteWord(&tmp[(19-j)*2],ReadWord(&font[(i*20+j)*2]));
	    }
	    memcpy(&font[i*20*2],tmp,20*2);
	}
    }

    glColor3f(1.0f,1.0f,1.0f);
    if (x == -1000 && y == -1000) { // center
	x = (sdl_screen->w-strlen(msg)*10)/2;
	y = (sdl_screen->h-20)/2;
    } else {
	if (x<0)
	    x = sdl_screen->w+x*10;
	else
	    x *= 10;
	if (y < 0) 
	    y = sdl_screen->h+y*20;
	else
	    y *= 20;
    }
    glRasterPos2i(x,y);

    glPixelStorei(GL_UNPACK_ROW_LENGTH,2);
    glPixelStorei(GL_UNPACK_ALIGNMENT,2);
    while (*msg) {
	glBitmap(10, 20, 0.0, 0.0, 10.0, 0.0, font+*msg*20*2);
	msg++;
    }
    glPixelStorei(GL_UNPACK_ROW_LENGTH,GameScreen.xfull);
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
}

void finish_opengl() {
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

void opengl_done() {
    if (font) free(font);
}
