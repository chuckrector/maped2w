
#ifndef COLUMNS_INC
#define COLUMNS_INC

extern void BlitPlayField();
extern void BlitPlayFieldwDestruct(char flash);
extern void BlitPiece();
extern void GenerateNewPiece();
extern void DestroyBlock(int x, int y);
extern void ClearConsecutiveColumns();
extern void NextTurn();
extern int CanGoLeft();
extern int CanGoRight();
extern void RotatePiece();
extern void ProcessLevels();
extern void LoadBackground();
extern void Columns();

#endif // COLUMNS_INC
