#pragma once
#include <SDL.h>
#include "mkv_util.hpp"
#include <memory>
#include <functional>
#include <bitset>
#include <array>
#include <random>

namespace sdl
{
	struct point
	{
		SDL_Point p;

		point(int32_t x, int32_t y) { p.x = x; p.y = y; }
		point() {};

		operator SDL_Point() { return p; }
		SDL_Point* ptr() { return &p; }
	};

	struct rect
	{
		SDL_Rect p;
			
		rect(int32_t x, int32_t y, int32_t w, int32_t h) { p.x = x; p.y = y; p.w = w; p.h = h; }
		rect() {};
		operator SDL_Rect() { return p; }
		SDL_Rect* ptr() { return &p; }
	};

	class renderer
	{
		SDL_Renderer* _rend;

	public:

		renderer(SDL_Renderer* rend) : _rend(rend) {};

		~renderer()
		{
			SDL_DestroyRenderer(_rend);
		}

		void set_blend_mode(SDL_BlendMode blendmode)
		{
			SDL_SetRenderDrawBlendMode(_rend, blendmode);
		}

		void set_draw_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
		{
			SDL_SetRenderDrawColor(_rend, r, g, b, a);
		}

		void clear()
		{
			SDL_RenderClear(_rend);
		}

		void draw_point(int32_t x, int32_t y)
		{
			SDL_RenderDrawPoint(_rend, x, y);
		}

		void draw_points(const mkv::point* ptr, int32_t n)
		{
			SDL_RenderDrawPoints(_rend, reinterpret_cast<const SDL_Point*>(ptr), n);
		}

		void draw_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
		{
			SDL_RenderDrawLine(_rend, x1, y1, x2, y2);
		}

		void draw_lines(const mkv::point* ptr, int32_t n)
		{
			SDL_RenderDrawLines(_rend, reinterpret_cast<const SDL_Point*>(ptr), n);
		}

		void render()
		{
			SDL_RenderPresent(_rend);
		}

	};

	class window
	{
		SDL_Window* _win;

	public:

		window(const char* name, int32_t pos_x, int32_t pos_y, int32_t width, int32_t height, uint32_t flags)
		{
			_win = SDL_CreateWindow(name, pos_x, pos_y, width, height, flags);
		};

		~window()
		{
			SDL_DestroyWindow(_win);
		}

		renderer get_renderer(int32_t index = -1, uint32_t flags = SDL_RENDERER_ACCELERATED)
		{
			return renderer(SDL_CreateRenderer(_win, index, flags));
		}
		
		SDL_Window* ptr()
		{
			return _win;
		}
	};

	

	struct session
	{
		session()
		{
			SDL_Init(SDL_INIT_EVERYTHING);
		}

		~session()
		{
			SDL_Quit();
		}
	};
}

