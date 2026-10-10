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

	void DrawEllipse(double cx, double cy, double rx, double ry, bool isTransparent = false) override {
    	float maxR = std::max(F(rx), F(ry));
    	if (maxR <= 0) return;

    	sf::CircleShape circle(maxR);
    	circle.setPointCount(100);
    	circle.setOrigin({maxR, maxR});
    	circle.setPosition({F(cx), F(cy)});
    	circle.setScale({F(rx) / maxR, F(ry) / maxR});
    	circle.setFillColor(!isTransparent ? m_currentColor : sf::Color::Transparent);
    	circle.setOutlineColor(m_currentColor);
    	circle.setOutlineThickness(1.0f);
    	m_window.draw(circle);
    }

	void DrawRectangle(double x, double y, double w, double h, bool isTransparent = false) override {
    	sf::RectangleShape rectangle({static_cast<float>(w), static_cast<float>(h)});
    	rectangle.setPosition({static_cast<float>(x), static_cast<float>(y)});
    	rectangle.setFillColor(!isTransparent ? m_currentColor : sf::Color::Transparent);
    	rectangle.setOutlineColor(m_currentColor);
    	rectangle.setOutlineThickness(1.f);
    	m_window.draw(rectangle);
    }

	void DrawTriangle(double x1, double y1, double x2, double y2,
					  double x3, double y3, bool isTransparent = false) override {
    	sf::ConvexShape triangle(3);
    	triangle.setPoint(0, {static_cast<float>(x1), static_cast<float>(y1)});
    	triangle.setPoint(1, {static_cast<float>(x2), static_cast<float>(y2)});
    	triangle.setPoint(2, {static_cast<float>(x3), static_cast<float>(y3)});
    	triangle.setFillColor(!isTransparent ? m_currentColor : sf::Color::Transparent);;
    	triangle.setOutlineColor(m_currentColor);
    	triangle.setOutlineThickness(1.f);
    	m_window.draw(triangle);
    }

private:
	static float F(double v) { return static_cast<float>(v); }
	static sf::Vector2f V(double x, double y) { return {F(x), F(y)}; }
    sf::RenderWindow& m_window;
    sf::Vector2f m_currentPos;
    sf::Color m_currentColor;
};
#endif //FIGURES_SFMLCANVAS_H