#include <SDL.h>
#include <SDL_opengl.h>
#include "sdl/compat.h"
#include "blit.h"
#include "blit_sdl.h"
#include "games.h"
#include "sdl/display_sdl.h"
#include "files.h"
#include "newmem.h"
#include "raine.h" // ReadWord/WriteWord
#include "sdl/glsl.h"

static UINT8 *font;

void check_error(char *msg) {
#ifdef RAINE_DEBUG
    // No need to display this in non debug builds
    int gl_error = glGetError( );

    if( gl_error != GL_NO_ERROR ) {
	fprintf( stderr, "%s: OpenGL error: %d\n", msg,gl_error );
    }

    char *sdl_error = SDL_GetError( );

    if( sdl_error[0] != '\0' ) {
	fprintf(stderr, "%s: SDL error '%s'\n",msg, sdl_error);
	SDL_ClearError();
    }
#endif
}

void ogl_save_png(char *name) {
    // unsigned long lImageSize;   // Size in bytes of image
    GLint iViewport[4];         // Viewport in pixels
    SDL_Surface *s;
    int bpp = sdl_screen->format->BitsPerPixel;
    SDL_PixelFormat *f = sdl_screen->format;

    // Get the viewport dimensions
    glGetIntegerv(GL_VIEWPORT, iViewport);

    // How big is the image going to be (targas are tightly packed)
    // lImageSize = iViewport[2] * 3 * iViewport[3];	

    s = SDL_CreateRGBSurface(SDL_SWSURFACE,sdl_screen->w,sdl_screen->h,
	    f->BitsPerPixel,f->Rmask,f->Gmask,f->Bmask,f->Amask);

    // Read bits from color buffer
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

    // Get the current read buffer setting and save it. Switch to
    // the front buffer and do the read operation. Finally, restore
    // the read buffer state
    glReadPixels(0, 0, iViewport[2], iViewport[3], (bpp == 32 ? GL_BGRA : GL_BGR), GL_UNSIGNED_BYTE, s->pixels);
    save_png_surf_rev(name,s);
}

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
    if (ogl.render == 1) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    }
}

static int GetAttribute(int attr, int *value) {
    int ret = SDL_GL_GetAttribute(attr,value);
    if (ret < 0) {
	// Some attributes unsupported on some system (broken i945GM !)
	// so just clear the opengl error in this case
	glGetError( );
	SDL_ClearError();
    }
    return ret;
}

void get_ogl_infos() {
    check_error("start ogl_infos");
    ogl.info = 1;
    // Slight optimization
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_POLYGON_SMOOTH);
    glDisable(GL_STENCIL_TEST);
    if (ogl.render == 1)
	glEnable(GL_TEXTURE_2D);
    else
	glDisable(GL_TEXTURE_2D);



    if (ogl.render == 1 && strcmp(ogl.shader,"None")) {
	printf("read_shader...\n");
	read_shader(ogl.shader);
    }
    if (ogl.vendor) {
	free(ogl.vendor);
	free(ogl.renderer);
	free(ogl.version);
    }
    ogl.vendor = strdup( (char*)glGetString( GL_VENDOR ) );
    ogl.renderer = strdup( (char*)glGetString( GL_RENDERER ) );
    ogl.version = strdup( (char*)glGetString( GL_VERSION ) );
    GetAttribute( SDL_GL_DOUBLEBUFFER, &ogl.infos.dbuf );
    GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &ogl.infos.fsaa_buffers );
    GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &ogl.infos.fsaa_samples );
    GetAttribute( SDL_GL_ACCELERATED_VISUAL, &ogl.infos.accel );
    GetAttribute( SDL_GL_SWAP_CONTROL, &ogl.infos.vbl );
    check_error("end ogl_infos");
}

void render_texture(int linear) {
    // glPushMatrix();
    // glTranslated((area_overlay.x+area_overlay.w-1),0,0);
    // glRotatef(90.0,0.0,0.0,1.0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear);
    glTexImage2D(GL_TEXTURE_2D,0, GL_RGB,
	    GameScreen.xview,GameScreen.yview,0,GL_RGB,
	    GL_UNSIGNED_SHORT_5_6_5_REV,
	    sdl_game_bitmap->pixels+current_game->video_info->border_size*2*(1+GameScreen.xfull));


    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f(0,0,1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3d(area_overlay.x, area_overlay.y+area_overlay.h-1,0);
    glTexCoord2f(1.0,0.0);
    glVertex3d(area_overlay.x+area_overlay.w-1, area_overlay.y+area_overlay.h-1,0);
    glTexCoord2f(0.0,1.0);
    glVertex3d(area_overlay.x, area_overlay.y,0);
    glTexCoord2f(1.0,1.0);
    glVertex3d(area_overlay.x+area_overlay.w-1, area_overlay.y,0);
    glEnd();
    // glPopMatrix();
}

void draw_opengl(int linear) {
    glClear(GL_COLOR_BUFFER_BIT);

    if (ogl.render == 1) {
	draw_shader(linear);
    } else {
	// Current Raster Position always at bottom left hand corner of window
	glRasterPos2i(area_overlay.x, area_overlay.y+area_overlay.h-1);
	glPixelZoom((GLfloat)area_overlay.w/(GLfloat)GameScreen.xview,
		-(GLfloat)area_overlay.h/(GLfloat)GameScreen.yview);
	glDrawPixels(GameScreen.xview,GameScreen.yview,GL_RGB,GL_UNSIGNED_SHORT_5_6_5_REV,sdl_game_bitmap->pixels+current_game->video_info->border_size*2*(1+GameScreen.xfull));
    }
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
	if (!size) {
	    printf("couldn't find font %s\n",name);
	    return;
	}
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
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f,1.0f,1.0f);
    while (*msg) {
	glBitmap(10, 20, 0.0, 0.0, 10.0, 0.0, font+*msg*20*2);
	msg++;
    }
    glPixelStorei(GL_UNPACK_ROW_LENGTH,GameScreen.xfull);
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    glEnable(GL_TEXTURE_2D);
}

// Called at the end of a frame
void finish_opengl() {
    if (ogl.dbuf)
	SDL_GL_SwapBuffers();
    else
	glFlush();
#ifdef RAINE_DEBUG
    /* Check for error conditions. */
    check_error("finish_opengl");
#endif
}

// Called when sdl_done is called
void opengl_done() {
    if (font) free(font);
    delete_shaders();
}
