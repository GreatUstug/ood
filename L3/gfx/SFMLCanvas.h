#ifndef FIGURES_SFMLCANVAS_H
#define FIGURES_SFMLCANVAS_H
#pragma once
#include "../gfx/ICanvas.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>


//посмотреть по поводу буфера из сфмл
class SFMLCanvas : public gfx::ICanvas {
public:
    SFMLCanvas(sf::RenderWindow& window)
        : m_window(window),
          m_currentPos{0.f, 0.f}, m_currentColor(0, 0, 0, 255) {}

    void SetColor(const gfx::Color& c) override {
        m_currentColor = sf::Color(c.r, c.g, c.b, c.a);
    }

    void MoveTo(double x, double y) override {
        m_currentPos = V(x, y);
    }

    void LineTo(double x, double y) override {
        sf::Vector2f end = V(x, y);
        m_lines.push_back({m_currentPos, end, m_currentColor});
        m_currentPos = end;
    }

    void DrawEllipse(double cx, double cy, double rx, double ry) override {
    	float maxR = std::max(F(rx), F(ry));
    	if (maxR <= 0) return;
    	m_ellipses.push_back({F(cx), F(cy), F(rx), F(ry), m_currentColor});
    }

    void Render() {
        for (const auto& l : m_lines) {
            sf::Vertex v[] = {
                sf::Vertex(l.a, l.color),
                sf::Vertex(l.b, l.color)
            };
            m_window.draw(v, 2, sf::PrimitiveType::Lines);
        }

        for (const auto& e : m_ellipses) {
            float maxR = std::max(e.rx, e.ry);
            if (maxR <= 0) continue;

            sf::CircleShape circle(maxR);
            circle.setPointCount(100);
            circle.setOrigin({maxR, maxR});
            circle.setPosition({e.cx, e.cy});
            circle.setScale({e.rx / maxR, e.ry / maxR});
            circle.setFillColor(sf::Color::Transparent);
            circle.setOutlineColor(e.color);
            circle.setOutlineThickness(1.0f);
            m_window.draw(circle);
        }

    }

private:
	static float F(double v) { return static_cast<float>(v); }
	static sf::Vector2f V(double x, double y) { return {F(x), F(y)}; }
    sf::RenderWindow& m_window;
    sf::Vector2f m_currentPos;
    sf::Color m_currentColor;

    struct LineRec    { sf::Vector2f a, b; sf::Color color; };
    struct EllipseRec { float cx, cy, rx, ry; sf::Color color; };

    std::vector<LineRec>    m_lines;
    std::vector<EllipseRec> m_ellipses;
};
#endif //FIGURES_SFMLCANVAS_H