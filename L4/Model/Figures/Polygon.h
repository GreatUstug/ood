#ifndef FIGURES_POLYGON_H
#define FIGURES_POLYGON_H
#include "IShapeGeometry.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>
#include <vector>

namespace shapes
{
class Polygon : public IShapeGeometry
{
public:
    Polygon(double cx, double cy, double radius, int sides, double rotation = 0.0)
        : m_cx(cx), m_cy(cy), m_radius(radius),
          m_sides(std::max(3, sides)), m_rotation(rotation) {}

    ~Polygon() override = default;

    std::string GetInfo() const override
    {
        return "polygon " + std::to_string(m_cx) + " " + std::to_string(m_cy) + " " +
                          std::to_string(m_radius) + " " + std::to_string(m_sides) + " " +
                          std::to_string(m_rotation);
    }

    void Move(double dx, double dy) override { m_cx += dx; m_cy += dy; }

    Bounds GetBounds() const override
    { return { m_cx - m_radius, m_cy - m_radius, 2*m_radius, 2*m_radius }; }

    std::string GetType() const override { return "polygon"; }

    void SetBounds(const Bounds& b) override
    {
        m_cx = b.x + b.width / 2.0;
        m_cy = b.y + b.height / 2.0;
        m_radius = std::min(b.width, b.height) / 2.0;
    }

    bool HitTest(double px, double py) const override
    {
        auto v = Vertices();
        int n = (int)v.size();
        bool inside = false;
        for (int i = 0, j = n - 1; i < n; j = i++)
        {
            double xi = v[i].first,  yi = v[i].second;
            double xj = v[j].first,  yj = v[j].second;
            if (((yi > py) != (yj > py)) &&
                (px < (xj - xi) * (py - yi) / (yj - yi + 1e-12) + xi))
                inside = !inside;
        }
        return inside;
    }

    void Accept(IShapeGeometryVisitor& v) const override { v.Visit(*this); }

    std::vector<std::pair<double,double>> Vertices() const
    {
        constexpr double PI = 3.14159265358979323846;
        std::vector<std::pair<double,double>> v;
        v.reserve(m_sides);
        double step = 2.0 * PI / m_sides;
        double start = -PI / 2.0 + m_rotation;
        for (int i = 0; i < m_sides; ++i)
        {
            double a = start + i * step;
            v.emplace_back(m_cx + m_radius * std::cos(a),
                           m_cy + m_radius * std::sin(a));
        }
        return v;
    }

    double GetCx() const { return m_cx; }
    double GetCy() const { return m_cy; }
    double GetRadius() const { return m_radius; }
    int    GetSides() const { return m_sides; }
    double GetRotation() const { return m_rotation; }

private:
    double m_cx = 0, m_cy = 0, m_radius = 50;
    int    m_sides = 6;
    double m_rotation = 0;
};
}
#endif //FIGURES_POLYGON_H