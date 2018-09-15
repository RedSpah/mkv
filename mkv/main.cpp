#include <SDL_main.h>
#include <SDL.h>

#include "sdl_wrap.cpp"

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


static std::mt19937_64 rng{ std::random_device()() };
static std::uniform_int_distribution<int> col(0, 255);

static std::array<std::bitset<field_max_width>, field_max_height> data_field;

__forceinline int data_field_acc(int x, int y) { return data_field[y][x]; }

inline bool process_pattern(int x, int y, prog_automata_pattern& pattern)
{
	int ret = 0; 

	for (int i = 0; i < pattern.p_type.backref_n; i++)
	{
		ret |= data_field_acc(x + pattern.p_type.backrefs[i].p.x, y + pattern.p_type.backrefs[i].p.y) << i;
	}

	return pattern.pattern[ret];
}

void mutate_pattern(prog_automata_pattern& pattern, int bits)
{
	std::uniform_int_distribution<int> dist(0, (1 << pattern.p_type.backref_n) - 1);

	for (int i = 0; i < bits; i++)
	{
		pattern.pattern.flip(dist(rng));
	}
}

void drawing_thread(sdl_renderer_managed& rend, std::atomic_bool& change_mode, prog_automata_pattern& pattern)
{
	while (1)
	{
		if (change_mode)
		{
			change_mode = 0;

			data_field.fill(std::bitset<field_max_width>(0));

			std::uniform_int_distribution<int> nucl(0, field_max_width - 1);
			std::uniform_int_distribution<int> colsel(0, 2);

			for (int j = 0; j < 1; j++)
			{
				data_field[max_offscreen - 3].set(nucl(rng), true);
				data_field[max_offscreen - 2].set(nucl(rng), true);
				data_field[max_offscreen - 1].set(nucl(rng), true);
			}

			sdl_rect rect(0, 0, screen_max_width, screen_max_height);

			SDL_SetRenderDrawBlendMode(rend.get(), SDL_BLENDMODE_NONE);
			SDL_SetRenderDrawColor(rend.get(), 0, 0, 0, 255);
			SDL_RenderClear(rend.get());
			SDL_SetRenderDrawColor(rend.get(), 255, 255, 255, 120);
			SDL_SetRenderDrawBlendMode(rend.get(), SDL_BLENDMODE_BLEND);

			std::array<sdl_point, screen_max_width> point_list;
			int point_num = 0;

			for (int i = 0; i < 4; i++)
			{
				int colv = colsel(rng);

				//if (colv == 0)
				//{
			//		SDL_SetRenderDrawColor(rend.get(), col(rng), 0, 0, 50);
			//	}
			//	else if (colv == 1)
			//	{
			//		SDL_SetRenderDrawColor(rend.get(), 0, col(rng), 0, 50);
			//	}
			//	else
			//	{
					SDL_SetRenderDrawColor(rend.get(), 255, 255, 255, 75);
			//	}
				

				for (int y = max_offscreen; y < screen_max_height + max_offscreen; y++)
				{
					for (int x = max_offscreen; x < screen_max_width + max_offscreen; x++)
					{
						bool automata = process_pattern(x, y, pattern);
						if (automata) { point_list[point_num++] = sdl_point(x - max_offscreen, y - max_offscreen); }
						data_field[y].set(x, automata);
					}

					
					SDL_RenderDrawPoints(rend.get(), reinterpret_cast<SDL_Point*>(point_list.data()), point_num);
					point_num = 0;

				}
				mutate_pattern(pattern, 1);
				SDL_RenderPresent(rend.get());


				std::fill(data_field.begin(), data_field.begin() + max_offscreen, std::bitset<field_max_width>(0));
				

				for (int j = 0; j < 1; j++)
				{
					data_field[max_offscreen - 3].set(nucl(rng), true);
					data_field[max_offscreen - 2].set(nucl(rng), true);
					data_field[max_offscreen - 1].set(nucl(rng), true);
				}

				//for (int j = 0; j < 8; j++)
				//{
				//	data_field[max_offscreen - 1].set(nucl(rng), true);
				//}
				

			}
			SDL_RenderPresent(rend.get());

			std::cout << "finish gen;\n";
		}
	}
}

int main(int argc, char** argv)
{

	sdl_window_managed main_window;
	sdl_surface* automata_surface;
	sdl_renderer_managed automata_renderer;

	SDL_Init(SDL_INIT_EVERYTHING);
	at_exit at_exit_sdl(SDL_Quit);

	main_window.reset(SDL_CreateWindow("MKV", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_max_width, screen_max_height, SDL_WINDOW_SHOWN));
	automata_surface = SDL_GetWindowSurface(main_window.get());

	automata_renderer.reset(SDL_CreateRenderer(main_window.get(), -1, SDL_RENDERER_ACCELERATED));


	std::atomic_bool change = false;
	prog_automata_pattern automata(t1_3, 0);


	
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
				int n = mkv_rand(3, 6, rng);
				std::array<sdl_point, 6> pts;
				for (int i = 0; i < n; i++)
				{
					pts[i] = sdl_point(mkv_rand(-3, 3, rng), mkv_rand(-3, -1, rng));
				}


				automata.change_type(automata_pattern_type(n, pts));
				automata.change_pattern(std::bitset<64>(pattern_dist(rng)));
				change = true;
				std::cout << "regenerated\n";
			}
		}
	}



}



