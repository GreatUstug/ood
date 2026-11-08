#pragma once
#include "Figure.h"
#include "Figures/IShapeGeometry.h"
#include "View/gfx/ICanvas.h"
#include <memory>
#include <string>

namespace shapes
{
class PrimitiveFigure : public Figure
{
public:
	PrimitiveFigure(const std::string& id,
		const std::string& fillColor,
		std::unique_ptr<IShapeGeometry> geometry,
		const std::string& strokeColor = "#000000")
		: m_id(id), m_fillColor(gfx::Color::ParseToRGB(fillColor)),
		  m_strokeColor(gfx::Color::ParseToRGB(strokeColor)), m_geometry(std::move(geometry))
	{
	}

	virtual ~PrimitiveFigure() = default;

	void SetGeometry(std::unique_ptr<IShapeGeometry> geometry) { m_geometry = std::move(geometry); }

	const std::string& GetId() const override { return m_id; }

	void Move(double dx, double dy) override { m_geometry->Move(dx, dy); }

	Bounds GetBounds() const override { return m_geometry->GetBounds(); }
	void SetBounds(const Bounds& bounds) override { m_geometry->SetBounds(bounds); }
	bool HitTest(double x, double y) const override { return m_geometry->HitTest(x, y); }
	std::string GetType() const override { return m_geometry->GetType(); }
	void Accept(IShapeGeometryVisitor& v) const override { m_geometry->Accept(v); }

	gfx::Color GetFillColor() const override { return m_fillColor; }
	gfx::Color GetStrokeColor() const override { return m_strokeColor; }
	void SetFillColor(const gfx::Color& color) override { m_fillColor = color; }
	void SetStrokeColor(const gfx::Color& color) override { m_strokeColor = color; }

	double GetTime() const override { return m_time; }
	void SetTime(double t) override { m_time = t; }
	double GetDuration() const override { return m_duration; }
	void SetDuration(double d) override { m_duration = d; }

	std::string GetInfo() const override
	{
		std::string geo = m_geometry->GetInfo();
		auto sp = geo.find(' ');
		return geo.substr(0, sp) + " " + m_id + " "
			 + m_fillColor.ParseToString() + " " + m_strokeColor.ParseToString()
			 + geo.substr(sp) + " " + std::to_string(m_time) + " " + std::to_string(m_duration);
	}

	std::unique_ptr<Figure> Clone() const override
	{
		auto clonedGeo = m_geometry->Clone();
		auto clonedFig = std::make_unique<PrimitiveFigure>(
			m_id, m_fillColor.ParseToString(), std::move(clonedGeo), m_strokeColor.ParseToString()
		);
		clonedFig->SetTime(m_time);
		clonedFig->SetDuration(m_duration);
		return clonedFig;
	}

	const IShapeGeometry* GetGeometry() const { return m_geometry.get(); }

private:
	std::string m_id;
	std::unique_ptr<IShapeGeometry> m_geometry;
	gfx::Color m_fillColor;
	gfx::Color m_strokeColor;
	double m_time = 0.0;
	double m_duration = 1.0;
};
}