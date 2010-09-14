#ifndef CONTROLS_INC
#define CONTROLS_INC

namespace undo {

extern int UndoCount(void);

};

extern void PollMovement();
extern void ProcessControls();

#endif // CONTROLS_INC