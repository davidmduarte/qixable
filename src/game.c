/*
 	TODO:
 	- Na construção de linha se o user parar não acontece nada portanto o user pode parar
 	x 	Se o user tentar voltar para traz, ele pode andar para tras mas é preciso apagar a linha que ja foi escrita para a frente
 	- 	Se o um enimigo colidir com o user ou com a linha que o user está a construir então o user perde uma vida e a linha que ele está a construir é destruida
	-	Se o user tentar crusar pro cima da linha que está a ser construida por ele ele perder uma vida e a linha que ele está a construir é destruida
	-   Se o user levantar a tecla que premite dezenhar uma linha livremente essa linha deve desaparecer e o user volta ao ponto de partida
	-   Quando uma linha livre é concluida mesmo que o user continue a carregar na tecla de criar na tecla de crias linhas livres a mesma não devera funcionar até que o user deixe de pressionar e volte a pressionar a tecla
 */

#include <stdio.h>
#include "raylib.h"
#include "game.h"
#include "constants.h"

Game GameInit() {
	Game self;

	self.init = 1;
	self.curLevel = 0;

	self.levelTex[0]  = LoadTexture("categories/cartoons/1.png");
	self.levelTex[1]  = LoadTexture("categories/cartoons/2.png");
	self.levelTex[2]  = LoadTexture("categories/cartoons/3.png");

	self.levelPercent[0] = 50;
	self.levelPercent[1] = 53;
	self.levelPercent[2] = 56;

	self.plx = -20;
	self.ply = -20;

	self.dir = STOP;
	self.lastDir = STOP;

	return self;
}

void GameUnload(Game *self) {
	int i;

	if(self->pixels != NULL) {
		UnloadImageColors(self->pixels);
		self->pixels = NULL;
	}

	if(self->tex.id != 0) {
		UnloadTexture(self->tex);
		self->tex.id = 0;
	}

	if(self->img.data != NULL) {
		UnloadImage(self->img);
		self->img.data = NULL;
	}

	for(i = 0; i < 3; i++) {
		if(self->levelTex[i].id != 0) {
			UnloadTexture(self->levelTex[i]);
			self->levelTex[i].id = 0;
		}
	}

	self->init = 0;
}

int GameRender(Game *self) {
	int free;
	int debugKey;
	unsigned int offset;
	int gameOver;
	char aux[50];
	int percent;

	if(self->init == 1) {
		self->scale = 600 / self->levelTex[self->curLevel].height;

		self->s.x = self->levelTex[self->curLevel].width * self->scale;
		self->s.y = self->levelTex[self->curLevel].height * self->scale;

		self->p.x = (SCREEN_WIDTH - self->s.x) / 2;
		self->p.y = (SCREEN_HEIGHT - self->s.y) / 2;

		self->plx = self->p.x;
		self->ply = self->p.y;

		InitEnemy(self->enemies);

		self->enemies[0].pos.x = 490; self->enemies[0].pos.y = 300; self->enemies[0].step.x = 1; self->enemies[0].step.y = 1;
		self->enemies[1].pos.x = 540; self->enemies[1].pos.y = 300; self->enemies[1].step.x = -1; self->enemies[1].step.y = 1;
		self->enemies[2].pos.x = 540; self->enemies[2].pos.y = 350; self->enemies[2].step.x = -1; self->enemies[2].step.y = -1;
		self->enemies[3].pos.x = 490; self->enemies[3].pos.y = 350; self->enemies[3].step.x = 1; self->enemies[3].step.y = -1;

		self->img    = GenImageColor((int)self->s.x, (int)self->s.y, BLUE);
		self->tex    = LoadTextureFromImage(self->img);
		self->pixels = LoadImageColors(self->img);

		InitPixels(self);

		self->init       = 2;
		self->invPoint.x = -1;
		self->invPoint.y = -1;
		gameOver         = false;
		self->pathIdx    = 0;
		self->lives      = 5;
		
	} else if(self->init == 2) {
		free     = FALSE;
		debugKey = FALSE;

		// RENDER
		ClearBackground(BLACK);
		DrawRectangle((int)(self->p.x - 5), (int)(self->p.y - 5), (int)(self->s.x + 10), (int)(self->s.y + 10), WHITE);

		DrawTextureEx(self->levelTex[self->curLevel], self->p, 0.0, self->scale, WHITE);
		UpdateTexture(self->tex, self->pixels);
		DrawTextureEx(self->tex, self->p, 0.0, 1, WHITE);
		//DrawPlayer(self->plx, self->ply);

		gameOver = DrawEnemy(self, self->enemies);

		DrawText("SCORE: ", 5, 5, 20, WHITE);
		DrawText("0", 100, 4, 22, YELLOW);
		DrawText("LIVES: ", 5, 35, 20, WHITE);
		DrawText("5", 100, 34, 22, YELLOW);
		percent = GetDonePercentage(self);
		sprintf(aux, "%d %% OF %d %%", percent, self->levelPercent[self->curLevel]);
		DrawText(aux, 400, 50, 40, SKYBLUE);

		if(gameOver) self->init = 3;

		// DEBUG
		if(DEBUG == 1) {
			sprintf(aux, "X: %d", (int)(self->plx - self->p.x));
			DrawText(aux, 750, 200, 20, PINK);
			sprintf(aux, "Y: %d", (int)(self->ply - self->p.y));
			DrawText(aux, 750, 230, 20, PINK);

			offset = (self->ply - (int)self->p.y) * (int)self->s.x + (self->plx - (int)self->p.x);

			DrawRectangle(750, 260, 50, 25, self->pixels[offset]);

			sprintf(aux, "InvPoint(x y): (%d, %d)", (int)self->invPoint.x, (int)self->invPoint.y);
			DrawText(aux, 750, 290, 20, PINK);

			sprintf(aux, "(Axis Step): (%c, %d)", self->invPointAxis, self->invPointStep);
			DrawText(aux, 750, 320, 20, PINK);
		}

		// KEYS
		self->lastDir = self->dir;
		self->dir = STOP;
		if(IsKeyDown(KEY_UP))    self->dir = UP;
		if(IsKeyDown(KEY_DOWN))  self->dir = DOWN;
		if(IsKeyDown(KEY_LEFT))  self->dir = LEFT;
		if(IsKeyDown(KEY_RIGHT)) self->dir = RIGHT;
		if(IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_SPACE)) free = TRUE;
	
		if(DEBUG == TRUE) {
			if(IsKeyDown(KEY_LEFT_SHIFT)) debugKey = TRUE;
		}

		MovePlayer(self, self->dir, free, debugKey);

		if(percent >= self->levelPercent[self->curLevel]) {
			self->init = 3;
		}
	} else {
		// RENDER
		ClearBackground(BLACK);
		DrawRectangle((int)(self->p.x - 5), (int)(self->p.y - 5), (int)(self->s.x + 10), (int)(self->s.y + 10), WHITE);
		DrawTextureEx(self->levelTex[self->curLevel], self->p, 0.0, self->scale, WHITE);

		if(IsKeyDown(KEY_ENTER)) {
			self->curLevel++;
			self->init = 1;
		}
	} 

	return 2;
}

void DrawPlayer(int x, int y) {
	DrawRectangle(x - 6, y - 6, 12, 12, WHITE);
	DrawRectangle(x - 5, y - 5, 10, 10, RED);
}

void InitEnemy(Enemy *e) 
{
	int i;

	for(i = 0 ; i < 100; i++) {
		e[i].pos.x = -1;
		e[i].pos.y = -1;
	}
}

int DrawEnemy(Game *self, Enemy *e)
{
	int i, x, y;
	unsigned int offset;

	for(i = 0; i < 100; i++) {
		if(e[i].pos.x == -1) continue;

		x = e[i].pos.x + e[i].step.x - (int)self->p.x;
		y = e[i].pos.y + e[i].step.y - (int)self->p.y;

		offset = y * (int)self->s.x + x;

		if(offset > (unsigned int)self->s.x * (unsigned int)self->s.y) {
			e[i].pos.x = -1;
			e[i].pos.y = -1;
			continue;
		}

		if(
			self->pixels[offset].r == 230 && self->pixels[offset].g == 41 &&
			self->pixels[offset].b == 55 && self->pixels[offset].a == 255
		) {
			e[i].step.x *= -1;

			x =  e[i].pos.x + e[i].step.x - (int)self->p.x;
			offset = y * (int)self->s.x + x;

			if(FoundPathCollision(self->path, self->pathIdx, offset)) printf("O PLAYER PERDEU UMA VIDA\n");

			if(
				self->pixels[offset].r == 230 && self->pixels[offset].g == 41 &&
				self->pixels[offset].b == 55 && self->pixels[offset].a == 255
			) {
				e[i].step.x *= -1;
				e[i].step.y *= -1;
			}
		}

		e[i].pos.x += e[i].step.x;
		e[i].pos.y += e[i].step.y;

		DrawCircle(e[i].pos.x, e[i].pos.y, 6, YELLOW);
		DrawCircle(e[i].pos.x, e[i].pos.y, 4, VIOLET);

		if(e[i].pos.x == self->plx && e[i].pos.y == self->ply) {
			printf("GAME OVER\n");
			return TRUE;
		}
	}

	return FALSE;
}

void InitPixels(Game *self)
{
	int x, y;
	int sx, sy;

	sx = (int)self->s.x;
	sy = (int)self->s.y;

	for(y = 0; y < sy; y++) {
		for(x = 0; x < sx; x++) {
			if(y == 0 || y == sy - 1) self->pixels[y * sx + x] = RED;
			if(x == 0 || x == sx - 1) self->pixels[y * sx + x] = RED;
		}
	}
}

int OutOfYBounds(Game * self, int y, unsigned int offset)
{
	if(y < 0) return TRUE;
	if(y >= self->s.y) return TRUE;
	if(self->pixels[offset].a == 0) return TRUE;

	return FALSE;
}

int OutOfXBounds(Game * self, int x, unsigned int offset)
{
	if(x < 0) return TRUE;
	if(x >= self->s.x) return TRUE;
	if(self->pixels[offset].a == 0) return TRUE;

	return FALSE;
}

void MovePlayer(Game *self, int dir, int free, int debugKey)
{
	int x, y;
	unsigned int offset, offset0;

	x = self->plx - (int)self->p.x;
	y = self->ply - (int)self->p.y;

	if(free) {
		switch(dir) {
		case UP:
			offset0 = y * (int)self->s.x + x;
			offset = (y - 1) * (int)self->s.x + x;
			if(OutOfYBounds(self, (y - 1), offset)) ClearAreaActions(self);
			else {
				if(! IsColor(self->pixels, offset, 230, 41, 55, 255)) {
					AddNumberToArray(self, offset);
					SetColor(self->pixels, offset, 230, 41, 55, 255);
					if(self->invPoint.x == -1) {
						AddNumberToArray(self, offset0);
						self->invPoint.x = x + 1;
						self->invPoint.y = y - 1;
						self->invPointAxis = 'x';
						self->invPointStep = -2;
					}
					
				} else if(InsidePath(self, offset)) {
					RemoveNumberFromArray(self);
					SetColor(self->pixels, offset0, BLUE.r, BLUE.g, BLUE.b, 255);
				}

				self->ply--;
			}
			break;
		case DOWN:
			offset0 = y * (int)self->s.x + x;
			offset = (y + 1) * (int)self->s.x + x;
			if(OutOfYBounds(self, (y + 1), offset)) ClearAreaActions(self);
			else {
				if(! IsColor(self->pixels, offset, 230, 41, 55, 255)) {
					AddNumberToArray(self, offset);
					SetColor(self->pixels, offset, 230, 41, 55, 255);
					if(self->invPoint.x == -1) {
						AddNumberToArray(self, offset0);
						self->invPoint.x = x + 1;
						self->invPoint.y = y + 1;
						self->invPointAxis = 'x';
						self->invPointStep = -2;
					}
				} else if(InsidePath(self, offset)) {
					RemoveNumberFromArray(self);
					SetColor(self->pixels, offset0, BLUE.r, BLUE.g, BLUE.b, 255);
				}

				self->ply++;
			}
			break;
		case LEFT:
			offset0 = y * (int)self->s.x + x;
			offset = y * (int)self->s.x + (x - 1);
			printf("LEFT\n");
			if(OutOfXBounds(self, (x - 1), offset)) ClearAreaActions(self);
			else {
				printf("InXBounds\n");
				if(! IsColor(self->pixels, offset, 230, 41, 55, 255)) {
					printf("Is Not RED Lets put RED\n");
					AddNumberToArray(self, offset);
					SetColor(self->pixels, offset, 230, 41, 55, 255);
					if(self->invPoint.x == -1) {
						printf("invpoint x -1\n");
						self->invPoint.x = x - 1;
						self->invPoint.y = y + 1;
						self->invPointAxis = 'y';
						self->invPointStep = -2;
					}
				} else if(InsidePath(self, offset)) {
					printf("Is RED from path\n");
					RemoveNumberFromArray(self);
					SetColor(self->pixels, offset0, BLUE.r, BLUE.g, BLUE.b, 255);
				}

				self->plx--;
			}
			printf("---\n");
			break;
		case RIGHT:
			offset0 = y * (int)self->s.x + x;
			offset = y * (int)self->s.x + (x + 1);
			if(OutOfXBounds(self, (x + 1), offset)) ClearAreaActions(self);
			else {
				if(! IsColor(self->pixels, offset, 230, 41, 55, 255)) {
					AddNumberToArray(self, offset);
					SetColor(self->pixels, offset, 230, 41, 55, 255);
					if(self->invPoint.x == -1) {
						AddNumberToArray(self, offset0);
						self->__X = x;
						self->__Y = y;
						self->invPoint.x = x + 1;
						self->invPoint.y = y + 1;
						self->invPointAxis = 'y';
						self->invPointStep = -2;
					}
				} else if(InsidePath(self, offset)) {
					RemoveNumberFromArray(self);
					SetColor(self->pixels, offset0, BLUE.r, BLUE.g, BLUE.b, 255);
				}

				self->plx++;
			}
			break;
		}
	} else {
		switch(dir) {
		case UP:
			if(y - 1 < 0) return;

			offset = (y - 1) * (int)self->s.x + x;

			if(debugKey) self->ply--;
			else {
				if(
					self->pixels[offset].r == 230 && self->pixels[offset].g == 41 &&
					self->pixels[offset].b == 55 && self->pixels[offset].a == 255
				) self->ply--;
			}
			break;
		case DOWN:
			if(y + 1 >= (int)self->s.y) return;

			offset = (y + 1) * (int)self->s.x + x;

			if(debugKey) self->ply++;
			else {
				if(
					self->pixels[offset].r == 230 && self->pixels[offset].g == 41 &&
					self->pixels[offset].b == 55 && self->pixels[offset].a == 255
				) self->ply++;
			}
			break;
		case LEFT:
			if(x - 1 < 0) return;

			offset = y * (int)self->s.x + (x - 1);

			if(debugKey) self->plx--;
			else {
				if(
					self->pixels[offset].r == 230 && self->pixels[offset].g == 41 &&
					self->pixels[offset].b == 55 && self->pixels[offset].a == 255
				) self->plx--;
			}
			break;
		case RIGHT:
			if(x + 1 >= (int)self->s.x) return;

			offset = y * (int)self->s.x + (x + 1);

			if(debugKey) self->plx++;
			else {
				if(
					self->pixels[offset].r == 230 && self->pixels[offset].g == 41 &&
					self->pixels[offset].b == 55 && self->pixels[offset].a == 255
				) self->plx++;
			}
			break;
		}
	}
}

void ClearAreaActions(Game *self)
{
	printf("ClearAreaActions\n");
	Vector2 r;
	unsigned int offset; 

	if(self->invPoint.x == -1) return;

	offset = self->__Y * (int)self->s.x + self->__X;

	if(!PointsInSamePlan(self)) {
		if(self->invPointAxis == 'x') self->invPoint.x += self->invPointStep;
		else self->invPoint.y += self->invPointStep;
	}

	r.x = self->invPoint.x;
	r.y = self->invPoint.y;

	r = ClearArea(self, r.x, r.y, 1);
	while(r.x != -1) r = ClearArea(self, r.x, r.y, 1);

	self->invPoint.x = -1;
	self->invPoint.y = -1;

	SetBorders(self);
}

Vector2 ClearArea(Game *self, int x, int y, unsigned int cnt)
{
	unsigned int offset, debug_offset;
	Vector2 ret;

	if(x == self->__X && y == self->__Y) {
		debug_offset = self->__Y * (int)self->s.x + self->__X;
	}

	ret.x = x;
	ret.y = y;

	if(cnt > 2000) {
		offset = y * (int)self->s.x + x;
		if(
			self->pixels[offset].r == 0 && self->pixels[offset].g == 121 &&
			self->pixels[offset].b == 241 && self->pixels[offset].a == 255
		) return ret;
	}

	ret.x = -1;
	ret.y = -1;

	if(self->invPoint.x == -1) return ret;
	if(x < 0 || x >= (int)self->s.x) return ret;
	if(y < 0 || y >= (int)self->s.y) return ret;

	offset = y * (int)self->s.x + x;

	if(
		self->pixels[offset].r == 230 && self->pixels[offset].g == 41 &&
		self->pixels[offset].b == 55 && self->pixels[offset].a == 255
	) self->pixels[offset].a = 0;

	if(
		self->pixels[offset].r == 0 && self->pixels[offset].g == 121 &&
		self->pixels[offset].b == 241 && self->pixels[offset].a == 255
	) {
		self->pixels[offset].a = 0;

		ret = ClearArea(self, x - 1, y - 1, cnt + 1);
		if(ret.x != -1) return ret;
		ret = ClearArea(self, x    , y - 1, cnt + 1);
		if(ret.x != -1) return ret;
		ret = ClearArea(self, x + 1, y - 1, cnt + 1);
		if(ret.x != -1)  return ret;
		ret = ClearArea(self, x - 1, y, cnt + 1);
		if(ret.x != -1) return ret;
		ret = ClearArea(self, x + 1, y, cnt + 1);
		if(ret.x != -1) return ret;
		ret = ClearArea(self, x -1 , y + 1, cnt + 1);
		if(ret.x != -1) return ret;
		ret = ClearArea(self, x    , y + 1, cnt + 1);
		if(ret.x != -1) return ret;
		ret = ClearArea(self, x + 1, y + 1, cnt + 1);
		if(ret.x != -1) return ret;
	}

	return ret;
}

int PointsInSamePlan(Game *self)
{
	// BUG: Ainda há aqui um bug que é preciso descobrir :(
	int i, j, cnt, cnt2, px, py, rx, ry, step;
	unsigned int offset;

	px = (int)self->invPoint.x;
	py = (int)self->invPoint.y;
	cnt2 = 0;

	for(j = 0; j < 100; j++) {

		if(self->enemies[j].pos.x == -1) break;

		rx = (int)self->enemies[j].pos.x - (int)self->p.x;
		ry = (int)self->enemies[j].pos.y - (int)self->p.y;
		cnt = 0;


		step = -1;
		if(px < rx) step = 1;

		for(i = px; i != rx; i += step) {
			offset = py * (int)self->s.x + i;

			if(
				self->pixels[offset].r == 230 && self->pixels[offset].g == 41 &&
				self->pixels[offset].b == 55 && self->pixels[offset].a > 200
			) cnt++;
		}

		step = -1;
		if(py < ry) step = 1;

		for(i = py; i != ry; i += step) {
			offset = i * (int)self->s.x + rx;

			if(
				self->pixels[offset].r == 230 && self->pixels[offset].g == 41 &&
				self->pixels[offset].b == 55 && self->pixels[offset].a > 200
			) cnt++;
		}

		if(cnt % 2 == 0) cnt2++;
	}

	return cnt2 < (int)(j / 2);
}

int InsidePath(Game *self, unsigned int offset)
{
	unsigned int i;
	for(i = 0; i < self->pathIdx; i++) {
		if(self->path[i] == offset) return TRUE;
	}
	return FALSE;
}

void AddNumberToArray(Game *self, unsigned int o)
{
	self->path[self->pathIdx] = o;
	self->pathIdx++;
}

void RemoveNumberFromArray(Game *self)
{
	self->pathIdx--;
}
void SetBorders(Game *self) {
	unsigned int i;

	for(i = 0; i < self->pathIdx; i++) {
		if(self->path[i] > (unsigned int)self->s.x * (unsigned int)self->s.y) break;

		self->pixels[self->path[i]].r = 230;
		self->pixels[self->path[i]].g = 41;
		self->pixels[self->path[i]].b = 55;
		self->pixels[self->path[i]].a = 255;
	}
	self->pathIdx = 0;
}

void SetColor(Color *pixels, unsigned int offset, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	pixels[offset].r = r;
	pixels[offset].g = g;
	pixels[offset].b = b;
	pixels[offset].a = a;
}

int IsColor(Color *pixels, unsigned int offset, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if(pixels[offset].r == r && pixels[offset].g == g && pixels[offset].b == b && pixels[offset].a == a) return TRUE;
	return FALSE;
}

int GetDonePercentage(Game *self)
{
	int x, y;
	unsigned int offset, cnt;

	cnt = 0;

	for(y = 0; y < (int)self->s.y; y++) {
		for(x = 0; x < (int)self->s.x; x++) {
			offset = y * (int)self->s.x + x;
			if(self->pixels[offset].a == 0) cnt++;
		}
	}
	
	if(cnt == 0) return 0;

	return (int)(100 * cnt / (self->s.x * self->s.y));
}

// esta opção é muito lenta ... há formas mais optimizadas de fazer este algoritmo
int FoundPathCollision(unsigned int *path, unsigned int idx, unsigned int offset)
{
	unsigned int i;

	for(i = 0; i < idx; i++) {
		if(path[i] == offset) return TRUE;
	}

	return FALSE;
}
