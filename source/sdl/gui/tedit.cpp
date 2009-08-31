#include "tedit.h"
#include "sdl/SDL_gfx/SDL_gfxPrimitives.h"

void TEdit::disp(SDL_Surface *s, TFont *myfont, int x, int y, int w,int h,
  int fg, int bg, int xoptions) {
  font = myfont;
  if (*menu->label) {
    TStatic::disp(s,myfont,x,y,w,h,fg,bg,xoptions);
    w -= xoptions - x;
    x = xoptions;
  }
  rectangleColor(s,x,y,x+w-1,y+h-1,mymakecol(255,255,255));
  if (field[0])
    font->surf_string(s,x+1,y+1,field,fg_color,0);
  myx = x; myy = y; myh = h; mys = s;
  if (bg && cursor_on) {
    update_cursor();
  }
}

void TEdit::update_cursor() {
  int w;
  if (old_pos != pos) {
    char buff[1024];
    strncpy(buff,field,pos);
    buff[pos] = 0;
    int bidon;
    if (pos > 0)
      font->dimensions(buff,&w,&bidon);
    else
      w = 0;
    pos_cursor = w;
    old_pos = pos;
  } else
    w = pos_cursor;
  w += myx+1;
  // boxColor(s,w,myy+1,w+4,myy+h-1,mymakecol(255,0,255));

  disp_cursor(mys,w,myy,8,myh);
}

int TEdit::get_height(TFont *font) {
  int w,h;
  font->dimensions("3",&w,&h);
  return h+2;
}

void TEdit::update() {
  if (update_count++ & 32) {
    if (cursor_on) {
      cursor_on = 0;
      update_cursor();
    }
  } else {
    if (!cursor_on) {
      cursor_on = 1;
      update_cursor();
    }
  }
}

int TEdit::get_width(TFont *font) {
  int w,h;
  font->dimensions("W",&w,&h);
  return w*maxl+2;
}

int TEdit::handle_key(SDL_Event *event) {
  unsigned int oldpos = pos;
  switch (event->type) {
    case SDL_KEYDOWN:
      int sym = event->key.keysym.sym;
      if (sym != SDLK_UP && sym != SDLK_DOWN)
	current_hist = -1; // reset history position
      switch(sym) {
	case SDLK_HOME: pos = 0; break;
	case SDLK_END:  pos = strlen(field); break;
	case SDLK_DELETE: if (pos < strlen(field)) {
	  memmove(&field[pos],&field[pos+1],strlen(field)-pos);
	}
	break;
	case SDLK_BACKSPACE: if (pos > 0) {
	  memmove(&field[pos-1],&field[pos],strlen(field)-pos+1);
	  pos--;
	}
	break;
	case SDLK_LEFT:
	  if (pos) pos--;
	  break;
	case SDLK_RIGHT:
	  if (pos < strlen(field)) pos++;
	  break;
	case SDLK_UP:
	case SDLK_DOWN:
	{
	  if (!use_hist) return 0;
	  if (used_hist <= 0) break;
	  if (current_hist == -1) { // start to browse history
            strncpy(prefix_hist,field,80);
	    prefix_hist[79] = 0;
	  }
	  int len = strlen(prefix_hist);
	  int direction = (sym == SDLK_UP ? 1 : -1);
	  int loop = 0;
	  do {
	    current_hist += direction;
	    if (current_hist >= used_hist) {
	      current_hist = 0;
	      loop++;
	    } else if (current_hist < 0) {
	      current_hist = used_hist -1;
	      loop++;
	    }
	  } while (strncasecmp(history[current_hist],prefix_hist,len) && loop<2);
	  if (loop == 2) printf("%c",7); // bell
	  else {
	    strcpy(field,history[current_hist]);
	    pos = strlen(field);
	  }
	  break;
	}
	case SDLK_RETURN:
	case SDLK_KP_ENTER:
	  pos = 0;
	  current_hist = -1;
	  add_history();
	  return 1; // call the handler !!!
	default:
	  int unicode = event->key.keysym.unicode;
	  if (((sym >= ' ' && sym < 255) || (sym == 0 && unicode)) && pos < maxl) {
	    memmove(&field[pos+1],&field[pos],maxl-pos);
	    if (unicode) sym = unicode;
	    field[pos++] = sym;
	  } else
	    return 0;
      }
  }
  if (cursor_on) {
    if (oldpos != pos) {
      int tmp = pos;
      pos = oldpos;
      update_cursor();
      pos = tmp;
      cursor_on = 0;
    }
  }
  update_count = 0; // cursor visible, now !
  return -1;
}

void TEdit::add_history() {
  if (!use_hist) return;
  if (used_hist && !strcmp(history[0],field))
    return; // don't store repeated commands !!!
  if (used_hist == MAX_HISTORY) {
    used_hist--;
    if (history[used_hist])
      free(history[used_hist]);
  }
  if (used_hist) 
    memmove(&history[1],&history[0],used_hist*sizeof(char*));
  history[0] = strdup(field);
  used_hist++;
}

void TEdit::insert(char *s) {
  int len = strlen(s);
  if (strlen(field) + len > maxl) {
    printf("%c",7);
    return;
  }
  memmove(&field[pos+len],&field[pos],strlen(&field[pos])+1);
  memcpy(&field[pos],s,len);
  pos += len;
}



