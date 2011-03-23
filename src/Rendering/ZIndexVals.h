#ifndef ZINDEXVALS_H_
#define ZINDEXVALS_H_

/* plik zawierajacy liste wartosci zIndex.
 * Chodzi o to, by wszystkie potwory byly renderowane w jednej warstwie,
 * wszystkie kafle w innej itp.
 *
 * Zapraszamy do dodawania wlasnych :) tylko w kolejnosci, zebysmy sie nie pogubili.
 */

const int Z_MAX          = 100;

const int Z_TILE         = 90;

const int Z_BLOOD_SPLATS = 85;

const int Z_SHADOWS      = 83;

const int Z_DEAD_BODY    = 80;

const int Z_WALL         = 70;
const int Z_DOOR         = 68;

const int Z_LAIR         = 60;
const int Z_LOOT         = 55;
const int Z_MAPSPRITE_BG = 52;
const int Z_PHYSICAL     = 50;
const int Z_OBSTACLE     = 49;
const int Z_PLAYER       = 45;

const int Z_PARTICLE     = 40;

const int Z_MAPSPRITE_FG = 35;

const int Z_EDITOR_WALL	 = 34;

const int Z_PRIMITIVES   = 32;

const int Z_FLOATER_MSG	 = 31;
const int Z_GUI1		 = 28;	/* hud, main menu */
const int Z_GUI2		 = 22;	/* cutscenki, dialogi */
const int Z_GUI3         = 16;	/* ekrany w grze */
const int Z_GUI4		 = 10;	/* popupy */

const int Z_CONSOLE      = 4;

const int Z_CONSOLE_TEXT = 3;

const int Z_DRAGGED_IMG  = 2;
const int Z_CURSOR       = 1;

const int Z_ZERO         = 0;


#endif /*ZINDEXVALS_H_*/
