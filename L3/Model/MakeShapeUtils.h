//
// Created by maxim on 10.10.2026.
//

#ifndef L3_SHAPEFACTORY_H
#define L3_SHAPEFACTORY_H

#include "Figure.h"
#include "Figures/Rectangle.h"
#include "Figures/Ellipse.h"
#include "Figures/Triangle.h"

#include <memory>
#include <string>
#include <random>

namespace shapes
{

namespace defaults
{
constexpr double RECT_W = 120.0;
constexpr double RECT_H = 80.0;
constexpr double ELLIPSE_RX = 60.0;
constexpr double ELLIPSE_RY = 40.0;
constexpr double TRIANGLE_W = 120.0;
constexpr double TRIANGLE_H = 80.0;
constexpr const char* COLOR = "#c678ff";
}

inline std::string MakeId(const std::string& prefix)
{
	static std::mt19937 rng{ std::random_device{}() };
	return prefix + std::to_string(rng());
}

inline std::unique_ptr<Figure> MakeRectangle(const std::string& id, const Bounds& bounds)
{
	return std::make_unique<Figure>(id, defaults::COLOR,
		std::make_unique<Rectangle>(bounds.x, bounds.y, bounds.width, bounds.height);
}

inline std::unique_ptr<Figure> MakeEllipse(const std::string& id, const Bounds& bounds)
{
	return std::make_unique<Figure>(id, defaults::COLOR,
		std::make_unique<Ellipse>(bounds.x + bounds.width / 2.0,
			bounds.y + bounds.height / 2.0,
			bounds.width / 2.0,
			bounds.height / 2.0));
}

inline std::unique_ptr<Figure> MakeTriangle(const std::string& id, const Bounds& bounds)
{
	double top = bounds.y;
	double bot = bounds.y + bounds.height;
	return std::make_unique<Figure>(id, defaults::COLOR,
		std::make_unique<Triangle>(
			bounds.x + bounds.width / 2.0, top,
			bounds.x + bounds.width, bot,
			bounds.x, bot));
}

inline std::unique_ptr<Figure> MakeShape(const std::string& type,
	const std::string& id,
	const Bounds& bounds)
{
	if (type == "rectangle")
		return MakeRectangle(id, bounds);
	if (type == "ellipse")
		return MakeEllipse(id, bounds);
	if (type == "triangle")
		return MakeTriangle(id, bounds);
	return nullptr;
}

}

#endif //L3_SHAPEFACTORY_H