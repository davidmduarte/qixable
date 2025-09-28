#include <stdio.h>
#include "menu.h"
#include "constants.h"
#include "raylib.h"

Menu MenuInit() {
	Menu self;

	self.init = 1;
	self.selectedOp = 0;
	sprintf(self.ops[0], "Start Game");
	sprintf(self.ops[1], "Undefined");
	sprintf(self.ops[2], "Score Board");
	sprintf(self.ops[3], "Credits");
	sprintf(self.ops[4], "Exit");

	return self;
}

int MenuRender(Menu *self) {
	int scene, i, px, py;
	Color c;

	scene = 1;
	px = (int)(SCREEN_WIDTH / 3);
	py = (int)(SCREEN_WIDTH / 6);

	ClearBackground(BLACK);
	DrawText("Vacation Game", px, py, 50, YELLOW);

	for(i = 0; i < 5; i++) {
		c = LIGHTGRAY;
		if(self->selectedOp == i) c = VIOLET;
		DrawText(self->ops[i], px + 50, py + 150 + (i * 80), 35, c);
	}

	if(IsKeyPressed(KEY_UP)) {
		if(self->selectedOp > 0) self->selectedOp--;

	} else if(IsKeyPressed(KEY_DOWN)) {
		if(self->selectedOp < 4) self->selectedOp++;

	} else if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
		scene = 2 + self->selectedOp;
	}

	return scene;
}