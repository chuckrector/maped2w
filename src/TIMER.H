
#ifndef TIMER_INC
#define TIMER_INC

extern volatile unsigned int systemtime;
extern volatile unsigned int timer_count;
extern volatile unsigned int tick;
extern volatile int pixel_scroll_timer;
extern volatile int tile_scroll_timer;
extern volatile int backupct;
extern volatile int idlect;
extern volatile int xz;
extern volatile int yz;
extern volatile int xzd;
extern volatile int yzd;
extern volatile int ssaver_enabled;

//extern int (*callback) (void); // ??? <aen, apr 21>

extern void SetHZ(unsigned int hz);
extern void RestoreHZ();
extern void InitTimer();
extern void ShutdownTimer();

#endif // TIMER_INC
