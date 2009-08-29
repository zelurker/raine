/******************************************************************************/
/*                                                                            */
/*                               RAINE GAME LISTS                             */
/*                                                                            */
/******************************************************************************/

#include "raine.h"
#include "games.h"

struct GAME_MAIN *current_game;
int game_count;

int is_current_game(char *main_name)
{
   return (!stricmp(current_game->main_name, main_name));
}


