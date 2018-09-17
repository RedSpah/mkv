#pragma once
#include <random>
#include <chrono>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <bitset>
#include <array>
#include <SDL.h>
#include <exception>

namespace mkv
{
	namespace tmp
	{
		template <uint8_t p>
		constexpr uint64_t pow_2 = (1ull << p);

		constexpr uint64_t at_least(uint64_t n, uint64_t l)
		{
			if (n < l) 
			{
				return l;
			}
			else return n;
		}
	}


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

		bool operator== (point o) { return x == o.x &&y == o.y; }

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

	//template <uint32_t width, uint32_t height, uint32_t margins>
	class field
	{
		std::vector<uint8_t> _field;
		const uint32_t width, height, margins;

		//std::array<std::array<uint8_t, width + margins * 2>, height + margins * 2> _field;

	public:
		field(uint32_t w_, uint32_t h_, uint32_t m_): width(w_), height(h_), margins(m_) 
		{
			_field = std::vector<uint8_t>((width + margins * 2) * (height + margins * 2));
			std::fill(_field.begin(), _field.end(), 0);
		};

		void clear_field()
		{
			std::fill(_field.begin() + margins * (width + margins * 2), _field.end(), 0);
		}

		void clear_top_margin()
		{
			std::fill(_field.begin(), _field.begin() + margins * (width + margins * 2), 0);
		}

		uint8_t& operator()(int32_t x, int32_t y)
		{
#if _DEBUG
			if (x + margins < 0 || y + margins < 0) { throw std::invalid_argument("index out of bounds"); }
#endif
			int x_r = x + margins, y_r = y + margins;

			return _field[y_r * (width + margins * 2) + x_r];
		}

		uint8_t& operator()(mkv::point p)
		{
			return operator()(p.x, p.y);
		}

		const uint8_t& operator()(int32_t x, int32_t y) const
		{
#if _DEBUG
			if (x + margins < 0 || y + margins < 0) { throw std::invalid_argument("index out of bounds"); }
#endif
			int x_r = x + margins, y_r = y + margins;

			return _field[y_r * (width + margins * 2) + x_r];
		}

		const uint8_t& operator()(mkv::point p) const
		{
			return operator()(p.x, p.y);
		}
	};

	class automata_pattern
	{
	public:
		virtual uint8_t pattern_calc(const field& field, int32_t x, int32_t y) = 0;

		virtual void mutate(int32_t bits) = 0;
	};

	template <uint8_t max_backrefs>
	class fundamental_progressive_pattern : public automata_pattern
	{
	public:
		using backref_t = std::array<point, max_backrefs>;
		using pattern_t = std::array<uint64_t, tmp::at_least(tmp::pow_2<max_backrefs> / 64, 1)>;

	private:
		backref_t backrefs;
		int32_t backref_n;
		pattern_t pattern;

		bool pattern_get(uint64_t bit)
		{
			uint32_t qword = bit / 64, sub_word = bit % 64;

			return (pattern[qword] & (1ull << sub_word));
		}

		void pattern_set(uint64_t bit, bool v)
		{
			uint32_t qword = bit / 64, sub_word = bit % 64;

			pattern[qword] |= (static_cast<uint64_t>(v) << sub_word);
		}

		void pattern_flip(uint64_t bit)
		{
			uint32_t qword = bit / 64, sub_word = bit % 64;

			pattern[qword] ^= (1ull << sub_word);
		}

	public:
		virtual uint8_t pattern_calc(const field& field, int32_t x, int32_t y) override
		{
			uint64_t bit_result = 0;

			for (int32_t i = 0; i < backref_n; i++)
			{
				bit_result |= ((static_cast<uint64_t>(field(x + backrefs[i].x, y + backrefs[i].y))) << i);
			}

			return pattern_get(bit_result);
		}

		virtual void mutate(int32_t bits) override
		{
			for (int32_t i = 0; i < bits; i++)
			{
				uint64_t bit = rand(0ull, tmp::pow_2<max_backrefs> -1);
				pattern_flip(bit);
			}
		}

		fundamental_progressive_pattern(const backref_t& backrefs_, const pattern_t& pattern_) : backrefs(backrefs_), pattern(pattern_) {};
		fundamental_progressive_pattern() {};

		void change_backrefs(const backref_t& backrefs_, int32_t n)
		{
			backref_n = n;
			backrefs = backrefs_;
		}

		void change_pattern(const pattern_t& pattern_)
		{
			pattern = pattern_;
		}

	};

	




}