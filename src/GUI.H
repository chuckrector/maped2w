
#ifndef GUI_INC
#define GUI_INC

#include "allegro.h"

extern int amxofs, amyofs;
extern int mmxofs, mmyofs;
extern int cnxofs, cnyofs;
extern int lmxofs, lmyofs;
extern int lvxofs, lvyofs;
extern int vaxofs, vayofs;
extern int mpxofs, mpyofs;
extern int zexofs, zeyofs;
extern int exofs, eyofs;
extern int prxofs, pryofs;
extern int rsxofs, rsyofs;

extern void CheckMessageExpirations();
extern void Message(char *text, int duration);
extern void DrawScreenControls();
extern void RenderMessages();
extern void RenderGUI();
extern void DoMainMenu();
//extern void MainMenu(int dx, int dy);
extern void newMainMenu(int x, int y);
extern unsigned char TileCmp(BITMAP *one, BITMAP *two);
extern void ImageToLayer();
extern void Destroyer();
extern void LayersSubMenu(int dx, int dy);
extern void ExecuteSubMenu(int dx, int dy);
extern void AddLayer();
extern void EditLayerProperties();
extern void NewMAP();
extern void About();
extern int  Confirm(char *message);
extern void LoadMAPDialog();
extern void LoadVSPDialog();
extern void MPDialog();
extern void ZoneEdDialog();
extern void EntityEditor(int i);
extern void CHRList();
extern void MoveScriptDialog();
extern void MapPropertiesDialog();
extern void ResizeMapDialog();
extern void CDDialog();
extern void NoticeDialog();
extern void OptionsDialog();
extern void VCNotify(char *msg);

#endif // GUI_INC
