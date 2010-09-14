/*
Copyright (C) 1998 BJ Eirich (aka vecna)
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public Lic
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <string.h>
#include <malloc.h>

#include "mode13h.h"
#include "vdriver.h"

// ================================= Code ====================================


void SetMode(int mode)
{
	if (mode==0x13)
		InitVideo(320, 240, 120, false);
	else
		InitVideo(640, 480, 120, false);
}

int Mode13hShutdown()
{
  return AllegroShutdown();
}

int Mode13hShowPage()
{
	return AllegroShowPage();
}

int Mode13hCopySprite(int x, int y, int width, int height, BITMAP* src)
{
	return AllegroCopySprite(x, y, width ,height, src);
}

int Mode13hCCopySprite(int x,int y,int width,int height,BITMAP* src)
{
	return AllegroCopySprite(x, y, width, height, src);
}

int Mode13hTCCopySprite(int x,int y,int width,int height,BITMAP* src)
{
	return AllegroTCopySprite(x, y, width, height, src);
}

int Mode13hTCopySprite(int x, int y, int width, int height, BITMAP* src)
{
	return AllegroTCopySprite(x, y, width, height, src);
}

int Mode13hCopyTile(int x, int y, BITMAP* src)
{
	return AllegroCopySprite(x, y, 16, 16, src);
}

int Mode13hVLine(int x, int y, int length, int color)
{
	return AllegroVLine(x, y, length, color);
}

int Mode13hHLine(int x, int y, int width, int color)
{
	return AllegroHLine(x, y, width, color);
}

int Mode13hFilledBox(int x, int y, int width, int height, int color)
{
	return AllegroFilledBox(x, y, width, height, color);
}

int Mode13hColorGrid(int x, int y, int color)
{
	return AllegroColorGrid(x, y, color);
}

int Mode13hColorGridSize(int x, int y, int width, int height, int color)
{
	return AllegroColorGridSize(x, y, width, height, color);
}

int Mode13hClearScreen()
{
	return AllegroClearScreen();
}

int Mode13hTCopySpriteLucent(int x, int y, int width, int height, BITMAP* src)
{
	return AllegroTCopySpriteLucent(x, y, width, height, src);
}

void InitMode13h()
{
  SetMode(0x13);
}
