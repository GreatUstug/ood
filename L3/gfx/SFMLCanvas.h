#ifndef FIGURES_SFMLCANVAS_H
#define FIGURES_SFMLCANVAS_H
#pragma once
#include "../gfx/ICanvas.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

//посмотреть по поводу буфера из сфмл
class SFMLCanvas : public gfx::ICanvas {
public:
    SFMLCanvas(sf::RenderWindow& window)
        : m_window(window),
          m_currentPos{0.f, 0.f}, m_currentColor(0, 0, 0, 255) {}

    void SetColor(const gfx::Color& c) override {
        m_currentColor = sf::Color(c.r, c.g, c.b, c.a);
    }

    // void MoveTo(double x, double y) override {
    //     m_currentPos = V(x, y);
    // }
    //
    // void LineTo(double x, double y) override {
    //     sf::Vector2f end = V(x, y);
    //     m_lines.push_back({m_currentPos, end, m_currentColor});
    //     m_currentPos = end;
    // }

	void DrawEllipse(double cx, double cy, double rx, double ry) override {
    	float maxR = std::max(F(rx), F(ry));
    	if (maxR <= 0) return;

    	sf::CircleShape circle(maxR);
    	circle.setPointCount(100);
    	circle.setOrigin({maxR, maxR});
    	circle.setPosition({F(cx), F(cy)});
    	circle.setScale({F(rx) / maxR, F(ry) / maxR});
    	circle.setFillColor(m_currentColor);
    	circle.setOutlineColor(m_currentColor);
    	circle.setOutlineThickness(1.0f);
    	m_window.draw(circle);
    }

	void DrawRectangle(double x, double y, double w, double h) override {
    	sf::RectangleShape shape({static_cast<float>(w), static_cast<float>(h)});
    	shape.setPosition({static_cast<float>(x), static_cast<float>(y)});
    	shape.setFillColor(m_currentColor);
    	shape.setOutlineColor(m_currentColor);
    	shape.setOutlineThickness(1.f);
    	m_window.draw(shape);
    }

	void DrawTriangle(double x1, double y1, double x2, double y2,
					  double x3, double y3) override {
    	sf::ConvexShape shape(3);
    	shape.setPoint(0, {static_cast<float>(x1), static_cast<float>(y1)});
    	shape.setPoint(1, {static_cast<float>(x2), static_cast<float>(y2)});
    	shape.setPoint(2, {static_cast<float>(x3), static_cast<float>(y3)});
    	shape.setFillColor(m_currentColor);
    	shape.setOutlineColor(m_currentColor);
    	shape.setOutlineThickness(1.f);
    	m_window.draw(shape);
    }

private:
	static float F(double v) { return static_cast<float>(v); }
	static sf::Vector2f V(double x, double y) { return {F(x), F(y)}; }
    sf::RenderWindow& m_window;
    sf::Vector2f m_currentPos;
    sf::Color m_currentColor;
};
#endif //FIGURES_SFMLCANVAS_H