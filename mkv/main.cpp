#include <SDL_main.h>
#include <SDL.h>

#include "sdl_wrap.hpp"


#include <algorithm>
#include <intrin.h>
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include <random>

constexpr int screen_max_width = 800;
constexpr int screen_max_height = 600;
constexpr int max_offscreen = 5;
constexpr int field_max_width = screen_max_width + 2 * max_offscreen;
constexpr int field_max_height = screen_max_height + 2 * max_offscreen;


//static std::mt19937_64 rng{ std::random_device()() };
//static std::uniform_int_distribution<int> col(0, 255);

static mkv::field data_field(screen_max_width, screen_max_height, max_offscreen);

void drawing_thread(sdl::renderer& rend, std::atomic_bool& change_mode, mkv::automata_pattern& pattern)
{
	while (1)
	{
		if (change_mode)
		{
			change_mode = 0;

			data_field.clear_field();
			data_field.clear_top_margin();

			for (int j = 0; j < 1; j++)
			{
				data_field(mkv::rand(0, field_max_width - 1), -3) = 1;
				data_field(mkv::rand(0, field_max_width - 1), -2) = 1;
				data_field(mkv::rand(0, field_max_width - 1), -1) = 1;
			}

		//	sdl::rect rect(0, 0, screen_max_width, screen_max_height);

			rend.set_blend_mode(SDL_BLENDMODE_NONE);
			rend.set_draw_color(0, 0, 0, 255);
			rend.clear();
			rend.set_draw_color(255, 255, 255, 120);
			rend.set_blend_mode(SDL_BLENDMODE_BLEND);

			std::array<mkv::point, screen_max_width> point_list;
			int point_num = 0;

			for (int i = 0; i < 1; i++)
			{
				int colv = mkv::rand(0, 2);

			//	if (i % 2 == 0)
				//{
					rend.set_draw_color(255, 255, 255, 255);
				//}
				//else
				//{
				//	rend.set_draw_color(0, 0, 0, 90);
				//}

				for (int y = 0; y < screen_max_height; y++)
				{
					for (int x = 0; x < screen_max_width; x++)
					{
						bool auto_result = pattern.pattern_calc(data_field, x, y);
						if (auto_result) { point_list[point_num++] = mkv::point(x, y); }

					//	if (i % 2 == 0)
						//{
							data_field(x, y) = auto_result;
						//}
						//else
						//{
							data_field(x, y) = !auto_result;
						//}

						//if (mkv::rand(1, 100) == 1) { mutate_pattern(pattern, 1); }
					}

					rend.draw_points(point_list.data(), point_num);
					point_num = 0;
					//if (mkv::rand(1, 2) == 1) { mutate_pattern(pattern, 1); }

				}
				pattern.mutate(4);
				rend.render();

				data_field.clear_field();
				
				

				//for (int j = 0; j < 1; j++)
				//{
				//	data_field[max_offscreen - 3].set(mkv::rand(0, field_max_width - 1), true);
				//	data_field[max_offscreen - 2].set(mkv::rand(0, field_max_width - 1), true);
				//	data_field[max_offscreen - 1].set(mkv::rand(0, field_max_width - 1), true);
				//}*/

				

				//for (int j = 0; j < 8; j++)
				//{
				//	data_field[max_offscreen - 1].set(nucl(rng), true);
				//}
				

			}
			rend.render();

			std::cout << "finish gen;\n";
		}
	}
}

int main(int argc, char** argv)
{
	sdl::session session;

	sdl::window main_window("mkv", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_max_width, screen_max_height, SDL_WINDOW_SHOWN);
	sdl::renderer automata_renderer = main_window.get_renderer();

	std::atomic_bool change = false;

	constexpr int32_t max_backrefs = 10;
	mkv::fundamental_progressive_pattern<max_backrefs> automata;

	decltype(automata)::pattern_t pattern;
	decltype(automata)::backref_t backrefs;

	
	std::thread draw_thread(drawing_thread, std::ref(automata_renderer), std::ref(change), std::ref(automata));

	while (1)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				std::exit(0);
			}

			else if (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_r)
			{
				int n = mkv::rand(10, 10);

				for (int i = 0; i < n; i++)
				{
					mkv::point p(mkv::rand(-5, 5), mkv::rand(-2, -1));
					if (std::count(backrefs.begin(), backrefs.begin() + i, p) > 0) { i--; continue; }
					backrefs[i] = p;
				}

				std::generate(pattern.begin(), pattern.end(), []() {return mkv::rand(0ull, std::numeric_limits<uint64_t>::max()); });

				automata.change_backrefs(backrefs, n);
				automata.change_pattern(pattern);

				change = true;
				std::cout << "regenerated\n";
			}
		}
	}



}



