#ifndef FIGURES_SFMLCANVAS_H
#define FIGURES_SFMLCANVAS_H
#pragma once
#include "../gfx/ICanvas.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

namespace gfx
{
class SFMLCanvas : public ICanvas
{
public:
	SFMLCanvas(sf::RenderWindow& window)
		: m_window(window),
		  m_currentPos{ 0.f, 0.f }
	{
	}

	~SFMLCanvas() override = default;

	void DrawEllipse(double cx, double cy, double rx, double ry,
		const gfx::Color& fill, const gfx::Color& outline) override
	{
		float maxR = std::max(F(rx), F(ry));
		if (maxR <= 0)
			return;

		sf::CircleShape circle(maxR);
		circle.setPointCount(100);
		circle.setOrigin({ maxR, maxR });
		circle.setPosition({ F(cx), F(cy) });
		circle.setScale({ F(rx) / maxR, F(ry) / maxR });
		circle.setFillColor(sf::Color(fill.r, fill.g, fill.b, fill.a));
		circle.setOutlineColor(sf::Color(outline.r, outline.g, outline.b, outline.a));
		circle.setOutlineThickness(outline.a > 0 ? 1.f : 0.f);
		m_window.draw(circle);
	}

	void DrawRectangle(double x, double y, double w, double h,
		const gfx::Color& fill, const gfx::Color& outline) override
	{
		sf::RectangleShape rect({ F(w), F(h) });
		rect.setPosition({ F(x), F(y) });
		rect.setFillColor(sf::Color(fill.r, fill.g, fill.b, fill.a));
		rect.setOutlineColor(sf::Color(outline.r, outline.g, outline.b, outline.a));
		rect.setOutlineThickness(outline.a > 0 ? 1.f : 0.f);
		m_window.draw(rect);
	}

	void DrawTriangle(double x1, double y1, double x2, double y2,
		double x3, double y3,
		const gfx::Color& fill, const gfx::Color& outline) override
	{
		sf::ConvexShape tri(3);
		tri.setPoint(0, { F(x1), F(y1) });
		tri.setPoint(1, { F(x2), F(y2) });
		tri.setPoint(2, { F(x3), F(y3) });
		tri.setFillColor(sf::Color(fill.r, fill.g, fill.b, fill.a));
		tri.setOutlineColor(sf::Color(outline.r, outline.g, outline.b, outline.a));
		tri.setOutlineThickness(outline.a > 0 ? 1.f : 0.f);
		m_window.draw(tri);
	}

	void DrawLine(double x1, double y1, double x2, double y2,
		const gfx::Color& color) override
	{
		sf::Vertex line[2] = {
			{ sf::Vector2f(F(x1), F(y1)), sf::Color(color.r, color.g, color.b, color.a) },
			{ sf::Vector2f(F(x2), F(y2)), sf::Color(color.r, color.g, color.b, color.a) }
		};
		m_window.draw(line, 2, sf::PrimitiveType::Lines);
	}

	void DrawPolygon(const std::vector<std::pair<double, double> >& points,
		const gfx::Color& fill, const gfx::Color& outline) override
	{
		if (points.size() < 3)
			return;
		sf::ConvexShape shape(points.size());
		for (std::size_t i = 0; i < points.size(); ++i)
			shape.setPoint(i, sf::Vector2f(F(points[i].first), F(points[i].second)));
		shape.setFillColor(sf::Color(fill.r, fill.g, fill.b, fill.a));
		shape.setOutlineColor(sf::Color(outline.r, outline.g, outline.b, outline.a));
		shape.setOutlineThickness(outline.a > 0 ? 1.5f : 0.f);
		m_window.draw(shape);
	}

private:
	static float F(double v) { return static_cast<float>(v); }
	static sf::Vector2f V(double x, double y) { return { F(x), F(y) }; }
	sf::RenderWindow& m_window;
	sf::Vector2f m_currentPos;
};
}
#endif //FIGURES_SFMLCANVAS_H