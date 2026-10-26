#ifndef FIGURES_LINE_H
#define FIGURES_LINE_H

#include <algorithm>
#include <cmath>
#include <string>
#include "IShapeGeometry.h"

namespace shapes
{
class Line : public IShapeGeometry
{
public:
    Line(double x1, double y1, double x2, double y2)
        : m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2) {}

    ~Line() override = default;

    std::string GetInfo() const override
    {
        return "line " + std::to_string(m_x1) + " " + std::to_string(m_y1) + " " +
                        std::to_string(m_x2) + " " + std::to_string(m_y2);
    }

    void Move(double dx, double dy) override
    { m_x1 += dx; m_y1 += dy; m_x2 += dx; m_y2 += dy; }

    Bounds GetBounds() const override
    {
        return { std::min(m_x1, m_x2), std::min(m_y1, m_y2),
                 std::abs(m_x2 - m_x1), std::abs(m_y2 - m_y1) };
    }

    std::string GetType() const override { return "line"; }

    void SetBounds(const Bounds& b) override
    {
        Bounds old = GetBounds();
        if (old.width == 0 && old.height == 0) return;
        double sx = (old.width  == 0) ? 1.0 : b.width  / old.width;
        double sy = (old.height == 0) ? 1.0 : b.height / old.height;
        auto sc = [&](double& px, double& py) {
            px = b.x + (px - old.x) * sx;
            py = b.y + (py - old.y) * sy;
        };
        sc(m_x1, m_y1); sc(m_x2, m_y2);
    }

    bool HitTest(double px, double py) const override
    {
        constexpr double TH = 6.0;
        double dx = m_x2 - m_x1, dy = m_y2 - m_y1;
        double len2 = dx*dx + dy*dy;
        if (len2 < 1e-12) return std::hypot(px - m_x1, py - m_y1) <= TH;
        double t = std::clamp(((px - m_x1)*dx + (py - m_y1)*dy) / len2, 0.0, 1.0);
        return std::hypot(px - (m_x1 + t*dx), py - (m_y1 + t*dy)) <= TH;
    }

    void Accept(IShapeGeometryVisitor& v) const override { v.Visit(*this); }

    double GetX1() const { return m_x1; }
    double GetY1() const { return m_y1; }
    double GetX2() const { return m_x2; }
    double GetY2() const { return m_y2; }

private:
    double m_x1 = 0, m_y1 = 0, m_x2 = 0, m_y2 = 0;
};
}
#endif //FIGURES_LINE_H