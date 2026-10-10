//
// Created by maxim on 14.09.2026.
//

#ifndef FIGURES_ICANVAS_H
#define FIGURES_ICANVAS_H

// gfx/ICanvas.h
#pragma once
#include <string>
#include <algorithm>
#include <stdexcept>

namespace gfx {
    struct Color {
        unsigned char r, g, b, a;

        Color(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 255)
            : r(r), g(g), b(b), a(a) {}

    	static Color ParseToRGB(const std::string& str) {
        	if (str.empty() || str[0] != '#' || str.length() != 7) {
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
        	} catch (...)
        	{
        		throw std::runtime_error("Invalid hex color: " + str);
        	}
        }
    	std::string ParseToString() const {
        	char buf[8];
        	std::snprintf(buf, sizeof(buf), "#%02x%02x%02x", r, g, b);
        	return buf;
        }
    };

    class ICanvas {
    public:
        virtual ~ICanvas() = default;
        virtual void SetColor(const Color& c) = 0;
        // virtual void MoveTo(double x, double y) = 0;
        // virtual void LineTo(double x, double y) = 0;
        virtual void DrawEllipse(double cx, double cy, double rx, double ry, bool isTransparent = false) = 0;
    	virtual void DrawRectangle(double x, double y, double w, double h, bool isTransparent = false) = 0;
    	virtual void DrawTriangle(double x1, double y1,
								  double x2, double y2,
								  double x3, double y3, bool isTransparent = false) = 0;
    };
}
#endif //FIGURES_ICANVAS_H