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
constexpr int max_offscreen = 3;
constexpr int field_max_width = screen_max_width + 2 * max_offscreen;
constexpr int field_max_height = screen_max_height + 2 * max_offscreen;


//static std::mt19937_64 rng{ std::random_device()() };
//static std::uniform_int_distribution<int> col(0, 255);

static std::array<std::bitset<field_max_width>, field_max_height> data_field;

__forceinline int data_field_acc(int x, int y) { return data_field[y][x]; }

inline bool process_pattern(int x, int y, mkv::prog_automata_pattern& pattern)
{
	int ret = 0; 

	for (int i = 0; i < pattern.p_type.backref_n; i++)
	{
		ret |= data_field_acc(x + pattern.p_type.backrefs[i].x, y + pattern.p_type.backrefs[i].y) << i;
	}

	return pattern.pattern[ret];
}

void mutate_pattern(mkv::prog_automata_pattern& pattern, int bits)
{
	for (int i = 0; i < bits; i++)
	{
		pattern.pattern.flip(mkv::rand(0, (1 << pattern.p_type.backref_n) - 1));
	}
}

void drawing_thread(sdl::renderer& rend, std::atomic_bool& change_mode, mkv::prog_automata_pattern& pattern)
{
	while (1)
	{
		if (change_mode)
		{
			change_mode = 0;

			data_field.fill(std::bitset<field_max_width>(0));

			for (int j = 0; j < 1; j++)
			{
			//	data_field[max_offscreen - 3].set(mkv::rand(0, field_max_width - 1), true);
			//	data_field[max_offscreen - 2].set(mkv::rand(0, field_max_width - 1), true);
				data_field[max_offscreen - 1].set(mkv::rand(0, field_max_width - 1), true);
			}

			sdl::rect rect(0, 0, screen_max_width, screen_max_height);

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

				rend.set_draw_color(255, 255, 255, 255);			

				for (int y = max_offscreen; y < screen_max_height + max_offscreen; y++)
				{
					for (int x = max_offscreen; x < screen_max_width + max_offscreen; x++)
					{
						bool automata = process_pattern(x, y, pattern);
						if (automata) { point_list[point_num++] = mkv::point(x - max_offscreen, y - max_offscreen); }
						data_field[y].set(x, automata);
					}

					rend.draw_points(point_list.data(), point_num);
					point_num = 0;
					//mutate_pattern(pattern, 1);

				}
				mutate_pattern(pattern, 1);
				rend.render();


				std::fill(data_field.begin(), data_field.begin() + max_offscreen, std::bitset<field_max_width>(0));
				

				for (int j = 0; j < 1; j++)
				{
					data_field[max_offscreen - 3].set(mkv::rand(0, field_max_width - 1), true);
					data_field[max_offscreen - 2].set(mkv::rand(0, field_max_width - 1), true);
					data_field[max_offscreen - 1].set(mkv::rand(0, field_max_width - 1), true);
				}

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
	mkv::prog_automata_pattern automata(mkv::t1_3, 0);
	
	std::uniform_int_distribution<unsigned long long int> pattern_dist(0, std::numeric_limits<unsigned long long int>::max());
	std::uniform_int_distribution<unsigned int> type_dist(0, 2);
	
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
				int n = mkv::rand(6, 6);
				std::array<mkv::point, 6> pts;
				for (int i = 0; i < n; i++)
				{
					pts[i] = mkv::point(mkv::rand(-3, 3), mkv::rand(-3, -1));
				}

				automata.change_type(mkv::automata_pattern_type(n, pts));
				automata.change_pattern(std::bitset<64>(mkv::rand<uint64_t>(0, std::numeric_limits<uint64_t>::max())));
				change = true;
				std::cout << "regenerated\n";
			}
		}
	}



}



