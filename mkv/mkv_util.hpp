#pragma once
#include <random>
#include <chrono>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <bitset>
#include <array>
#include <SDL.h>

namespace mkv
{
	static std::mt19937 rng {std::random_device()()};

	template <typename T = int>
	T rand(T min, T max)
	{
		return std::uniform_int_distribution<T>(min, max)(rng);
	}

	bool rand_b(float chance)
	{
		return (std::uniform_real_distribution<float>(0, 1)(rng) < chance);
	}

	struct point
	{
		int x, y;
		point(int x_, int y_) : x(x_), y(y_) {};
		point() {};

		operator SDL_Point() { SDL_Point p; p.x = x; p.y = y; return p; }
		SDL_Point* ptr() { return reinterpret_cast<SDL_Point*>(this); }
	};

	class at_exit
	{
	public:
		at_exit(std::function<void(void)> func_) : func(func_) {}
		~at_exit() { func(); }
	private:
		std::function<void(void)> func;
	};





	struct automata_pattern_type
	{
		std::array<point, 6> backrefs;
		int backref_n;

		automata_pattern_type(int n_, std::array<point, 6> b_ = { point(0, 0), point(0, 0), point(0, 0), point(0, 0), point(0, 0), point(0, 0) }) : backref_n(n_), backrefs(b_) {};
		automata_pattern_type() {};
	};

	const automata_pattern_type t1_3 = automata_pattern_type(3, { point(-1, -1), point(0, -1), point(1, -1) });
	const automata_pattern_type t1_5 = automata_pattern_type(5, { point(-1, -1), point(0, -1), point(1, -1), point(-2, -1), point(2, -1) });
	const automata_pattern_type t2_3 = automata_pattern_type(6, { point(-1, -1), point(0, -1), point(1, -1), point(-1, -2), point(0, -2), point(1, -2) });

	struct prog_automata_pattern
	{
		automata_pattern_type p_type;
		std::bitset<64> pattern;

		prog_automata_pattern(automata_pattern_type t_, std::bitset<64> p_) : p_type(t_), pattern(p_) {};
		prog_automata_pattern() {};

		void change_type(automata_pattern_type t_) { p_type = t_; }
		void change_pattern(std::bitset<64> p_) { pattern = p_; }
	};



}