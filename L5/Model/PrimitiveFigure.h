// Shapes/Shape.h
#pragma once
#include "Figures/IShapeGeometry.h"
#include "View/gfx/ICanvas.h"

#include <memory>
#include <string>

namespace shapes
{
class PrimitiveFigure
{
public:
	PrimitiveFigure(const std::string& id,
		const std::string& fillColor,
		std::unique_ptr<IShapeGeometry> geometry,
		const std::string& strokeColor = "#000000")
		: m_id(id), m_fillColor(gfx::Color::ParseToRGB(fillColor)), m_strokeColor(gfx::Color::ParseToRGB(strokeColor)), m_geometry(std::move(geometry))
	{
	}

	virtual ~PrimitiveFigure() = default;

	void SetGeometry(std::unique_ptr<IShapeGeometry> geometry)
	{
		m_geometry = std::move(geometry);
	}

	void Move(double dx, double dy)
	{
		m_geometry->Move(dx, dy);
	}

	const std::string& GetId() const { return m_id; }

	std::string GetInfo() const
	{
		std::string geo = m_geometry->GetInfo();
		auto sp = geo.find(' ');
		return geo.substr(0, sp) + " " + m_id + " "
			 + m_fillColor.ParseToString() + " " + m_strokeColor.ParseToString()
			 + geo.substr(sp) + " " + std::to_string(m_time) + " " + std::to_string(m_duration);
	}

	Bounds GetBounds() const
	{
		return m_geometry->GetBounds();
	}

	void SetBounds(const Bounds& bounds)
	{
		m_geometry->SetBounds(bounds);
	}

	bool HitTest(double x, double y) const
	{
		return m_geometry->HitTest(x, y);
	}

	std::string GetType() const
	{
		return m_geometry->GetType();
	}

	void Accept(IShapeGeometryVisitor& v) const
	{
		m_geometry->Accept(v);
	}

	gfx::Color GetFillColor() const
	{
		return m_fillColor;
	}

	gfx::Color GetStrokeColor() const
	{
		return m_strokeColor;
	}

	void SetFillColor(const gfx::Color& color) { m_fillColor = color; }
	void SetStrokeColor(const gfx::Color& color) { m_strokeColor = color; }

	double GetTime() const { return m_time; }
	void SetTime(double t) { m_time = t; }
	double GetDuration() const { return m_duration; }
	void SetDuration(double d) { m_duration = d; }
	const IShapeGeometry* GetGeometry() const { return m_geometry.get(); }

private:
	std::string m_id;
	std::unique_ptr<IShapeGeometry> m_geometry;
	double m_duration = 1.0;
	gfx::Color m_fillColor;
	gfx::Color m_strokeColor;
	double m_time = 0.0;
};
}