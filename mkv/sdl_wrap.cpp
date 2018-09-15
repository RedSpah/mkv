#pragma once
#include <SDL.h>
#include <memory>
#include <functional>
#include <bitset>
#include <array>
#include <random>

struct sdl_window_destroy
{
	void operator()(SDL_Window* win)
	{
		SDL_DestroyWindow(win);
	}
};

struct sdl_renderer_destroy
{
	void operator()(SDL_Renderer* win)
	{
		SDL_DestroyRenderer(win);
	}
};

class at_exit
{
public:
	at_exit(std::function<void(void)> func_): func(func_) {}
	~at_exit() { func(); }
private:
	std::function<void(void)> func;
};

using sdl_window_managed = std::unique_ptr<SDL_Window, sdl_window_destroy>;
using sdl_renderer_managed = std::unique_ptr<SDL_Renderer, sdl_renderer_destroy>;
using sdl_surface = SDL_Surface;


struct sdl_point
{
	SDL_Point p;

	sdl_point(int x, int y) { p.x = x; p.y = y; }
	sdl_point() {};
	operator SDL_Point() { return p; }
	SDL_Point* ptr() { return &p; }
};

struct sdl_rect
{
	SDL_Rect p;

	sdl_rect(int x, int y, int w, int h) { p.x = x; p.y = y; p.w = w; p.h = h; }
	sdl_rect() {};
	operator SDL_Rect() { return p; }
	SDL_Rect* ptr() { return &p; }
};


struct automata_pattern_type
{
	std::array<sdl_point, 6> backrefs;
	int backref_n;

	automata_pattern_type(int n_, std::array<sdl_point, 6> b_ = {sdl_point(0, 0), sdl_point(0, 0), sdl_point(0, 0), sdl_point(0, 0), sdl_point(0, 0), sdl_point(0, 0)}) : backref_n(n_), backrefs(b_) {};
	automata_pattern_type() {};
};

const automata_pattern_type t1_3 = automata_pattern_type(3, { sdl_point(-1, -1), sdl_point(0, -1), sdl_point(1, -1) });
const automata_pattern_type t1_5 = automata_pattern_type(5, { sdl_point(-1, -1), sdl_point(0, -1), sdl_point(1, -1), sdl_point(-2, -1), sdl_point(2, -1) });
const automata_pattern_type t2_3 = automata_pattern_type(6, { sdl_point(-1, -1), sdl_point(0, -1), sdl_point(1, -1), sdl_point(-1, -2), sdl_point(0, -2), sdl_point(1, -2) });

struct prog_automata_pattern
{
	automata_pattern_type p_type;
	std::bitset<64> pattern;

	prog_automata_pattern(automata_pattern_type t_, std::bitset<64> p_) : p_type(t_), pattern(p_) {};
	prog_automata_pattern();

	void change_type(automata_pattern_type t_) { p_type = t_; }
	void change_pattern(std::bitset<64> p_) { pattern = p_; }
};

template <typename RNG>
int mkv_rand(int min, int max, RNG&& rng)
{
	return std::uniform_int_distribution<int>(min, max)(std::forward<RNG>(rng)); 
	void;
}