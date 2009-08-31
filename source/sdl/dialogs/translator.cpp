#include "../gui/menu.h"
#include "raine.h"
#include "blit.h"
#include "palette.h"
#include "newspr.h"
#include "translator.h"
#include "sdl/gui/tbitmap.h"
#include "sdl/blit_sdl.h"
#include "sdl/dialogs/messagebox.h"
#include "neocd/neocd.h"
#include "neocd/cdrom.h"
#include "neocd/cache.h"
#include "files.h"
#include "starhelp.h" // byteswap
#if HAS_CONSOLE
#include "sdl/console/parser.h"
#endif

static int cursorx, cursory,len; // these must stay out of a class because
// of the draw function (pure C). Not sure it was a good idea to use this
// function as C...

class TTransBitmap : public TBitmap {
  protected:
    UINT8 *font;
    UINT16 *map;
    int size_map, used_map;
    int sprites_changed;
  public:
    TTransBitmap(menu_item_t *my_menu);
    ~TTransBitmap();
    virtual int handle_key(SDL_Event *event);
    void put_char(UINT8 *gfx, UINT8 *font);
};

TTransBitmap::TTransBitmap(menu_item_t *my_menu) : TBitmap(my_menu)
{ 
  cursorx = cursory = 0;
  memset(&pal[8],0,sizeof(SDL_Color)*8); // clear parasite colors in font
  FILE *f = fopen(get_shared("fonts/8x16.fnt"),"rb");
  sprites_changed = 0;
  if (!f) {
    ErrorMsg("no 8x16 font available !!!");
    font = NULL;
    map = NULL;
    return;
  }
  fseek(f,0L,SEEK_END);
  int taille = ftell(f);
  font = (UINT8*)malloc(taille);
  fseek(f,0L,SEEK_SET);
  fread(font,1,taille,f);
  fclose(f);
  map = (UINT16*)malloc(0xff*2);
  size_map = 255;
  used_map = 0;
  f = fopen(get_shared("savedata/ssrpg.map"),"rb");
  memset(map,0,255*2);
  if (f) {
    fread(map,1,0xff*2,f);
    fclose(f);
    while (used_map < size_map && map[used_map])
      used_map++;
    printf("used_map %d\n",used_map);
  } else
    memset(map,0,255*2);
}

static UINT32 base,offset,end;

TTransBitmap::~TTransBitmap() {
  if (font) free(font);
  if (map) {
    char path[1024];
    sprintf(path,"%ssavedata/ssrpg.map",dir_cfg.exe_path);
    FILE *f = fopen(path,"wb");
    if (f) {
      fwrite(map,1,0xff*2,f);
      fclose(f);
    } else
      printf("could not save map\n");
  }
  if (sprites_changed) {
    char path[1024];
    sprintf(path,"%soverride",dir_cfg.exe_path);
    mkdir_rwx(path);
    sprintf(path,"%soverride/combos.spr",dir_cfg.exe_path);
    save_file(path,&GFX[0x20100],0xff00);
    char *name;
    int nb;
    get_cache_origin(PRG_TYPE,base+offset,&name,&nb);
    printf("got name %s for origin %x base %x\n",name,base+offset,base);
    put_override(PRG_TYPE,name);
  }
}

void TTransBitmap::put_char(UINT8 *gfx, UINT8 *font) {
  int i,j;
  sprites_changed = 1;
  for (i=0;i<16;i++){
    for (j=7;j>=0;j--){
      if (font[i] & (1<<j))
	gfx[7-j] = 6;
      else
	gfx[7-j] = GFX[0x30100+7-j+j*16];
    }
    gfx += 16;
  }
}

class TTransMenu : public TMenu {
  public:
  TTransMenu(char *my_title, menu_item_t *mymenu) : TMenu(my_title,mymenu)
  { }
    virtual void create_child(int n);
};

void TTransMenu::create_child(int n) {
  if (menu[n].values_list_size == ITEM_TBITMAP) {
    child[n] = new TTransBitmap(&menu[n]);
  } else
    TMenu::create_child(n);
}

static TMenu *sprite_menu;

static UINT16 validchars[256];
static char str_offset[15];

#define OFFS_SPRITES 0xe1d6

int TTransBitmap::handle_key(SDL_Event *event) {
  switch (event->type) {
    case SDL_KEYDOWN:
      int sym = event->key.keysym.sym;
      switch (sym) {
	case SDLK_RIGHT:
	  if (cursory*20+(cursorx+1)<len) {
	    cursorx++;
	    if (cursorx == 20) {
	      cursorx = 0; cursory++;
	    }
	    sprintf(str_offset,"$%x",base+offset+(cursory*20+cursorx)*2);
	    sprite_menu->draw();
	    return 1;
	  }
	  break;
	case SDLK_LEFT:
	  if (cursory*20+(cursorx-1)>=0) {
	    cursorx--;
	    if (cursorx < 0) {
	      cursorx = 19; cursory--;
	    }
	    sprintf(str_offset,"$%x",base+offset+(cursory*20+cursorx)*2);
	    sprite_menu->draw();
	    return 1;
	  }
	  break;
	case SDLK_DOWN:
	  if ((cursory+1)*20+(cursorx)<len) {
	    cursory++;
	    sprintf(str_offset,"$%x",base+offset+(cursory*20+cursorx)*2);
	    sprite_menu->draw();
	    return 1;
	  }
	  break;
	case SDLK_UP:
	  if ((cursory-1)*20+(cursorx)>=0) {
	    cursory--;
	    sprintf(str_offset,"$%x",base+offset+(cursory*20+cursorx)*2);
	    sprite_menu->draw();
	    return 1;
	  }
	  break;
	case SDLK_RETURN:
	case SDLK_PAGEDOWN:
	  WriteWord(&RAM[base+offset+(cursorx+20*cursory)*2],
	      0); // line break
	  sprites_changed = 1;
	  event->key.keysym.sym = SDLK_RIGHT;
	  return handle_key(event);
	case SDLK_END:
	  WriteWord(&RAM[base+offset+(cursorx+20*cursory)*2],
	      0xffff); // end of text
	  sprites_changed = 1;
	  event->key.keysym.sym = SDLK_LEFT; // text ends earlier now
	  return handle_key(event);
	default:
	  int unicode = event->key.keysym.unicode;
	  if (unicode >= ' ' && unicode < 256 && validchars[unicode]) {
	    UINT8 *offs = &RAM[base+offset+(cursorx+20*cursory)*2];
	    UINT16 prevchar = ReadWord(offs-2)+OFFS_SPRITES;
	    if (cursory*20+(cursorx+1)==len && prevchar < 0x300) {
	      prevchar = ReadWord(offs)+OFFS_SPRITES; // exception for the very last char
	      offs += 2;
	    }
	    if ((cursorx > 0 || cursory > 0) && ((prevchar >= 0x301 && prevchar <= 0x410))) {
	      // 1 get prevchar code
	      int n;
	      for (n= 32; n<127; n++) {
		if (validchars[n] == prevchar) {
		  prevchar = n;
		  break;
		}
	      }
	      if (prevchar < 127) {
		int combi = (prevchar<<8) | unicode;
		sprites_changed = 1;
		for (n=0; n<used_map; n++) {
		  if (map[n] == combi) {
		    WriteWord(offs-2,0x201+n-OFFS_SPRITES);
		    return 1;
		  }
		}
		if (used_map < size_map) {
		  map[used_map] = combi;
		  video_spr_usage[0x201+used_map]=1;
		  put_char(&GFX[(0x201+used_map)*0x100],&font[prevchar*16+4]);
		  put_char(&GFX[(0x201+used_map)*0x100+8],&font[unicode*16+4]);
		  WriteWord(offs-2,0x201+used_map-OFFS_SPRITES);
		  used_map++;
		  return 1;
		}
	      }
	    }
	    if (ReadWord(offs) == 0xffff)
	      offs -= 2; // very last char fix
	    WriteWord(offs,
		validchars[unicode]-OFFS_SPRITES);
	    event->key.keysym.sym = SDLK_RIGHT;
	    return handle_key(event);
	  }
      }
      break;
  }
  return 0;
}

static int draw_translator(int sel) {
  if (bitmap_color_depth(GameBitmap) != 8) {
    sprite_menu->draw();
    return 0;
  }
  clear_game_screen(0);
  ClearPaletteMap();
  UINT8 map[16];

  int x = 0, y = 0;
  int n;
  for (n=0; n<16; n++)
    map[n] = n; // direct mapping
  n = 0;
  UINT16 spriteno;
  int cx = cursorx*16;
  int cy = cursory*16;
  while ((spriteno = ReadWord(&RAM[base+offset+n])) != 0xffff) {
    // printf("%d,%d,%x (%x+%x+%x)\n",x,y,spriteno,base,offset,n);
    if (!spriteno)
      spriteno = 0x400; // note (line break)
    else
      spriteno += OFFS_SPRITES;
    // printf("%d,%d,%x\n",x,y,spriteno);
    if (spriteno < 0x8000)
      Draw16x16_Trans_Mapped(&GFX[(spriteno)<<8],x,y,map);
    if (x == cx && y == cy) {
      int cx,cy;
      for (cx=0; cx<16; cx++) {
	for (cy=0; cy<16; cy++) {
          UINT8 *px = &GameBitmap->line[y+cy][x+cx];
	  if (*px < 8) // ssrpg fonts use "parasite" colors >= 8
	    *px ^= 255;
	  else
	    *px = 255;
	}
      }
    }
    x += 16;
    if (x+16 > GameBitmap->w) {
      x = 0;
      y += 16;
      if (y+16 > GameBitmap->h) break;
    }
    n+=2;
  }
  len = n/2;

  return 0;
}

static UINT32 location[] = { 0x72014, 0x73aca, // intro
  0x60ff8, 0x62b32, // ingame (prog.prg)
  0x7b39a, 0x1fffff}; // unknown

static int base_no;

static int set_location(int sel) {
  base = location[base_no*2];
  end = location[base_no*2+1];
  offset = 0;
  sprintf(str_offset,"offset %x",base+offset);
  cursorx = cursory = 0;
  if (sprite_menu) sprite_menu->draw();
  return 0;
}

int set_offset(int sel) {
  // go to next avail offset
  while (base+offset < end && ReadWord(&RAM[base+offset]) != 0xffff)
    offset += 2;
  offset += 2;
  if (base+offset > end)
    offset = 0;
  sprintf(str_offset,"offset %x",base+offset);
  cursorx = cursory = 0;
  if (sprite_menu) sprite_menu->draw();
  return 0;
}

static menu_item_t offset_menu[] =
{
  { "Enter offset (hex)", NULL, &base_no, ITEM_EDIT, { 10,0 }, { str_offset } },
  { NULL, NULL, NULL }
};

static int dlg_offset(int sel) {
  sprintf(str_offset,"$%x",base+offset);
  TMenu *menu = new TMenu("Offset",offset_menu);
  menu->execute();
  delete menu;
  if (str_offset[0] != '$' && strncmp(str_offset,"0x",2)) {
    memmove(&str_offset[1],&str_offset[0],10);
    str_offset[0] = '$';
  }
#ifdef HAS_CONSOLE
  UINT32 offs = parse(str_offset);
#else
  UINT32 offs;
  if (str_offset[0] == '$')
    sscanf(&str_offset[1],"%x",&offs);
  else if (!strncmp(str_offset,"0x",2))
    sscanf(&str_offset[2],"%x",&offs);
  else
    offs = atoi(str_offset);
#endif
  if (ReadWord(&RAM[offs-2]) == 0xffff) {
    offset = offs;
    if (offset >= location[2] && offset < location[3]) {
      offset -= location[2];
      base = location[2];
      base_no = 1;
    } else if (offset >= location[0] && offset < location[1]) {
      offset -= location[0];
      base = location[0];
      base_no = 0;
    } else {
      base = 0;
      base_no = 2;
    }
    sprintf(str_offset,"$%x",offs);
    sprite_menu->draw();
  } else
    ErrorMsg("No end of string mark just before this offset");
  return 0;
}

static menu_item_t sprites_menu[] =
{
  { "location", &set_location, &base_no, 2, { 0, 1, 2}, { "intro texts", "ingame texts", "unknown" }}, 
  { str_offset, &set_offset, NULL },
  { "", &draw_translator, &base_no, ITEM_TBITMAP, { 320, 240, 8 } },
  { "Set offset...", &dlg_offset },
  { NULL, NULL, NULL }
};

int do_translate(int sel) {
  memset(validchars,0,sizeof(validchars));
  int n;
  for (n='0'; n<='9'; n++)
    validchars[n] = 0x3a9+n-'0';
  for (n='A'; n<='Z'; n++)
    validchars[n] = 0x3b3+n-'A';
  for (n='a'; n<='z'; n++)
    validchars[n] = 0x3cd+n-'a';
  validchars[' '] = 0x301;
  validchars[39] = 0x403; // ' doesn't exist in the font, but we need it!
  validchars[34] = 0x404;
  validchars[','] = 0x405;
  validchars[';'] = 0x405;
  validchars['!'] = 0x401;
  validchars['?'] = 0x402;
  validchars['.'] = 0x403;
  validchars['('] = 0x407;
  validchars[')'] = 0x408;
  validchars['['] = 0x409;
  validchars[']'] = 0x40a;
  validchars['/'] = 0x40b;
  validchars[':'] = 0x40e;
  validchars['+'] = 0x40f;
  validchars['-'] = 0x410;
  /*validchars['é'] = validchars['è'] = validchars['ê'] = validchars['ë'] =
    0x3cd+'e'-'a';
    validchars['ù'] = 0x3cd+'u'-'a';
    validchars['â'] = validchars['à'] = 0x3cd+'a'-'a'; */
  set_location(0);
  sprite_menu = new TTransMenu("Translator",sprites_menu);
  sprite_menu->execute();
  delete sprite_menu;
  sprite_menu = NULL;
  return 0;
}

/* TTransSS2 : same thing, but for SS2 (samurai shodown 2).
 * This time it uses the fix layer so it has a real 8x16 font (made of 2 fix
 * sprites on top of each other). The print function reads a code on a word,
 * puts this word as the value of the first sprite, then puts this code+1 just
 * under the 1st one to make the 8x16 char. 
 * Since there is already a font available this time, the code will be easier */

class TTransSS2 : public TTransBitmap {
  public:
    TTransSS2(menu_item_t *my_menu) : TTransBitmap(my_menu)
  {
  cursorx = cursory = 0;
  sprites_changed = 0;
  font = NULL;
  map = NULL;
  }
    virtual int handle_key(SDL_Event *event);
};

int TTransSS2::handle_key(SDL_Event *event) {
  switch (event->type) {
    case SDL_KEYDOWN:
      int sym = event->key.keysym.sym;
      switch (sym) {
	case SDLK_RIGHT:
	case SDLK_LEFT:
	case SDLK_DOWN:
	case SDLK_UP:
	  return TTransBitmap::handle_key(event);
	case SDLK_RETURN:
	case SDLK_PAGEDOWN:
	  WriteWord(&RAM[base+offset+(cursorx+20*cursory)*2],
	      0x1000); // line break (0x1000 or 0x2000 ???)
	  event->key.keysym.sym = SDLK_RIGHT;
	  return handle_key(event);
	case SDLK_END:
	  WriteWord(&RAM[base+offset+(cursorx+20*cursory)*2],
	      0x0000); // end of text (not sure yet this one is usefull)
	  sprites_changed = 1;
	  event->key.keysym.sym = SDLK_LEFT; // text ends earlier now
	  return handle_key(event);
	default:
	  int unicode = event->key.keysym.unicode;
	  if (unicode >= ' ' && unicode < 256 && validchars[unicode]) {
	    UINT8 *offs = &RAM[base+offset+(cursorx+20*cursory)*2];
	    if (ReadWord(offs) == 0x0)
	      offs -= 2; // very last char fix
	    WriteWord(offs,
		validchars[unicode]);
	    event->key.keysym.sym = SDLK_RIGHT;
	    return handle_key(event);
	  }
      }
      break;
  }
  return 0;
}

class TTransSS2Menu : public TMenu {
  public:
  TTransSS2Menu(char *my_title, menu_item_t *mymenu) : TMenu(my_title,mymenu)
  { }
    virtual void create_child(int n);
};

void TTransSS2Menu::create_child(int n) {
  if (menu[n].values_list_size == ITEM_TBITMAP) {
    child[n] = new TTransSS2(&menu[n]);
  } else
    TMenu::create_child(n);
}

static int draw_translator_ss2(int sel) {
  if (bitmap_color_depth(GameBitmap) != 8) {
    sprite_menu->draw();
    return 0;
  }
  clear_game_screen(0);
  ClearPaletteMap();
  UINT8 map[16];

  int x = 0, y = 0;
  int n;
  for (n=0; n<16; n++)
    map[n] = n; // direct mapping
  n = 0;
  UINT16 spriteno;
  int cx = cursorx*16;
  int cy = cursory*16;
  while ((spriteno = ReadWord(&RAM[base+offset+n])) != 0x0000) {
    // printf("%d,%d,%x (%x+%x+%x)\n",x,y,spriteno,base,offset,n);
    if (!spriteno)
      spriteno = 0x400; // note (line break)
    // printf("%d,%d,%x\n",x,y,spriteno);
    if (spriteno < 0x1000) {
      Draw8x8_Trans_Mapped(&neogeo_fix_memory[(spriteno)<<6],x,y,map);
      Draw8x8_Trans_Mapped(&neogeo_fix_memory[(spriteno+1)<<6],x,y,map);
    }
    if (x == cx && y == cy) {
      int cx,cy;
      for (cx=0; cx<8; cx++) {
	for (cy=0; cy<16; cy++) {
          UINT8 *px = &GameBitmap->line[y+cy][x+cx];
	  if (*px < 8) // ssrpg fonts use "parasite" colors >= 8
	    *px ^= 255;
	  else
	    *px = 255;
	}
      }
    }
    x += 8;
    if (x+8 > GameBitmap->w) {
      x = 0;
      y += 16;
      if (y+16 > GameBitmap->h) break;
    }
    n+=2;
  }
  len = n/2;

  return 0;
}

int do_translate_ss2(int sel) {
  memset(validchars,0,sizeof(validchars));
  int n;
  for (n='A'; n<='Z'; n++)
    validchars[n] = 0x7440+n-'A';
  for (n='a'; n<='z'; n++)
    validchars[n] = 0x7440+n-'a'; // no lower case chars apparently...
  validchars[' '] = 0x74fc;
  validchars['!'] = 0x7478;
  validchars['?'] = 0x74b2;
  validchars['.'] = 0x2000;
  /*validchars['é'] = validchars['è'] = validchars['ê'] = validchars['ë'] =
    0x3cd+'e'-'a';
    validchars['ù'] = 0x3cd+'u'-'a';
    validchars['â'] = validchars['à'] = 0x3cd+'a'-'a'; */
  set_location(0);
  sprite_menu = new TTransSS2Menu("Translator",sprites_menu);
  sprite_menu->execute();
  delete sprite_menu;
  sprite_menu = NULL;
  return 0;
}

