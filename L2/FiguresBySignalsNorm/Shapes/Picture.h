// Shapes/Picture.h
#pragma once
#include "IFigure.h"
#include "Figures/IShapeGeometry.h"
#include "../gfx/ICanvas.h"
#include "../Observer/IPictureObserver.h"
#include "../Observer/Subscription.h"
#include "../Observer/Event.h"

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

namespace shapes
{
class Picture : public IFigureObserver
{
public:
	Picture()
	{
	}

	Event<void(const shapes::IFigure&, std::size_t)> ShapeAdded;
	Event<void(const std::string&, std::size_t)> ShapeRemoved;
	Event<void(const shapes::IFigure&, double, double)> ShapeMoved;
	Event<void(const shapes::IFigure&, const std::string&)> ShapeColorChanged;
	Event<void(const shapes::IFigure&)> ShapeGeometryChanged;

	auto SubscribeToShapeAdded(Event<void(const shapes::IFigure&, std::size_t)>::Handler h)
	{
		return ShapeAdded.Subscribe(std::move(h));
	}

	auto SubscribeToShapeRemoved(Event<void(const std::string&, std::size_t)>::Handler h)
	{
		return ShapeRemoved.Subscribe(std::move(h));
	}

	auto SubscribeToShapeMoved(Event<void(const shapes::IFigure&, double, double)>::Handler h)
	{
		return ShapeMoved.Subscribe(std::move(h));
	}

	auto SubscribeToShapeColorChanged(Event<void(const shapes::IFigure&, const std::string&)>::Handler h)
	{
		return ShapeColorChanged.Subscribe(std::move(h));
	}

	auto SubscribeToShapeGeometryChanged(Event<void(const shapes::IFigure&)>::Handler h)
	{
		return ShapeGeometryChanged.Subscribe(std::move(h));
	}

	~Picture()
	{
		for (auto* sub : m_subscriptions)
		{
			sub->MarkSubjectDead();
		}
	}

	std::unique_ptr<Subscription<Picture, IPictureObserver> >
	Subscribe(IPictureObserver* observer)
	{
		m_observers.AddObserver(observer);
		return std::make_unique<Subscription<Picture, IPictureObserver> >(this, observer);
	}

	void RegisterSubscription(Subscription<Picture, IPictureObserver>* sub)
	{
		m_subscriptions.push_back(sub);
	}

	void UnregisterSubscription(Subscription<Picture, IPictureObserver>* sub)
	{
		m_subscriptions.erase(
			std::remove(m_subscriptions.begin(), m_subscriptions.end(), sub),
			m_subscriptions.end());
	}

	void AddShape(std::unique_ptr<IFigure> figure)
	{
		const std::string id = figure->GetId();
		if (m_shapes.contains(id))
		{
			throw std::invalid_argument("Shape with this ID already exists");
		}
		figure->AddObserver(this);
		m_order.push_back(id);
		m_shapes[id] = std::move(figure);
		ShapeAdded.Notify(*m_shapes[id], m_shapes.size());
		NotifyPictureObservers();
	}

	void MoveShape(const std::string& id, double dx, double dy)
	{
		auto it = m_shapes.find(id);
		if (it == m_shapes.end())
			throw std::invalid_argument("Shape not found. MoveShape isn't available.");
		it->second->Move(dx, dy);
	}

	void MovePicture(double dx, double dy)
	{
		for (auto& [id, shape] : m_shapes)
		{
			shape->Move(dx, dy);
		}
	}

	void DeleteShape(const std::string& id)
	{
		if (!m_shapes.contains(id))
			throw std::invalid_argument("Shape not found. DeleteShape isn't available");
		m_shapes.find(id)->second->RemoveObserver(this);
		m_shapes.erase(id);
		m_order.erase(std::remove(m_order.begin(), m_order.end(), id), m_order.end());
		ShapeRemoved.Notify(id, m_shapes.size());
		NotifyPictureObservers();
	}

	void EditShapeColor(const std::string& id, const std::string& color)
	{
		auto it = m_shapes.find(id);
		if (it == m_shapes.end())
			throw std::invalid_argument("Shape not found. Edit color of this shape isn't available");
		it->second->SetColor(color);
	}

	void ChangeShape(const std::string& id, std::unique_ptr<IShapeGeometry> geometry)
	{
		auto it = m_shapes.find(id);
		if (it == m_shapes.end())
			throw std::invalid_argument("Shape not found. ChangeShape isn't available");
		it->second->SetGeometry(std::move(geometry));
	}

	std::vector<std::string> ListAllShapes() const
	{
		std::vector<std::string> result;
		int index = 1;
		for (const auto& id : m_order)
		{
			auto it = m_shapes.find(id);
			if (it != m_shapes.end())
			{
				result.push_back(std::to_string(index++) + " " + it->second->GetInfo());
			}
		}
		return result;
	}

	void DrawShape(const std::string& id, gfx::ICanvas& canvas) const
	{
		auto it = m_shapes.find(id);
		if (it == m_shapes.end())
			throw std::invalid_argument("Shape with this id not found. Draw isn't available");
		it->second->Draw(canvas);
	}

	void DrawPicture(gfx::ICanvas& canvas) const
	{
		for (const auto& id : m_order)
		{
			auto it = m_shapes.find(id);
			if (it != m_shapes.end())
				it->second->Draw(canvas);
		}
	}

	IFigure& GetShape(const std::string& id)
	{
		auto it = m_shapes.find(id);
		if (it == m_shapes.end())
			throw std::invalid_argument("Shape with this id not found.");
		return *it->second;
	}

	void UnsubscribeObserver(IPictureObserver* observer)
	{
		m_observers.RemoveObserver(observer);
	}

	void AddObserver(IPictureObserver* observer) { m_observers.AddObserver(observer); }
	void RemoveObserver(IPictureObserver* observer) { m_observers.RemoveObserver(observer); }

	void OnShapeMoved(const shapes::IFigure& f, double dx, double dy) override
	{
		ShapeMoved.Notify(f, dx, dy);
		NotifyPictureObservers();
	}

	void OnShapeColorChanged(const shapes::IFigure& f, const std::string& c) override
	{
		ShapeColorChanged.Notify(f, c);
		NotifyPictureObservers();
	}

	void OnShapeGeometryChanged(const shapes::IFigure& f) override
	{
		ShapeGeometryChanged.Notify(f);
		NotifyPictureObservers();
	}

private:
	void NotifyPictureObservers()
	{
		m_observers.Notify([this](IPictureObserver* o) {
			o->OnPictureChanged(m_shapes.size());
		});
	}

	std::unordered_map<std::string, std::unique_ptr<IFigure> > m_shapes;
	std::vector<std::string> m_order;
	ObserverList<IPictureObserver> m_observers;
	std::vector<Subscription<Picture, IPictureObserver>*> m_subscriptions;

};
}