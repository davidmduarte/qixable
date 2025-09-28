#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include "menu.h"
#include "constants.h"
#include "raylib.h"

Menu MenuInit() {
	Menu self;

	self.init = 1;

	self.selectedCat = 0;

	DIR *folder;
	struct dirent *entry;

	int cnt = 0;

	folder = opendir("./assets/categories");
	if(folder != NULL) {
	    while( (entry = readdir(folder)) && cnt < 10) {
	    	if(entry->d_name[0] != '.') {
	    		strcpy(self.cats[cnt], entry->d_name);
	    		cnt++;
	    	}
	    }
	}
	closedir(folder);

	self.catsLen = cnt++;

	self.selectedOp = 0;
	sprintf(self.ops[0], "Start Game");
	sprintf(self.ops[1], "Categories: %s", self.cats[0]);
	sprintf(self.ops[2], "Score Board");
	sprintf(self.ops[3], "Credits");
	sprintf(self.ops[4], "Exit");

	return self;
}

int MenuRender(Menu *self) {
	int scene, i, px, py, px2, py2;
	Color c;

	scene = 1;

	px = (int)(SCREEN_WIDTH / 3);
	py = (int)(SCREEN_WIDTH / 6);

	ClearBackground(BLACK);
	DrawText("Qixable", px, py, 50, YELLOW);

	for(i = 0; i < 5; i++) {
		c = LIGHTGRAY;
		if(self->selectedOp == i) c = VIOLET;
		DrawText(self->ops[i], px + 50, py + 150 + (i * 80), 35, c);
	}

	if(self->init == 1) {
		if(IsKeyPressed(KEY_UP)) {
			if(self->selectedOp > 0) self->selectedOp--;

		} else if(IsKeyPressed(KEY_DOWN)) {
			if(self->selectedOp < 4) self->selectedOp++;

		} else if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
			if(self->selectedOp == 1) {
				self->init = 2;
			} else scene = 2 + self->selectedOp;
		}
	} else {
		px2 = px + 50;
		py2 = py + 290;

		DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 0, 0, 0, 150 });
		DrawRectangle(px2, py2, 300, 130, WHITE);

		for(i = 0; i < self->catsLen; i++) {
			c = DARKGRAY;
			if(self->selectedCat == i) c = VIOLET;
			DrawText(self->cats[i], px2 + 25, py2 + 25 + (i * 50), 30, c);
		}

		if(IsKeyPressed(KEY_UP)) {
			if(self->selectedCat > 0) self->selectedCat--;

		} else if(IsKeyPressed(KEY_DOWN)) {
			if(self->selectedCat < self->catsLen - 1) self->selectedCat++;

		} else if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
			sprintf(self->ops[1], "Categories: %s", self->cats[self->selectedCat]);
			self->init = 1;
		}
	}

	return scene;
}
