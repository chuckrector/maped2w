/* Tile Editor Header
 * (C)1998 Richard Lau
 */

#ifndef TILEED_INC
#define TILEED_INC

// TEtype defs
#define TEUNKNOWN 0
#define TETILE    1

// external functions
extern void TileEdit(void);

// external variables
extern unsigned int TEtype;    // type of graphic being edited
extern unsigned int TEtile; // tile to edit - may possibly expand this to an array for simultaneous editing
extern unsigned int TExsize;   // width of graphic being edited
extern unsigned int TEysize;   // height of graphic being edited
extern BITMAP *TEsource; // pointer to original image being edited

#endif
