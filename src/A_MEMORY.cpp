// ****************************************************************
// * PCP                                                          *
// * Copyright (C)1999 Dark Nova Software (DNS)                   *
// * All Rights Reserved.                                         *
// *                                                              *
// * File: a_memory.cpp                                           *
// * Author: vecna                                                *
// * Portability: All systems                                     *
// * Description: Memory allocation stuff.                        *
// *                                                              *
// * Note: When the PARANOID conditional define is enabled,       *
// *       buffers are allocated at the front and end of each     *
// *       memory chunk. (default 256 bytes each) These are       *
// *       initialized to a value (default 254) and at periodic   *
// *       points the program will check to make sure each chunk  *
// *       has not overridden it's bounds. If it has, the program *
// *       will exit, saying which chunk is corrupt, and if its   *
// *       prefix or suffix corruption.                           *
// ****************************************************************

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "a_memory.h"
#include "log.h"
#include "error.h"

// *****
// TODO: Move the chunk list from an array to a linked list?
//       Would eliminate hardcoded chunk limit, but would make
//       general operation slower. Probably not The Right Thing,
//       sides the chunk limit can be interesting sometimes. If
//       it becomes problematic, consider a Binary Tree.
// *****

// ***************************** Data *****************************

#define MAXCHUNKS 200
#define PARANOID 1
#define PADFILLVALUE 254
#define PADSIZE 256

void MemReport(void);

#define errm MemReport(), err

typedef struct
{
   void *pointer;
   int  size;
   int  owner;
   char desc[20];
} memblockType;

memblockType chunks[MAXCHUNKS+1];
int numchunks=0;

// ***************************** Code *****************************

void *valloc(int amount, char *desc, int owner)
{
   if (numchunks == MAXCHUNKS)
     err("Failed allocated %d bytes (%s), reason: Out of chunks.",
        amount, desc);

#ifdef PARANOID
   CheckCorruption();
   chunks[numchunks].pointer = (void *) ((int) malloc(amount + (PADSIZE * 2)) + PADSIZE);
   chunks[numchunks].size = amount;
   memset((char *) chunks[numchunks].pointer - PADSIZE, PADFILLVALUE, PADSIZE);
   memset((char *) chunks[numchunks].pointer +
               chunks[numchunks].size, PADFILLVALUE, PADSIZE);
#else
   chunks[numchunks].pointer = malloc(amount);
   chunks[numchunks].size = amount;
#endif
   chunks[numchunks].owner = owner;
   strncpy(chunks[numchunks].desc, desc, 19);
   memset(chunks[numchunks].pointer, 0, chunks[numchunks].size);
   return chunks[numchunks++].pointer;
}

void *qvalloc(int amount)
{
   void *ptr;

   // Quick and dirty memory allocation. Should be used ONLY
   // for temporary blocks in speed-critical loops.

   ptr = malloc(amount);
   if (!ptr) errm("qvalloc: Failed allocating %d bytes.", amount);
   return ptr;
}

void qvfree(void *ptr)
{
   free(ptr);
}

int TotalBytesAllocated(void)
{
   int i, tally=0;

   for (i=0; i<numchunks; i++)
      tally += chunks[i].size;

   return tally;
}

int FindChunk(void *pointer)
{
   int i;

   for (i=0; i<numchunks; i++)
      if (chunks[i].pointer == pointer) return i;
   return -1;
}

void FreeChunk(int i)
{
#ifdef PARANOID
   CheckCorruption();
   free((void *) ((int) chunks[i].pointer - PADSIZE));
#else
   free(chunks[i].pointer);
#endif
   for (; i<numchunks; i++)
      chunks[i]=chunks[i+1];
   numchunks--;
}

int vfree(void *pointer)
{
   int i;

   i=FindChunk(pointer);
   if (i == -1)
   {
      Log("vfree: Attempted to free ptr %u that was not allocated. [dumping mem report]", pointer);
      MemReport();
      return -1;
   }
   FreeChunk(i);

   return 0;
}

void FreeByOwner(int owner)
{
   int i;

   for (i=0; i<numchunks; i++)
      if (chunks[i].owner == owner)
         FreeChunk(i--);
}

void MemReport(void)
{
   int i;

   Log("");
   Log("========================================");
   Log("= Memory usage report for this session =");
   Log("========================================");
   Log("Chunks currently allocated: %d (MAXCHUNKS %d)", numchunks, MAXCHUNKS);
   Log("%d total bytes allocated. ", TotalBytesAllocated());
#ifdef PARANOID
   Log("PARANOID is ON. (pad size: %d pad value: %d)", PADSIZE, PADFILLVALUE);
#else
   Log("PARANOID is OFF.");
#endif
   Log("");
   Log("Per-chunk analysis: ");

   for (i=0; i<numchunks; i++)
   {
       Log("[%3d] Ptr at: %8u size: %8d owner: %3d desc: %s",
          i, chunks[i].pointer, chunks[i].size, chunks[i].owner, chunks[i].desc);
   }

}

#ifdef PARANOID
int ChunkIntegrity(int i)
{
   char *tptr;

   tptr=(char *) malloc(PADSIZE);
   memset(tptr, PADFILLVALUE, PADSIZE);
   if (memcmp((char *) chunks[i].pointer - PADSIZE, tptr, PADSIZE))
      return -1;      // Prefix corruption
   if (memcmp((char *) chunks[i].pointer + chunks[i].size, tptr, PADSIZE))
      return 1;       // Suffix corruption
   free(tptr);
   return 0;          // no corruption
}

void CheckCorruption(void)
{
   int i, j;

   for (i=0; i<numchunks; i++)
   {
      j=ChunkIntegrity(i);
      if (!j) continue;
      if (j == -1) errm("Prefix corruption on chunk %d.", i);
      if (j ==  1) errm("Suffix corruption on chunk %d.", i);
   }
}
#else
void CheckCorruption(void)
{
   return;
}
#endif
