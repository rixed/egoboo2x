// input.c

// Egoboo, Copyright (C) 2000 Aaron Bishop

#include "egoboo.h"

#include <stdint.h>
static uint_least32_t gp2x_keys;
int key_set(unsigned k)
{
	assert(k <= 18);
	return !!(gp2x_keys & (1<<k));
}
int key_left(void)
{
	return key_set(GP2X_LEFT) || key_set(GP2X_UPLEFT) || key_set(GP2X_DOWNLEFT);
}
int key_right(void)
{
	return key_set(GP2X_RIGHT) || key_set(GP2X_UPRIGHT) || key_set(GP2X_DOWNRIGHT);
}
int key_up(void)
{
	return key_set(GP2X_UP) || key_set(GP2X_UPRIGHT) || key_set(GP2X_UPLEFT);
}
int key_down(void)
{
	return key_set(GP2X_DOWN) || key_set(GP2X_DOWNRIGHT) || key_set(GP2X_DOWNLEFT);
}
int key_set_once(unsigned k)
{
	if (key_set(k)) {
		gp2x_keys &= ~(1<<k);
		return 1;
	}
	return 0;
}
int gp2x_click(void)
{
	return key_set(GP2X_CLICK) || key_set(GP2X_X);
}
static unsigned gp2x2gp2x(unsigned k)
{
	assert(k <= 18);
	return 1<<k;
}
static int sdlsym2gp2x(int sym)
{
	switch (sym)
	{
		case SDLK_UP:
			return 1<<GP2X_UP;
		case SDLK_DOWN:
			return 1<<GP2X_DOWN;
		case SDLK_LEFT:
			return 1<<GP2X_LEFT;
		case SDLK_RIGHT:
			return 1<<GP2X_RIGHT;
		case SDLK_RETURN:
			return 1<<GP2X_CLICK;
		case SDLK_a:
			return 1<<GP2X_A;
		case SDLK_b:
			return 1<<GP2X_B;
		case SDLK_y:
			return 1<<GP2X_Y;
		case SDLK_x:
			return 1<<GP2X_X;
		case SDLK_r:
			return 1<<GP2X_R;
		case SDLK_l:
			return 1<<GP2X_L;
		case SDLK_t:
			return 1<<GP2X_START;
		case SDLK_e:
			return 1<<GP2X_SELECT;
		case SDLK_MINUS:
			return 1<<GP2X_VOLM;
		case SDLK_PLUS:
			return 1<<GP2X_VOLP;
		default:
			fprintf(stderr, "ignoring sdl key %d\n", sym);
			return 0;
	}
}

void read_input()
{
	// ZZ> This function gets all the current player input states
	int cnt;
	SDL_Event ev;

	SDL_PumpEvents();
	int had_click = gp2x_click();
#	ifdef GP2X
	while(SDL_PollEvent(&ev))
	{
		switch(ev.type)
		{
			case SDL_JOYBUTTONDOWN:
				gp2x_keys |= gp2x2gp2x(ev.jbutton.button);
				break;
			case SDL_JOYBUTTONUP:
				gp2x_keys &= ~gp2x2gp2x(ev.jbutton.button);
				break;
		}
	}
#	else
	while(SDL_PollEvent(&ev))
	{
		switch(ev.type)
		{
			case SDL_KEYDOWN:
				gp2x_keys |= sdlsym2gp2x(ev.key.keysym.sym);
				break;
			case SDL_KEYUP:
				gp2x_keys &= ~sdlsym2gp2x(ev.key.keysym.sym);
				break;
		}
	}
#	endif
	pending_click = !had_click && gp2x_click();
}

void reset_press()
{
	gp2x_keys = 0;
}
