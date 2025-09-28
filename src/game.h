#ifndef __GAME__
#define __GAME__

#define DEBUG 1

#define FALSE 0
#define TRUE  1

#define STOP -1
#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4

typedef struct {
	Vector2 pos, step;
} Enemy;

typedef struct {
	int init;
	Texture2D levelTex[18];
	int levelPercent[18];
	int curLevel;
	int plx;
	int ply;
	Enemy enemies[100];
	Image img;
	Texture2D tex;
	Color *pixels;
	float scale;
	Vector2 p;
	Vector2 s;
	Vector2 invPoint;
	int invPointStep;
	char invPointAxis;
	unsigned int path[4000];
	unsigned int pathIdx;
	int __X, __Y;
} Game;

Game GameInit();
int GameRender(Game *self);
void DrawPlayer(int x, int y);
void InitEnemy(Enemy *e);
int DrawEnemy(Game *self, Enemy *e);
int OutOfYBounds(Game * self, int y, unsigned int offset);
int OutOfXBounds(Game * self, int x, unsigned int offset);
void MovePlayer(Game *self, int dir, int free, int debugKey);
void InitPixels(Game *self);
void ClearAreaActions(Game *self);
Vector2 ClearArea(Game *self, int x, int y, unsigned int cnt);
int PointsInSamePlan(Game *self);
void AddNumberToArray(Game *self, unsigned int o);
void SetBorders(Game *self);
void SetColor(Color *pixels, unsigned int offset, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
int IsColor(Color *pixels, unsigned int offset, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
int GetDonePercentage(Game *self);
int FoundPathCollision(unsigned int *path, unsigned int idx, unsigned int offset);

#endif