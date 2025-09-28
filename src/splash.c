#include "splash.h"
#include "raylib.h"
#include "constants.h"

Splash SplashInit() {
	Splash self;

	self.frame = 0;
	self.alpha = 0.0;
	self.init = 1;

	return self;
}

int SplashRender(Splash *self) {
	Color c1 = (Color){ 255, 255, 255, (int)self->alpha };
	Color c2 = (Color){ 255, 203, 0, (int)self->alpha };

	ClearBackground(c1);
	DrawText("Splash Screen", (int)(SCREEN_WIDTH / 3), (int)(SCREEN_HEIGHT / 2), 50, c2);

	self->alpha += 0.5;
	self->frame += 1;

	if(self->frame >= 350) {
		self->init = 0;
		self->frame = 0;
		return 1;
	}

	if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) return 1;

	return 0;
}