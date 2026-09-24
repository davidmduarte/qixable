/* TODO: enemy collisions with the player or an unfinished line must cancel it and cost a life. */

#include <stdio.h>
#include <stdlib.h>
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
		self->drawActive = FALSE;
		self->drawLocked = FALSE;
		
	} else if(self->init == 2) {
		free     = FALSE;
		debugKey = FALSE;

		// RENDER
		ClearBackground(BLACK);
		DrawRectangle((int)(self->p.x - 5), (int)(self->p.y - 5), (int)(self->s.x + 10), (int)(self->s.y + 10), WHITE);

		DrawTextureEx(self->levelTex[self->curLevel], self->p, 0.0, self->scale, WHITE);
		UpdateTexture(self->tex, self->pixels);
		DrawTextureEx(self->tex, self->p, 0.0, 1, WHITE);
		DrawPlayer(self->plx, self->ply);

		gameOver = DrawEnemy(self, self->enemies);

		DrawText("SCORE: ", 5, 5, 20, WHITE);
		DrawText("0", 100, 4, 22, YELLOW);
		DrawText("LIVES: ", 5, 35, 20, WHITE);
		sprintf(aux, "%u", self->lives);
		DrawText(aux, 100, 34, 22, YELLOW);
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
		if(IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_SPACE)) {
			if(!self->drawLocked) free = TRUE;
		} else {
			self->drawLocked = FALSE;
		}
	
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

static void CancelDraw(Game *self, int returnToStart, int lockDraw)
{
	unsigned int i;

	for(i = 0; i < self->pathIdx; i++) {
		SetColor(self->pixels, self->path[i], BLUE.r, BLUE.g, BLUE.b, 255);
	}
	self->pathIdx = 0;
	if(returnToStart && self->drawActive) {
		self->plx = self->drawStartX;
		self->ply = self->drawStartY;
	}
	self->drawActive = FALSE;
	self->invPoint.x = -1;
	self->invPoint.y = -1;
	if(lockDraw) self->drawLocked = TRUE;
}

static void LoseLife(Game *self)
{
	int hadActiveDraw;
	hadActiveDraw = self->drawActive;
	if(self->lives > 0) self->lives--;
	CancelDraw(self, TRUE, TRUE);
	if(!hadActiveDraw) {
		self->plx = (int)self->p.x;
		self->ply = (int)self->p.y;
	}
}

int DrawEnemy(Game *self, Enemy *e)
{
	int i, x, y, hitPlayer;
	unsigned int offset;
	hitPlayer = FALSE;

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
			if(!hitPlayer && self->drawActive &&
				FoundPathCollision(self->path, self->pathIdx, offset)) {
				LoseLife(self);
				hitPlayer = TRUE;
			}
			e[i].step.x *= -1;

			x =  e[i].pos.x + e[i].step.x - (int)self->p.x;
			offset = y * (int)self->s.x + x;

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
			if(!hitPlayer) {
				LoseLife(self);
				hitPlayer = TRUE;
			}
			if(self->lives == 0) return TRUE;
		}
	}

	return self->lives == 0;
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
	int x, y, nextX, nextY;
	unsigned int offset, startOffset;
	int onRedLine, onBlue;

	x = self->plx - (int)self->p.x;
	y = self->ply - (int)self->p.y;
	nextX = x;
	nextY = y;
	if(!free && self->drawActive) {
		CancelDraw(self, TRUE, FALSE);
		return;
	}
	if(!free && self->drawLocked) self->drawLocked = FALSE;

	if(dir == UP) nextY--;
	else if(dir == DOWN) nextY++;
	else if(dir == LEFT) nextX--;
	else if(dir == RIGHT) nextX++;

	if(dir == STOP) return;
	if(nextX < 0 || nextX >= (int)self->s.x || nextY < 0 || nextY >= (int)self->s.y) {
		if(free && self->drawActive) {
			ClearAreaActions(self);
			self->drawActive = FALSE;
			self->drawLocked = TRUE;
		}
		return;
	}

	offset = (unsigned int)(nextY * (int)self->s.x + nextX);
	if(free) {
		onRedLine = IsColor(self->pixels, offset, 230, 41, 55, 255);
		onBlue = IsColor(self->pixels, offset, BLUE.r, BLUE.g, BLUE.b, BLUE.a);
		startOffset = (unsigned int)(self->drawStartY * (int)self->s.x +
			self->drawStartX);

		/* Step backwards over the most recently drawn pixel. */
		if(self->drawActive && self->pathIdx > 0 &&
			((self->pathIdx > 1 && offset == self->path[self->pathIdx - 2]) ||
			 (self->pathIdx == 1 && offset == startOffset))) {
			SetColor(self->pixels, self->path[self->pathIdx - 1],
				BLUE.r, BLUE.g, BLUE.b, 255);
			self->pathIdx--;
			self->plx = (int)self->p.x + nextX;
			self->ply = (int)self->p.y + nextY;
			if(self->pathIdx == 0) {
				self->drawActive = FALSE;
				self->invPoint.x = -1;
				self->invPoint.y = -1;
			}
			return;
		}

		if(self->drawActive && InsidePath(self, offset)) return;

		if(self->drawActive && (onRedLine || self->pixels[offset].a == 0)) {
			if(self->pathIdx + 2 <= sizeof(self->path) / sizeof(self->path[0])) {
				startOffset = (unsigned int)(
					(self->drawStartY - (int)self->p.y) * (int)self->s.x +
					(self->drawStartX - (int)self->p.x));
				AddNumberToArray(self, startOffset);
				AddNumberToArray(self, offset);
			}
			ClearAreaActions(self);
			self->drawActive = FALSE;
			self->drawLocked = TRUE;
			return;
		}
		if(self->drawActive && !onBlue) return;
		if(!self->drawActive && onRedLine) {
			self->plx = (int)self->p.x + nextX;
			self->ply = (int)self->p.y + nextY;
			return;
		}
		if(!onBlue) return;

		if(!self->drawActive) {
			self->drawActive = TRUE;
			self->drawStartX = self->plx;
			self->drawStartY = self->ply;
			self->__X = x;
			self->__Y = y;
		}
		if(self->pathIdx >= sizeof(self->path) / sizeof(self->path[0])) return;
		if(self->invPoint.x == -1) {
			if(dir == UP) {
				self->invPoint.x = x + 1; self->invPoint.y = nextY;
				self->invPointAxis = 'x'; self->invPointStep = -2;
			} else if(dir == DOWN) {
				self->invPoint.x = x + 1; self->invPoint.y = nextY;
				self->invPointAxis = 'x'; self->invPointStep = -2;
			} else if(dir == LEFT) {
				self->invPoint.x = nextX; self->invPoint.y = y + 1;
				self->invPointAxis = 'y'; self->invPointStep = -2;
			} else {
				self->invPoint.x = nextX; self->invPoint.y = y + 1;
				self->invPointAxis = 'y'; self->invPointStep = -2;
			}
		}
		AddNumberToArray(self, offset);
		SetColor(self->pixels, offset, 230, 41, 55, 255);
		self->plx = (int)self->p.x + nextX;
		self->ply = (int)self->p.y + nextY;
		return;
	}

	if(self->drawActive) return;
	if(debugKey || IsColor(self->pixels, offset, 230, 41, 55, 255)) {
		self->plx = (int)self->p.x + nextX;
		self->ply = (int)self->p.y + nextY;
	}
}

void ClearAreaActions(Game *self)
{
	printf("ClearAreaActions\n");

	if(self->invPoint.x == -1) return;

	if(!PointsInSamePlan(self)) {
		if(self->invPointAxis == 'x') self->invPoint.x += self->invPointStep;
		else self->invPoint.y += self->invPointStep;
	}

	ClearArea(self, (int)self->invPoint.x, (int)self->invPoint.y);

	self->invPoint.x = -1;
	self->invPoint.y = -1;

	SetBorders(self);
}

void ClearArea(Game *self, int x, int y)
{
	int width, height, nx, ny, dx, dy;
	size_t pixelCount, head, tail;
	unsigned int *queue;
	unsigned int offset, neighbor;
	Color *pixel;

	width = (int)self->s.x;
	height = (int)self->s.y;
	if(width <= 0 || height <= 0) return;
	if(x < 0 || x >= width || y < 0 || y >= height) return;
	if(self->invPoint.x == -1 || self->pixels == NULL) return;

	pixelCount = (size_t)width * (size_t)height;
	queue = (unsigned int *)malloc(pixelCount * sizeof(*queue));
	if(queue == NULL) return;

	head = 0;
	tail = 0;
	offset = (unsigned int)(y * width + x);
	pixel = &self->pixels[offset];
	if(pixel->r == 0 && pixel->g == 121 && pixel->b == 241 && pixel->a == 255) {
		pixel->a = 0;
		queue[tail++] = offset;
	} else if(pixel->r == 230 && pixel->g == 41 && pixel->b == 55 && pixel->a == 255) {
		pixel->a = 0;
	} else {
		free(queue);
		return;
	}

	while(head < tail) {
		offset = queue[head++];
		x = (int)(offset % (unsigned int)width);
		y = (int)(offset / (unsigned int)width);

		for(dy = -1; dy <= 1; dy++) {
			for(dx = -1; dx <= 1; dx++) {
				if(dx == 0 && dy == 0) continue;
				nx = x + dx;
				ny = y + dy;
				if(nx < 0 || nx >= width || ny < 0 || ny >= height) continue;

				neighbor = (unsigned int)(ny * width + nx);
				pixel = &self->pixels[neighbor];
				if(pixel->r == 0 && pixel->g == 121 && pixel->b == 241 && pixel->a == 255) {
					/* Mark when enqueued so each pixel enters the queue once. */
					pixel->a = 0;
					queue[tail++] = neighbor;
				} else if(pixel->r == 230 && pixel->g == 41 && pixel->b == 55 && pixel->a == 255) {
					/* The drawn path is cleared here, then restored by SetBorders. */
					pixel->a = 0;
				}
			}
		}
	}

	free(queue);
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
