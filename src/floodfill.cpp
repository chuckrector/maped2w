/////////////////////////////////////
// nb: this implementation assumes the region to be filled is not bounded
// by the edge of the screen, but is bounded on all sides by pixels of
// the specified boundary color.  This exercise is left to the student...

// pixel setter and getter
void SetPixel(int x, int y, Color c);
Color GetPixel(int x, int y);
// this algorithm uses a global stack of pixel coordinates
void pushSeed(int x, int y);
bool popSeed(int *x, int *y); // returns false iff stack was empty


// the set it up and kick it off routine.
void SeedFill(int x, int y, Color bound, Color fill)
{
   // we assume the stack is created empty, and that no other
   // routines or threads are using this stack for anything
   pushSeed(x, y);
   FillSeedsOnStack(bound, fill);
}

// the main routine
void FillSeedsOnStack(Color bound, Color fill)
{
   Color col1, col2;
   int x, y;              // current seed pixel
   int xLeft, xRight;     // current span boundary locations
   int i;

   while (popSeed(&x, &y)) {
      if (GetPixel(x, y) != bound) {
         FillContiguousSpan(x, y, bound, fill, &xLeft, &xRight);

         // single pixel spans handled as a special case in the else clause
         if (xLeft != xRight) {
            // handle the row above you
            y++;
            for(i=xLeft+1; i<=xRight; i++) {
               col1 = GetPixel(i-1, y);
               col2 = GetPixel(i, y);
               if (col1 != bound && col1 != fill && col2 == bound)
                  pushSeed(i-1, y);
            }
            if (col2 != bound && col2 != fill)
               pushSeed(xRight, y);

            // handle the row below you
            y -= 2;
            for(i=xLeft+1; i<=xRight; i++) {
               col1 = GetPixel(i-1, y);
               col2 = GetPixel(i, y);
               if (col1 != bound && col1 != fill && col2 == bound)
                  pushSeed(i-1, y);
            }
            if (col2 != bound && col2 != fill)
               pushSeed(xRight, y);
         } else {
            col1 = GetPixel(xLeft, y+1);
            col2 = GetPixel(xLeft, y-1);
            if (col1 != fill)
               pushSeed(xLeft, y+1);
            if (col2 != fill)
               pushSeed(xLeft, y-1);
         }

      } // end if (GetPixel)
   }  // end while (popSeed)
}


// fill pixels to the left and right of the seed pixel until you hit
// boundary pixels.  Return the locations of the leftmost and rightmost
// filled pixels.
void FillContiguousSpan(int x, int y, Color bound, Color fill, int *xLeft, int *xRight)
{
   Color col;
   int i;

   // fill pixels to the right until you reach a boundary pixel
   i = x;
   col = GetPixel(i, y);
   while(col != bound) {
      SetPixel(i, y, fill);
      i++;
      col = GetPixel(i, y);
   }
   *xRight = i-1;

   // fill pixels to the left until you reach a boundary pixel
   i = x-1;
   col = GetPixel(i, y);
   while(col != bound) {
      SetPixel(i, y, fill);
      i--;
      col = GetPixel(i, y);
   }
   *xLeft = i+1;
}


