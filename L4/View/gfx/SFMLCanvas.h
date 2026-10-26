#ifndef FIGURES_SFMLCANVAS_H
#define FIGURES_SFMLCANVAS_H
#pragma once
#include "../gfx/ICanvas.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

//посмотреть по поводу буфера из сфмл
class SFMLCanvas : public gfx::ICanvas
{
public:
	SFMLCanvas(sf::RenderWindow& window)
		: m_window(window),
		  m_currentPos{ 0.f, 0.f }, m_currentColor(0, 0, 0, 255)
	{
	}

	void SetColor(const gfx::Color& c) override
	{
		m_currentColor = sf::Color(c.r, c.g, c.b, c.a);
	}

	void DrawEllipse(double cx, double cy, double rx, double ry, bool isTransparent = false) override
	{
		float maxR = std::max(F(rx), F(ry));
		if (maxR <= 0)
			return;

		sf::CircleShape circle(maxR);
		circle.setPointCount(100);
		circle.setOrigin({ maxR, maxR });
		circle.setPosition({ F(cx), F(cy) });
		circle.setScale({ F(rx) / maxR, F(ry) / maxR });
		circle.setFillColor(!isTransparent ? m_currentColor : sf::Color::Transparent);
		circle.setOutlineColor(m_currentColor);
		circle.setOutlineThickness(1.0f);
		m_window.draw(circle);
	}

	void DrawRectangle(double x, double y, double w, double h, bool isTransparent = false) override
	{
		sf::RectangleShape rectangle({ static_cast<float>(w), static_cast<float>(h) });
		rectangle.setPosition({ static_cast<float>(x), static_cast<float>(y) });
		rectangle.setFillColor(!isTransparent ? m_currentColor : sf::Color::Transparent);
		rectangle.setOutlineColor(m_currentColor);
		rectangle.setOutlineThickness(1.f);
		m_window.draw(rectangle);
	}

	void DrawTriangle(double x1, double y1, double x2, double y2,
		double x3, double y3, bool isTransparent = false) override
	{
		sf::ConvexShape triangle(3);
		triangle.setPoint(0, { static_cast<float>(x1), static_cast<float>(y1) });
		triangle.setPoint(1, { static_cast<float>(x2), static_cast<float>(y2) });
		triangle.setPoint(2, { static_cast<float>(x3), static_cast<float>(y3) });
		triangle.setFillColor(!isTransparent ? m_currentColor : sf::Color::Transparent);;
		triangle.setOutlineColor(m_currentColor);
		triangle.setOutlineThickness(1.f);
		m_window.draw(triangle);
	}

	void DrawLine(double x1, double y1, double x2, double y2) override
	{
		sf::Vertex line[2] = {
			{ sf::Vector2f((float)x1, (float)y1), m_currentColor },
			{ sf::Vector2f((float)x2, (float)y2), m_currentColor }
		};
		m_window.draw(line, 2, sf::PrimitiveType::Lines);
	}

	void DrawPolygon(const std::vector<std::pair<double,double>>& points, bool filled) override
	{
		if (points.size() < 3) return;
		sf::ConvexShape shape(points.size());
		for (size_t i = 0; i < points.size(); ++i)
			shape.setPoint(i, sf::Vector2f((float)points[i].first, (float)points[i].second));

		if (filled)
		{
			shape.setFillColor(m_currentColor);
			shape.setOutlineThickness(0.f);
		}
		else
		{
			shape.setFillColor(sf::Color::Transparent);
			shape.setOutlineColor(m_currentColor);
			shape.setOutlineThickness(1.5f);
		}
		m_window.draw(shape);
	}

private:
	static float F(double v) { return static_cast<float>(v); }
	static sf::Vector2f V(double x, double y) { return { F(x), F(y) }; }
	sf::RenderWindow& m_window;
	sf::Vector2f m_currentPos;
	sf::Color m_currentColor;
};
#endif //FIGURES_SFMLCANVAS_H