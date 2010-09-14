#ifndef BROWSE_INC
#define BROWSE_INC

#include <cstring>

extern char cwd_prefix[1024];

extern int bxsize;
extern int bysize;
extern int bxofs;
extern int byofs;
extern int bmode;

struct Entry {
	Entry* next;
	char name[1024];
	Entry(char* name): next(0) {
		if (name!=0) {
			int len=strlen(name);
			if (len>1023) len=1023;
			memcpy(this->name,name,len+1);
		}
	}
};

struct EntryList {
	Entry* head;
	Entry* tail;
	int count;
	EntryList(): head(0), tail(0), count(0) {}
	void add(Entry* entry) {
		//Log("start add()");
		if (tail==0) {
			head=entry;
		} else {
			tail->next=entry;
		}
		tail=entry;
		++count;
		//Log("end add()");
	}
// returns null on bad index
	Entry* get(int index) {
		//Log("start get()");
		if (index<0) return 0;
		Entry* iter=head;
		for (; iter!=0 && --index>=0; iter=iter->next) {
			if (iter->next==0) {
				return 0;
			}
		}
		//Log("end get()");
		return iter;
	}
	void free() {
		//Log("start free()");
		Entry* iter=head;
		while (iter!=0) {
			Entry* next=iter->next;
			delete iter;
			iter=next;
		}
		//Log("end free()");
	}
};

extern void Browse(char n, char *m1, char *m2, char *m3, char *m4, char *dest);

#endif // BROWSE_INC