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
#include "sdl/dialogs/messagebox.h"

#ifdef RAINE_UNIX
  #include <GL/glx.h>
  #define glGetProcAddress(name) (*glXGetProcAddress)((const GLubyte*)(name))
#elif defined(RAINE_WIN32)
//  With sdl glext.h doesn't need to be included here, and it's a good thing
//  because it has disappeared from the i686-w64-mingw32-gcc files
//  #include <GL/glaux.h>
  #define glGetProcAddress(name) wglGetProcAddress(name)
#else
  #error "OpenGL: unsupported platform"
#endif

static UINT8 *font;
static int shader_support;
static GLuint vertexshader; // only one
static int nb_pass; // number of passes for the shader
#define MAX_PASS 10
typedef struct {
    GLuint glprogram, fragmentshader;
    int sizex,sizey,filter;
    float outscalex,outscaley;
} tpass;

tpass pass[MAX_PASS];

static PFNGLCREATEPROGRAMPROC glCreateProgram = 0;
static PFNGLVALIDATEPROGRAMPROC glValidateProgram = 0;
static PFNGLDELETEPROGRAMPROC glDeleteProgram = 0;
static PFNGLUSEPROGRAMPROC glUseProgram = 0;
static PFNGLCREATESHADERPROC glCreateShader = 0;
static PFNGLDELETESHADERPROC glDeleteShader = 0;
static PFNGLSHADERSOURCEPROC glShaderSource = 0;
static PFNGLCOMPILESHADERPROC glCompileShader = 0;
static PFNGLGETSHADERIVPROC glGetShaderiv = 0;
static PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = 0;
static PFNGLATTACHSHADERPROC glAttachShader = 0;
static PFNGLDETACHSHADERPROC glDetachShader = 0;
static PFNGLLINKPROGRAMPROC glLinkProgram = 0;
static PFNGLGETPROGRAMIVPROC glGetProgramiv = 0;
static PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = 0;
static PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = 0;
static PFNGLUNIFORM1IPROC glUniform1i = 0;
static PFNGLUNIFORM2FVPROC glUniform2fv = 0;
static PFNGLUNIFORM4FVPROC glUniform4fv = 0;

#ifdef RAINE_DEBUG
static void check_error(char *msg) {
    int gl_error = glGetError( );

    if( gl_error != GL_NO_ERROR ) {
	fprintf( stderr, "%s: OpenGL error: %d\n", msg,gl_error );
    }

    char *sdl_error = SDL_GetError( );

    if( sdl_error[0] != '\0' ) {
	fprintf(stderr, "%s: SDL error '%s'\n",msg, sdl_error);
	SDL_ClearError();
    }
}
#endif

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

static char* getstr(char *s, char *what) {
    char *p = strstr(s,what);
    if (p) p += strlen(what);
    return p;
}

static char *arg, *value;

static char* getarg(char *p) {
    arg = value = NULL;
    while (*p < 33) p++;
    if (*p < 'a' || *p > 'z') return p;
    char *e = strchr(p,'=');
    if (!e) return p;
    arg = p;
    *e = 0;
    p = e+1;
    if (*p == '"') { // quote
	p++;
	e = strchr(p,'"');
	if (!e) return p;
	*e = 0;
	value = p;
	p = e+1;
    } else { // don't even know if something can go without quotes in xml...
	e = p;
	while (*p != ' ' && *p)
	    p++;
	if (*p) {
	    *p = 0;
	    p++;
	}
	value = e;
    }
    return p;
}

static char *my_load_file(char *shader) {
    int size = size_file(shader);
    if (!size) {
	printf("shader: couldn't find file %s\n",shader);
	return NULL;
    }
    char *buf = malloc(size);
    if (!buf) {
	printf("shader: couldn't allocate %d bytes\n",size);
	return NULL;
    }
    if (!load_file(shader,(unsigned char*)buf,size)) {
	printf("shader: couldn't load %s\n",shader);
	free(buf);
	return NULL;
    }
    return buf;
}

static char *process_cdata(char *p) {
    while (*p < 32) p++;
    value = p;
    p = getstr(p,"]]>");
    p[-3] = 0;
    return p;
}

static int mystrcmp(char **s, char *cmp) {
    int len = strlen(cmp);
    int ret = strncmp(*s,cmp,len);
    if (!ret) *s += len;
    return ret;
}

static int attach(GLuint shader) {
    int gl_error;
    if (!pass[nb_pass].glprogram) {
	pass[nb_pass].glprogram = glCreateProgram();
	print_debug("pass %d glprogram created %d\n",nb_pass,pass[nb_pass].glprogram);
    }
    if (vertexshader && vertexshader != shader) {
	print_debug("attaching vertex shader to program %d\n",pass[nb_pass].glprogram);
	gl_error = glGetError( );
	glAttachShader(pass[nb_pass].glprogram, vertexshader);
	gl_error = glGetError( );
	if( gl_error != GL_NO_ERROR ) {
	    strcpy(ogl.shader,"None");
	    MessageBox("OpenGL error","error while attaching vertex shader","ok");
	    return 0;
	}
    }
    print_debug("attaching shader %d to program %d\n",shader,pass[nb_pass].glprogram);
    gl_error = glGetError( );
    glAttachShader(pass[nb_pass].glprogram, shader);
    gl_error = glGetError( );
    if( gl_error != GL_NO_ERROR ) {
	strcpy(ogl.shader,"None");
	MessageBox("OpenGL error","error while attaching fragment shader","ok");
	return 0;
    }
    return 1;
}

static int set_fragment_shader(const char *source) {
    int ret = 1;
    GLuint fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentshader, 1, &source, 0);
    glCompileShader(fragmentshader);

    GLint tmp;
    glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &tmp);
    if (!tmp) {
	glGetShaderiv(fragmentshader, GL_INFO_LOG_LENGTH, &tmp);
	GLchar *buf = malloc(tmp);
	glGetShaderInfoLog(fragmentshader, tmp, NULL, buf);
	strcpy(ogl.shader,"None");
	MessageBox("Errors compiling fragment shader",buf,"ok");
	free(buf);
	ret = 0;
    } else 
	ret = attach(fragmentshader);
    pass[nb_pass].fragmentshader = fragmentshader;
    return ret;
}

static int set_vertex_shader(const char *source) {
    int ret = 1;
    vertexshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexshader, 1, &source, 0);
    glCompileShader(vertexshader);

    GLint tmp;
    glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &tmp);
    if (!tmp) {
	glGetShaderiv(vertexshader, GL_INFO_LOG_LENGTH, &tmp);
	GLchar *buf = malloc(tmp);
	glGetShaderInfoLog(vertexshader, tmp, NULL, buf);
	printf("Errors compiling vertex shader: %s\n", buf);
	free(buf);
	ret = 0;
    } else {
	/* After seeing an example of a v1.2 xml shader, it seems the vertex
	 * shader can be totally at the end of the file, so I guess that it
	 * needs to be attached to all created programs in this case ! */
	int nb = nb_pass;
	for (nb_pass=0; nb_pass <= nb; nb_pass++)
	    ret &= attach(vertexshader);
	nb_pass = nb;
    }
    return ret;
}

static void delete_shaders() {
    int n;
    for (n=0; n<=nb_pass; n++) {
	if (pass[n].glprogram) {
	    if (pass[n].fragmentshader) {
		glDetachShader(pass[n].glprogram, pass[n].fragmentshader);
		glDeleteShader(pass[n].fragmentshader);
	    }
	    if (vertexshader) glDetachShader(pass[n].glprogram,vertexshader);
	    glDeleteProgram(pass[n].glprogram);
	    pass[n].fragmentshader = pass[n].glprogram = 0;
	}
	pass[n].sizex = pass[n].sizey = pass[n].filter = 0;
	pass[n].outscalex = pass[n].outscaley = 0.0;
    }
    if (vertexshader) glDeleteShader(vertexshader);
    nb_pass = 0;
}

static void read_shader(char *shader) {
    /* The idea is to try to make the code as readable as possible (it's
     * about string manipulation in C, so there won't be any miracle !), but
     * I'll have a separate block of code for each tag, and since there are
     * only shader, vertex and fragment here, it should do... 
     *
     * Also I saw somewhere on the web that sometimes they use src instead of
     * cdata even though it's not part of 1.1 specification, but I added
     * support for it anyway... */

    // A fast exit define because it happens a lot...
#define flee \
    if (frag_used_src) free(frag_src);    \
    if (vertex_used_src) free(vertex_src);\
    free(buf);                            \
    return;

    char *vertex_src = NULL, *frag_src = NULL;
    char *buf = my_load_file(shader);
    int vertex_used_src = 0, frag_used_src = 0;

    delete_shaders();
	
    if (!buf)
	return;
    char *p = getstr(buf,"<shader");
    if (!p) {
	printf("no shader in file %s\n",shader);
	flee;
    }
    while (*p == ' ') {
	p = getarg(p);
	if (strcmp(arg,"language"))
	    printf("unknown shader argument : %s=%s\n",arg,value);
	else if (strcmp(value,"GLSL")) {
	    printf("shader language is not GLSL, bye !\n");
	    flee;
	}
    }
    while ((p = getstr(p,"<"))) {
	if (!mystrcmp(&p,"!--")) // comment
	    p = getstr(p,"-->");


	else if (!mystrcmp(&p,"vertex")) {
	    if (vertex_src) {
		printf("shader: 2 vertex codes found in this shader, aborting\n");
		flee;
	    }
	    while (*p == ' ') {
		/* No argument expected for vertex shader, but I'll process
		 * src anyway */
		p = getarg(p);
		if (!strcmp(arg,"src")) {
		    vertex_src = my_load_file(value);
		    if (!vertex_src) {
			printf("shader: couldn't load vertex shader from src %s\n",value);
			flee;
		    }
		    vertex_used_src = 1;
		} else
		    printf("shader: unknown vertex argument : %s=%s\n",arg,value);
	    }
	    p = getstr(p,"<");
	    if (!mystrcmp(&p,"![CDATA[")) {
		if (vertex_src) {
		    printf("shader: 2 vertex declarations\n");
		    flee;
		}
		p = process_cdata(p);
		vertex_src = value;
	    } else if (mystrcmp(&p,"/vertex")) {
		printf("unexpected tag in vertex : %s\n",p);
	    }
	    if (!vertex_src) {
		printf("shader: parsing of vertex tag failed\n");
		flee;
	    }
	    if (!set_vertex_shader(vertex_src)) {
		delete_shaders();
		flee;
	    }
	    p = getstr(p,"/vertex>");


	} else if (!mystrcmp(&p,"fragment")) {
	    if (pass[nb_pass].fragmentshader)
		printf("shader: pass %d\n",++nb_pass);
	    while (*p == ' ') {
		if (frag_used_src) {
		    free(frag_src);
		    frag_used_src = 0;
		}
		p = getarg(p);
		if (!strcmp(arg,"src")) {
		    frag_src = my_load_file(value);
		    if (!frag_src) {
			printf("shader: couldn't load fragment shader from src %s\n",value);
			flee;
		    }
		    frag_used_src = 1;
		} else if (!strcmp(arg,"filter")) {
		    if (!strcmp(value,"linear"))
			pass[nb_pass].filter = 1;
		    else if (!strcmp(value,"nearest"))
			pass[nb_pass].filter = 2;
		    else {
			printf("filter value unknown : %s\n",value);
			pass[nb_pass].filter = 0;
		    }
		} else if (!strcmp(arg,"size")) {
		    pass[nb_pass].sizex = pass[nb_pass].sizey = atoi(value);
		} else if (!strcmp(arg,"size_x"))
		    pass[nb_pass].sizex = atoi(value);
		else if (!strcmp(arg,"size_y"))
		    pass[nb_pass].sizey = atoi(value);
		else if (!strcmp(arg,"scale")) {
		    pass[nb_pass].sizex = GameScreen.xview*atof(value);
		    pass[nb_pass].sizey = GameScreen.yview*atof(value);
		} else if (!strcmp(arg,"scale_x"))
		    pass[nb_pass].sizex = GameScreen.xview*atof(value);
		else if (!strcmp(arg,"scale_y"))
		    pass[nb_pass].sizey = GameScreen.yview*atof(value);
		else if (!strcmp(arg,"outscale")) {
		    pass[nb_pass].outscalex = atof(value);
		    pass[nb_pass].outscaley = atof(value);
		} else if (!strcmp(arg,"outscale_x"))
		    pass[nb_pass].outscalex = atof(value);
		else if (!strcmp(arg,"outscale_y"))
		    pass[nb_pass].outscaley = atof(value);
		else
		    printf("shader: unknown fragment argument : %s=%s\n",arg,value);
	    }
	    p = getstr(p,"<");
	    if (!mystrcmp(&p,"![CDATA[")) {
		if (frag_src && frag_used_src) {
		    free(frag_src);
		    frag_used_src = 0;
		}
		p = process_cdata(p);
		frag_src = value;
	    } else if (mystrcmp(&p,"/fragment")) {
		printf("unexpected tag in vertex : %s\n",p);
	    }
	    if (!frag_src) {
		printf("shader: parsing of fragment tag failed\n");
		flee;
	    }
	    if (!set_fragment_shader(frag_src)) {
		delete_shaders();
		flee;
	    }
	    p = getstr(p,"/fragment>");
	} else if (!mystrcmp(&p,"/shader>")) {
	    int n;
	    printf("end of shader\n");

	    for (n=0; n<=nb_pass; n++) {
		glLinkProgram(pass[n].glprogram);

		GLint tmp;
		glGetProgramiv(pass[n].glprogram, GL_LINK_STATUS, &tmp);
		if (!tmp) {
		    glGetProgramiv(pass[n].glprogram, GL_INFO_LOG_LENGTH, &tmp);
		    GLchar *buf = malloc(tmp);
		    glGetProgramInfoLog(pass[n].glprogram, tmp, NULL, buf);
		    delete_shaders();
		    strcpy(ogl.shader,"None");
		    MessageBox("Error linking shader",buf,"OK");
		    free(buf);
		} else if (glValidateProgram) {
		    glValidateProgram(pass[n].glprogram);
		    GLint tmp;
		    glGetProgramiv(pass[n].glprogram, GL_INFO_LOG_LENGTH, &tmp);
		    if (!tmp) {
			glGetProgramiv(pass[n].glprogram, GL_INFO_LOG_LENGTH, &tmp);
			GLchar *buf = malloc(tmp);
			glGetProgramInfoLog(pass[n].glprogram, tmp, NULL, buf);
			printf("Errors validating shader program %d: %s\n", n,buf);
			free(buf);
		    } else
			printf("validation glprogram %d pass %d ok\n",pass[n].glprogram,n);
		} else
		    printf("impossible to validate shader, no validation function\n");
	    }

	    flee;
	} else {
	    char *e = p;
	    p = getstr(p,">");
	    p[-1] = 0;
	    printf("shader: unknown tag %s\n",e);
	}
    }

    printf("shader: something went wrong, missing end shader declaration ?\n");
    flee;
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

void get_ogl_infos() {
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


    //bind shader functions
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)glGetProcAddress("glCreateProgram");
    glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)glGetProcAddress("glValidateProgram");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)glGetProcAddress("glDeleteProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)glGetProcAddress("glUseProgram");
    glCreateShader = (PFNGLCREATESHADERPROC)glGetProcAddress("glCreateShader");
    glDeleteShader = (PFNGLDELETESHADERPROC)glGetProcAddress("glDeleteShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)glGetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)glGetProcAddress("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)glGetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glGetProcAddress("glGetShaderInfoLog");
    glAttachShader = (PFNGLATTACHSHADERPROC)glGetProcAddress("glAttachShader");
    glDetachShader = (PFNGLDETACHSHADERPROC)glGetProcAddress("glDetachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)glGetProcAddress("glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glGetProcAddress("glGetProgramInfoLog");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glGetProcAddress("glGetUniformLocation");
    glUniform1i = (PFNGLUNIFORM1IPROC)glGetProcAddress("glUniform1i");
    glUniform2fv = (PFNGLUNIFORM2FVPROC)glGetProcAddress("glUniform2fv");
    glUniform4fv = (PFNGLUNIFORM4FVPROC)glGetProcAddress("glUniform4fv");

    shader_support = glCreateProgram && glUseProgram && glCreateShader
    && glDeleteShader && glShaderSource && glCompileShader && glGetShaderiv
    && glGetShaderInfoLog && glAttachShader && glDetachShader && glLinkProgram
    && glGetProgramiv && glGetProgramInfoLog && glGetUniformLocation
    && glUniform1i && glUniform2fv && glUniform4fv;

    if (shader_support && ogl.render == 1 && strcmp(ogl.shader,"None")) {
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
    SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &ogl.infos.dbuf );
    SDL_GL_GetAttribute( SDL_GL_MULTISAMPLEBUFFERS, &ogl.infos.fsaa_buffers );
    SDL_GL_GetAttribute( SDL_GL_MULTISAMPLESAMPLES, &ogl.infos.fsaa_samples );
    SDL_GL_GetAttribute( SDL_GL_ACCELERATED_VISUAL, &ogl.infos.accel );
    SDL_GL_GetAttribute( SDL_GL_SWAP_CONTROL, &ogl.infos.vbl );
}

static void render_texture(int linear) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear);
    glTexImage2D(GL_TEXTURE_2D,0, GL_RGB,
	    GameScreen.xview,GameScreen.yview,0,GL_RGB,
	    GL_UNSIGNED_SHORT_5_6_5_REV,
	    sdl_game_bitmap->pixels+current_game->video_info->border_size*2*(1+GameScreen.xfull));


    glBegin(GL_QUADS);
    glNormal3f(0,0,1.0);
    glTexCoord2f(0.0,0.0);
    glVertex3d(area_overlay.x, area_overlay.y+area_overlay.h-1,0);
    glTexCoord2f(1.0,0.0);
    glVertex3d(area_overlay.x+area_overlay.w-1, area_overlay.y+area_overlay.h-1,0);
    glTexCoord2f(1.0,1.0);
    glVertex3d(area_overlay.x+area_overlay.w-1, area_overlay.y,0);
    glTexCoord2f(0.0,1.0);
    glVertex3d(area_overlay.x, area_overlay.y,0);
    glEnd();
}

void draw_opengl(int linear) {
    glClear(GL_COLOR_BUFFER_BIT);

    if (ogl.render == 1) {
	if (shader_support && pass[0].glprogram) {

	    int n;
	    for (n=0; n<=nb_pass; n++) {

		GLuint glprogram = pass[n].glprogram;
		glUseProgram(glprogram);
		GLint location;

		float inputSize[2] = { (float)GameScreen.xview, (float)GameScreen.yview };
#if 0
		if (n > 0) {
		    inputSize[0] = (float)area_overlay.w;
		    inputSize[1] = (float)area_overlay.h;
		}
#endif
		location = glGetUniformLocation(glprogram, "rubyInputSize");
		if (location > -1)
		    glUniform2fv(location, 1, inputSize);

		float outputSize[2] = { (float)area_overlay.w, (float)area_overlay.h };
		location = glGetUniformLocation(glprogram, "rubyOutputSize");
		if (location > -1)
		    glUniform2fv(location, 1, outputSize);

		// This one is supposed to be >= GameScreen.[xy]view
		// I guess it's in case you decide to use hq2/3x on GameScreen
		// to generate the texture, but I don't think I'll want to do
		// that (loss of cycles everywhere !)
		float textureSize[2] = { (float)GameScreen.xview, (float)GameScreen.yview };
#if 0
		if (n > 0) {
		    textureSize[0] = (float)area_overlay.w;
		    textureSize[1] = (float)area_overlay.h;
		}
#endif
		location = glGetUniformLocation(glprogram, "rubyTextureSize");
		if (location > -1)
		    glUniform2fv(location, 1, textureSize);

		if (pass[n].filter == 2) // explicit nearest
		    linear = GL_NEAREST;
		else
		    linear = GL_LINEAR; // default = linear
		/* Notice : for now all the size/scale/outscale parameters are
		 * simply ignored, it's because I didn't find any shader using
		 * them until now */
		render_texture(linear);
	    }
	    glUseProgram(0); // all shaders off now
	    return;
	} else
	    printf("shader support %d 1st program %d\n",shader_support,pass[0].glprogram);
	if (linear) 
	    linear = GL_LINEAR;
	else
	    linear = GL_NEAREST;
	render_texture(linear);
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
