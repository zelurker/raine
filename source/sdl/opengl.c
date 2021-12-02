#define NOGDI // define this before including windows.h to avoid al_bitmap !
#define GL_GLEXT_PROTOTYPES
#include <SDL.h>
#ifdef ANDROID
#include <GLES/gl.h>
#include <GLES/glext.h>
// #include <GLES2/gl2.h>
#else

#ifdef RAINE_UNIX
// apparently windows doesn't like this !
#define GL_GLEXT_LEGACY // to try not to include glext.h which redefines the GL_GLEXT_VERSION which shouldn't have gone to SDL_opengl.h !
#endif
#include <SDL_opengl.h>
#include <GL/glu.h>

#endif
#undef WINAPI
#include "compat.h"
#include "blit.h"
#include "blit_sdl.h"
#include "games.h"
#include "display_sdl.h"
#include "files.h"
#include "newmem.h"
#include "raine.h" // ReadWord/WriteWord
#include "glsl.h"
#ifdef MessageBox
#undef MessageBox
#endif
#include "dialogs/messagebox.h"
#include "str_opaque.h"
#ifndef GL_PIXEL_UNPACK_BUFFER
#define GL_PIXEL_UNPACK_BUFFER GL_PIXEL_UNPACK_BUFFER_ARB // ???!
#endif

static UINT8 *font;
static int gl_format,gl_type;
static int desk_w,desk_h;

void check_error(char *msg) {
#ifdef RAINE_DEBUG
    // No need to display this in non debug builds
    int gl_error = glGetError( );

    if( gl_error != GL_NO_ERROR ) {
	fprintf( stderr, "%s: OpenGL error: %d\n", msg,gl_error );
    }

    const char *sdl_error = SDL_GetError( );

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

    s = SDL_CreateRGBSurface(SDL_SWSURFACE,desk_w,desk_h,
	    f->BitsPerPixel,f->Rmask,f->Gmask,f->Bmask,f->Amask);

    // Read bits from color buffer
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
#ifndef ANDROID
    // Available in GLES3 only, and for now it's hard to get gles3 !
    glPixelStorei(GL_PACK_ROW_LENGTH, 0);
    glPixelStorei(GL_PACK_SKIP_ROWS, 0);
    glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
#endif

    // Get the current read buffer setting and save it. Switch to
    // the front buffer and do the read operation. Finally, restore
    // the read buffer state
    glReadPixels(0, 0, iViewport[2], iViewport[3],
#ifdef ANDROID
	    (bpp == 32 ? GL_RGBA : GL_RGB),
#else
	    (bpp == 32 ? GL_BGRA : GL_BGR),
#endif
	    GL_UNSIGNED_BYTE, s->pixels);
    save_png_surf_rev(name,s);
}

#if SDL == 2
SDL_GLContext *context;
#endif

void opengl_reshape(int w, int h) {
    /* There is a conflict between rend and context for the use of glDrawPixels and glBitmap
     * These 2 functions produce nothing visible on screen if using any rendering function after the gl context has been created
     * I couldn't find the cause nor restore the normal behavior no matter what I tried.
     * There seems to be only 2 solutions : destroy the renderer or detroy the gl context so they do not conflict
     * I choose to destroy the gl context, so it's destroyed when exiting from here in gui.cpp, after the call to run_game_emulation
     * and recreated here... */
#if SDL == 2
    if (!context) {
	context = SDL_GL_CreateContext(win);
    }
    if (!context) {
	SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GL_CreateContext(): %s\n", SDL_GetError());
	exit(2);
    }
#endif
    desk_w = w; desk_h = h;
    reset_shaders();
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, w, 0.0, h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, w, h);

#ifndef ANDROID
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPixelStorei(GL_UNPACK_ROW_LENGTH,GameScreen.xfull);
    glPixelStorei(GL_UNPACK_LSB_FIRST,0);
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    if (ogl.render == 1) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    }
    if (!ogl.dbuf) {
#if SDL == 2
	SDL_GL_SetSwapInterval(0);
#endif
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, ogl.dbuf );
    } else {
#if SDL == 2
	if (SDL_GL_SetSwapInterval(-1) < 0) {
	    ogl.infos.vbl = 1;
	    SDL_GL_SetSwapInterval(1);
	} else
	    ogl.infos.vbl = -1;
#else
	ogl.infos.vbl = 1;
#endif
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, ogl.dbuf );
    }
    check_error("end opengl_reshape");
#endif
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

static int broken_gl_format = 0;

static const char* myglGetString( GLenum id) {
	const char *s = (const char*)glGetString(id);
	return (s ? s : "");
}

void get_ogl_infos() {
	check_error("start ogl_infos");
	int format_error = 0;
#if SDL==1
	switch (sdl_screen->format->BitsPerPixel) {
#else
        switch(display_cfg.bpp) {
#endif
	case 16:
		gl_format = GL_RGB;
#ifdef ANDROID
		gl_type = GL_UNSIGNED_SHORT_5_6_5;
#else
		gl_type = GL_UNSIGNED_SHORT_5_6_5_REV;
#endif
		break;
	case 32:
#ifdef ANDROID
		// No such flexibility with opengl/es !
		gl_format = GL_RGBA;
		gl_type = GL_UNSIGNED_INT_24_8_OES;
#else
#if SDL == 1
		switch (sdl_screen->format->Bshift) {
		case 0:
			gl_format = GL_BGRA;
			gl_type = GL_UNSIGNED_INT_8_8_8_8_REV;
			break;
		case 16:
			gl_format = GL_RGBA;
			gl_type = GL_UNSIGNED_INT_8_8_8_8_REV;
			break;
		case 24:
			gl_format = GL_BGRA;
			gl_type = GL_UNSIGNED_INT_8_8_8_8;
			break;
		default:
			format_error = 1;
		}
#else
			gl_format = GL_RGBA;
			gl_type = GL_UNSIGNED_INT_8_8_8_8;
#endif
#endif
		break;
	default:
		format_error = 1;
	}
	if (format_error && !broken_gl_format) {
		char buff[1024];
		broken_gl_format = 1;
		sprintf(buff,_("bad screen format, report this to rainemu.com:\n"
				"bpp : %d rshift %d gshift %d bshift %d"),
				sdl_screen->format->BitsPerPixel,
				sdl_screen->format->Rshift,
				sdl_screen->format->Gshift,
				sdl_screen->format->Bshift);
		MessageBox(gettext("OpenGL error"),buff,gettext("ok"));
	}

	ogl.info = 1;
	// Slight optimization
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
#ifndef ANDROID
	glDisable(GL_POLYGON_SMOOTH);
#endif
	glDisable(GL_STENCIL_TEST);
	if (ogl.render == 1)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);



	if (ogl.render == 1 && strcmp(ogl.shader,"None")) {
		read_shader(ogl.shader);
	}
	if (ogl.vendor) {
		free(ogl.vendor);
		free(ogl.renderer);
		free(ogl.version);
	}
	ogl.vendor = strdup( (char*)myglGetString( GL_VENDOR ) );
	ogl.renderer = strdup( (char*)myglGetString( GL_RENDERER ) );
	ogl.version = strdup( (char*)myglGetString( GL_VERSION ) );
	GetAttribute( SDL_GL_DOUBLEBUFFER, &ogl.infos.dbuf );
	GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &ogl.infos.fsaa_buffers );
	GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &ogl.infos.fsaa_samples );
	GetAttribute( SDL_GL_ACCELERATED_VISUAL, &ogl.infos.accel );
#if SDL==2
	ogl.infos.vbl = SDL_GL_GetSwapInterval();
#else
	GetAttribute( SDL_GL_SWAP_CONTROL, &ogl.infos.vbl );
#endif
	check_error("end ogl_infos");
}

void render_texture(int linear) {
    // glPushMatrix();
    // glTranslated((area_overlay.x+area_overlay.w-1),0,0);
    // glRotatef(90.0,0.0,0.0,1.0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear);
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGB,
		GameScreen.xview,GameScreen.yview,0,gl_format,
		gl_type,
		GameViewBitmap->line[0]);

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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (ogl.render == 1) {
	draw_shader(linear);
    } else {
	// Current Raster Position always at bottom left hand corner of window
	glPixelZoom((GLfloat)area_overlay.w/(GLfloat)GameScreen.xview,
		-(GLfloat)area_overlay.h/(GLfloat)GameScreen.yview);
	glRasterPos2i(area_overlay.x, area_overlay.y+area_overlay.h-1);
	glDrawPixels(GameScreen.xview,GameScreen.yview,gl_format,gl_type,GameViewBitmap->line[0]);
    }
}

static UINT8 *opaque_bmp;

void opengl_text(char *msg, int x, int y) {
    // Display the text using an opengl bitmap based on an sdl_gfx font
    // the coordinates are in characters - 0,0 being bottom left now
    // -x is right alignment
    // -y is top alignment
    // and -1000,-1000 is center text

    // static GLuint opaque_buff,gl_font;
    if (!font) {
	char *name= get_shared("fonts/10x20.fnt");
	UINT32 size = size_file(name);
	if (!size) {
	    printf("couldn't find font %s\n",name);
	    return;
	}
	font = malloc(size);
	load_file(name,font,size);
	opaque_bmp = malloc(20*2*80);
	memset(opaque_bmp,0xff,20*2*80);
	/*
	glGenBuffers(1,&opaque_buff);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER,opaque_buff);
	glBufferData(GL_PIXEL_UNPACK_BUFFER,20*2*80,opaque_bmp,GL_STATIC_DRAW);
	*/
	int i,j;
	char tmp[20*2];
	for (i=0; i<256; i++) {
	    for (j=0; j<20; j++) {
		WriteWord(&tmp[(19-j)*2],ReadWord(&font[(i*20+j)*2]));
	    }
	    memcpy(&font[i*20*2],tmp,20*2);
	}
	/*
	glGenBuffers(1,&gl_font);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER,gl_font);
	glBufferData(GL_PIXEL_UNPACK_BUFFER,20*2*256,font,GL_STATIC_DRAW);
	*/
    }

    if (x == -1000 && y == -1000) { // center
	x = (desk_w-strlen(msg)*10)/2;
	y = (desk_h-20)/2;
    } else {
	if (x<0)
	    x = desk_w+x*10;
	else
	    x *= 10;
	if (y < 0)
	    y = desk_h+y*20;
	else
	    y *= 20;
    }
    glRasterPos2i(x,y);

    glPixelStorei(GL_UNPACK_ROW_LENGTH,2);
    glPixelStorei(GL_UNPACK_ALIGNMENT,2);
    glPixelStorei(GL_UNPACK_LSB_FIRST,0);
    glDisable(GL_TEXTURE_2D);
    if (opaque_hud) {
	// glColor3f(0.0f,1.0f,0.0f);
	// glBindBuffer(GL_PIXEL_UNPACK_BUFFER,opaque_buff);
	glDrawPixels(10*strlen(msg) ,20, GL_COLOR_INDEX,GL_UNSIGNED_BYTE, opaque_bmp);
    }

    glColor3f(1.0f,1.0f,1.0f);
    // glBindBuffer(GL_PIXEL_UNPACK_BUFFER,gl_font);
    // GLubyte *zero = 0;
    while (*msg) {
	glBitmap(10, 20, 0.0, 0.0, 10.0, 0.0, font+*msg*20*2);
	msg++;
    }
    // glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0);
    glPixelStorei(GL_UNPACK_ROW_LENGTH,GameScreen.xfull);
    glPixelStorei(GL_UNPACK_ALIGNMENT,4);
    glEnable(GL_TEXTURE_2D);
}

// Called at the end of a frame
void finish_opengl() {
    if (ogl.dbuf) {
#if SDL==2
	SDL_GL_SwapWindow(win);
#else
	SDL_GL_SwapBuffers();
#endif
    } else
#if SDL==2
	// Paradox yeah, apparently glFlush displays nothing
	// and the good way is to use this, eventually disabling double buffer first
	SDL_GL_SwapWindow(win);
#else
	glFlush();
#endif
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
