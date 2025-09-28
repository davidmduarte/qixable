#ifndef __splash__
#define __splash__

typedef struct {
	int init;
	int frame;
	float alpha;
} Splash;

Splash SplashInit();
int SplashRender(Splash *self);

#endif