#ifndef __MENU__
#define __MENU__

typedef struct {
	int init;
	int selectedOp;
	char ops[5][50];
	int selectedCat;
	char cats[10][50];
	int catsLen;
} Menu;

Menu MenuInit();
int MenuRender(Menu *self);

#endif
