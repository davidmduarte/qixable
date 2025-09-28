#include <stdio.h>
#include "raylib.h"
#include "constants.h"
#include "splash.h"
#include "menu.h"
#include "game.h"

int main() {
	int quit = 0;
	int scene = 0;
	char fps[50];
	Splash splash;
	Menu menu;
	Game game;

	splash.init = 0;
	menu.init = 0;
	game.init = 0;

	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Vacation Game");
	SetTargetFPS(120);

	while(! WindowShouldClose()) {
		BeginDrawing();

		switch(scene) {
			case 0:
				if(splash.init == 0) splash = SplashInit();
				scene = SplashRender(&splash);
				break;
			case 1:
				if(menu.init == 0) menu = MenuInit();
				scene = MenuRender(&menu);
				break;
			case 2:
				if(game.init == 0) game = GameInit();
				scene = GameRender(&game);
				break;
			case 3:
				printf("OP 3");
				break;
			case 4:
				printf("OP 4");
				break;
			case 5:
				printf("OP 5");
				break;
			case 6:
				quit = 1;
				break;

		}

		if(quit == 1) break;

		sprintf(fps, "FPS: %d", GetFPS());
		DrawText(fps, 0, 700, 30, GREEN);
		EndDrawing();
	}

	CloseWindow();

	return 0;
}