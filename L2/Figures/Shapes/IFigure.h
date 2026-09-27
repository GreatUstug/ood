// Shapes/Shape.h
#pragma once
#include "Figures/IShapeGeometry.h"
#include "../Observer/IFigureObserver.h"
#include "../Observer/ObserverList.h"

#include <memory>
#include <string>

namespace shapes
{
class IFigure
{
public:
	IFigure(const std::string& id,
		  const std::string& color,
		  std::unique_ptr<IShapeGeometry> geometry)
		: m_id(id), m_colorRGB(gfx::Color::ParseToRGB(color)), m_geometry(std::move(geometry))
	{
	}

	void SetGeometry(std::unique_ptr<IShapeGeometry> geometry) {
		m_geometry = std::move(geometry);
		NotifyObservers();
	}

	void SetColor(const std::string& color)
	{
		m_colorRGB = gfx::Color::ParseToRGB(color);
		NotifyObservers();
	}

	void Draw(gfx::ICanvas& canvas) const {
		canvas.SetColor(m_colorRGB);
		m_geometry->Draw(canvas);
	}

	void Move(double dx, double dy)
	{
		m_geometry->Move(dx, dy);
		NotifyObservers();
	}

	const std::string& GetId() const { return m_id; }

	std::string GetInfo() const {
		std::string geo = m_geometry->GetInfo();
		auto sp = geo.find(' ');
		return geo.substr(0, sp) + " " + m_id + " " + m_colorRGB.ParseToString() + geo.substr(sp);
	}

	void AddObserver(IFigureObserver* obs)    { m_observers.AddObserver(obs); }
	void RemoveObserver(IFigureObserver* obs) { m_observers.RemoveObserver(obs); };
private:
	void NotifyObservers() {
		m_observers.Notify([](IFigureObserver* o) {
			o->OnShapeChanged();
		});
	}
	std::string m_id;
	gfx::Color m_colorRGB;
	std::unique_ptr<IShapeGeometry> m_geometry;
	ObserverList<IFigureObserver> m_observers;
};
}
