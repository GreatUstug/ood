//
// Created by maxim on 16.11.2026.
//

#ifndef L5_COLOR_H
#define L5_COLOR_H
#include <stdexcept>

struct Color
{
	unsigned char r, g, b, a;

	Color(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 255)
		: r(r), g(g), b(b), a(a)
	{
	}

	bool operator==(const Color& other) const
	{

		return r == other.r
			&& g == other.g
			&& b == other.b
			&& a == other.a;
	}

	bool operator!=(const Color& other) const
	{
		return !(*this == other);
	}

	static Color ParseToRGB(const std::string& str)
	{
		if (str.empty() || str[0] != '#' || str.length() != 7)
		{
			throw std::runtime_error("Invalid hex color (expected #RRGGBB): " + str);
		}
		try
		{
			unsigned long val = std::stoul(str.substr(1), nullptr, 16);
			return Color(
				static_cast<unsigned char>((val >> 16) & 0xFF),
				static_cast<unsigned char>((val >> 8) & 0xFF),
				static_cast<unsigned char>(val & 0xFF)
				);
		}
		catch (...)
		{
			throw std::runtime_error("Invalid hex color: " + str);
		}
	}

	std::string ParseToString() const
	{
		char buf[8];
		std::snprintf(buf, sizeof(buf), "#%02x%02x%02x", r, g, b);
		return buf;
	}
};


#endif //L5_COLOR_H