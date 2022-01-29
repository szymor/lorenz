#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_gfxPrimitives.h>

#define SCREEN_WIDTH		(320)
#define SCREEN_HEIGHT		(240)
#define SCREEN_BPP			(32)
#define FPS_LIMIT			(60)
#define BRIGHTNESS			(255)
#define ALPHA				(24)
#define RADIUS				(4)

struct Lorenz
{
	double rho;
	double sigma;
	double beta;
	double x;
	double y;
	double z;
};

SDL_Surface *screen = NULL;
int fps = 0;
bool fps_on = false;

void lorenz_update(struct Lorenz *l, double dt)
{
	double dx = l->rho * (l->y - l->x);
	double dy = l->x * (l->rho - l->z) - l->y;
	double dz = l->x * l->y - l->beta * l->z;
	l->x += dx * dt;
	l->y += dy * dt;
	l->z += dz * dt;
}

Uint32 getPixel(SDL_Surface *s, int x, int y)
{
	int i = y * s->w + x;
	Uint32 *p = (Uint32*)s->pixels;
	return p[i];
}

void setPixel(SDL_Surface *s, int x, int y, Uint32 c)
{
	if (x < 0 || x >= s->w || y < 0 || y >= s->h)
		return;
	int i = y * s->w + x;
	Uint32 *p = (Uint32*)s->pixels;
	p[i] = c;
}

void fps_counter(double dt)
{
	static double total = 0;
	static int count = 0;
	total += dt;
	++count;
	if (total > 1.0)
	{
		fps = count;
		total -= 1.0;
		count = 0;
	}
}

void fps_limiter(void)
{
	static const Uint32 target_frame_duration = 1000 / FPS_LIMIT;
	static Uint32 prev = 0;
	Uint32 curr = SDL_GetTicks();
	if (0 == prev)
	{
		prev = curr;
	}
	while (curr < prev + target_frame_duration)
	{
		SDL_Delay(1);
		curr = SDL_GetTicks();
	}
	prev = curr;
}

void fps_draw(void)
{
	char string[8] = "";
	sprintf(string, "%d", fps);
	SDL_FillRect(screen, &(SDL_Rect){.x = 0, .y = 0, .w = 64, .h = 16}, SDL_MapRGB(screen->format, 0, 0, 0));
	stringRGBA(screen, 0, 0, string, 255, 255, 255, 255);
}

int main(int argc, char *argv[])
{
	srand(time(NULL));
	SDL_Init(SDL_INIT_VIDEO);
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE);
	//SDL_EnableKeyRepeat(100, 50);
	SDL_ShowCursor(SDL_DISABLE);

	struct Lorenz l1 = {
		.rho = 28,
		.sigma = 10,
		.beta = 8/3,
		.x = 10,
		.y = 25,
		.z = 53
	};
	struct Lorenz l2 = {
		.rho = 34,
		.sigma = 9,
		.beta = 2,
		.x = 0.5,
		.y = 1.5,
		.z = 1
	};
	struct Lorenz l3 = {
		.rho = 28,
		.sigma = 9,
		.beta = 2,
		.x = 0.2,
		.y = 0.7,
		.z = 0.9
	};
	struct Lorenz l4 = {
		.rho = 28,
		.sigma = 10,
		.beta = 8/3,
		.x = 1,
		.y = 1,
		.z = 1
	};
	struct Lorenz l5 = {
		.rho = 34,
		.sigma = 9,
		.beta = 2,
		.x = 5,
		.y = 5,
		.z = 5
	};
	struct Lorenz l6 = {
		.rho = 28,
		.sigma = 9,
		.beta = 2,
		.x = 20,
		.y = 20,
		.z = 20
	};

	SDL_Surface *darken = SDL_CreateRGBSurface(SDL_SRCALPHA, SCREEN_WIDTH,
		SCREEN_HEIGHT, SCREEN_BPP, screen->format->Rmask, screen->format->Gmask,
		screen->format->Bmask, screen->format->Amask);
	SDL_FillRect(darken, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
	SDL_SetAlpha(darken, SDL_SRCALPHA, 1);

	bool quit = false;
	Uint32 curr = SDL_GetTicks();
	Uint32 prev = curr;
	while (!quit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_RETURN:
							fps_on = !fps_on;
							break;
						case SDLK_ESCAPE:
							quit = true;
							break;
					}
					break;
				case SDL_QUIT:
					quit = true;
					break;
			}
		}
		curr = SDL_GetTicks();
		Uint32 delta = curr - prev;
		prev = curr;
		double dt = delta / 1000.0;
		fps_limiter();
		fps_counter(dt);

		//SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		SDL_LockSurface(screen);
		const double ddt = 0.001;
		for (int i = 0; i < 10; ++i)
		{
			lorenz_update(&l1, ddt);
			lorenz_update(&l2, ddt);
			lorenz_update(&l3, ddt);
			lorenz_update(&l4, ddt);
			lorenz_update(&l5, ddt);
			lorenz_update(&l6, ddt);
			//printf("%f %f %f\n", dx, dy, dz);
			circleRGBA(screen, l1.x * 6 + SCREEN_WIDTH / 2, l1.z * 4 + 15, RADIUS, BRIGHTNESS, 0, 0, ALPHA);
			circleRGBA(screen, l2.x * 6 + SCREEN_WIDTH / 2, l2.y * 4 + 120, RADIUS, 0, BRIGHTNESS, 0, ALPHA);
			circleRGBA(screen, l3.y * 6 + SCREEN_WIDTH / 2, l3.z * 4 + 15, RADIUS, 0, 0, BRIGHTNESS, ALPHA);
			circleRGBA(screen, l4.x * 6 + SCREEN_WIDTH / 2, l4.z * 4 + 15, RADIUS, BRIGHTNESS, BRIGHTNESS, 0, ALPHA);
			circleRGBA(screen, l5.x * 6 + SCREEN_WIDTH / 2, l5.y * 4 + 120, RADIUS, 0, BRIGHTNESS, BRIGHTNESS, ALPHA);
			circleRGBA(screen, l6.y * 6 + SCREEN_WIDTH / 2, l6.z * 4 + 15, RADIUS, BRIGHTNESS, 0, BRIGHTNESS, ALPHA);
			//setPixel(screen, l1.x * 4 + SCREEN_WIDTH / 2, l1.z * 4 + 15, SDL_MapRGB(screen->format, BRIGHTNESS, 0, 0));
			//setPixel(screen, l2.x * 4 + SCREEN_WIDTH / 2, l2.y * 4 + 120, SDL_MapRGB(screen->format, 0, BRIGHTNESS, 0));
			//setPixel(screen, l3.y * 4 + SCREEN_WIDTH / 2, l3.z * 4 + 15, SDL_MapRGB(screen->format, 0, 0, BRIGHTNESS));
			//setPixel(screen, l4.x * 4 + SCREEN_WIDTH / 2, l4.z * 4 + 15, SDL_MapRGB(screen->format, BRIGHTNESS, BRIGHTNESS, 0));
			//setPixel(screen, l5.x * 4 + SCREEN_WIDTH / 2, l5.y * 4 + 120, SDL_MapRGB(screen->format, 0, BRIGHTNESS, BRIGHTNESS));
			//setPixel(screen, l6.y * 4 + SCREEN_WIDTH / 2, l6.z * 4 + 15, SDL_MapRGB(screen->format, BRIGHTNESS, 0, BRIGHTNESS));
		}
		SDL_UnlockSurface(screen);
		SDL_BlitSurface(darken, NULL, screen, NULL);
		if (fps_on)
			fps_draw();
		else
			SDL_FillRect(screen, &(SDL_Rect){.x = 0, .y = 0, .w = 64, .h = 16}, SDL_MapRGB(screen->format, 0, 0, 0));
		SDL_Flip(screen);
	}
	SDL_Quit();
	return 0;
}
