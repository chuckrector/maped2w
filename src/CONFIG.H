
#ifndef CONFIG_INC
#define CONFIG_INC

extern int vm;
extern char *strbuf;
extern int white,black,darkred,pad,th,mh;
extern int winbg,titlebg,brightw,darkw;

extern void InitializeDefaults();
extern void GetConfig(char *fname);

#endif // CONFIG_INC
