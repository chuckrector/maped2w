#ifndef A_MEMORY_INC
#define A_MEMORY_INC

extern void *valloc(int amount, char *desc, int owner);
extern void *qvalloc(int amount);
extern void qvfree(void *ptr);
extern int TotalBytesAllocated(void);
extern int FindChunk(void *pointer);
extern void FreeChunk(int i);
extern int vfree(void *pointer);
extern void FreeByOwner(int owner);
extern void MemReport(void);
extern int ChunkIntegrity(int i);
extern void CheckCorruption(void);

#endif // A_MEMORY_INC