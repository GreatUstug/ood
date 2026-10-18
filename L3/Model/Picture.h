// Shapes/Picture.h
#pragma once
#include "Bounds.h"
#include "Figure.h"
#include "ShapeInfo.h"
#include "Figures/IShapeGeometry.h"
#include "../View/gfx/ICanvas.h"
#include "Observer/ObserverList.h"
#include "Observer/PictureObserver.h"

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

namespace shapes
{
class Picture
{
public:
	Picture()
	{
	}

	void Subscribe(PictureObserver* obs) { m_observers.AddObserver(obs); }
	void Unsubscribe(PictureObserver* obs) { m_observers.RemoveObserver(obs); }

	void AddShape(std::unique_ptr<Figure> figure)
	{
		const std::string id = figure->GetId();
		if (m_shapes.contains(id))
		{
			throw std::invalid_argument("Shape with this ID already exists");
		}
		m_order.push_back(id);
		m_shapes[id] = std::move(figure);
		NotifyPictureObservers();
	}

	void MoveShapes(const std::vector<std::string>& ids, double dx, double dy)
	{
		for (const auto& id : ids)
		{
			auto it = m_shapes.find(id);
			if (it == m_shapes.end())
				throw std::invalid_argument("Shape not found. MoveShapes isn't available.");
			it->second->Move(dx, dy);
		}
		NotifyPictureObservers();
	}

	void DeleteShapes(const std::vector<std::string>& ids)
	{
		for (const auto& id : ids)
		{
			if (!m_shapes.contains(id))
			{
				throw std::invalid_argument("Shape not found. DeleteShapes isn't available");
			}
			m_shapes.erase(id);
			m_order.erase(std::remove(m_order.begin(), m_order.end(), id), m_order.end());
		}
		NotifyPictureObservers();
	}

	std::vector<std::string> GetAllShapesInfo() const
	{
		std::vector<std::string> result;
		for (const auto& id : m_order)
		{
			auto it = m_shapes.find(id);
			if (it != m_shapes.end())
			{
				result.push_back(it->second->GetInfo());
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

	Figure& GetShape(const std::string& id) const
	{
		auto it = m_shapes.find(id);
		if (it == m_shapes.end())
			throw std::invalid_argument("Shape with this id not found.");
		return *it->second;
	}

	bool HasShape(const std::string& id) const
	{
		return m_shapes.contains(id);
	}

	void SetShapeBounds(const std::string& id, const Bounds& b)
	{
		auto it = m_shapes.find(id);
		if (it == m_shapes.end())
			throw std::invalid_argument("Shape not found");
		it->second->SetBounds(b);
		NotifyPictureObservers();
	}

	std::string HitTest(double x, double y) const
	{
		for (auto it = m_order.rbegin(); it != m_order.rend(); ++it)
		{
			auto shapeIt = m_shapes.find(*it);
			if (shapeIt != m_shapes.end() && shapeIt->second->HitTest(x, y))
			{
				return *it;
			}
		}
		return {};
	}

	void ReplaceWith(Picture&& other)
	{
		m_shapes = std::move(other.m_shapes);
		m_order = std::move(other.m_order);
		NotifyPictureObservers();
	}

private:
	void NotifyPictureObservers()
	{
		m_observers.Notify([this](PictureObserver* o) {
			o->OnPictureChanged(BuildShapeInfo());
		});
	}

	std::vector<ShapeInfo> BuildShapeInfo() const
	{
		std::vector<ShapeInfo> result;
		result.reserve(m_shapes.size());
		for (const auto& id : m_order)
		{
			auto it = m_shapes.find(id);
			if (it != m_shapes.end())
			{
				result.push_back({
					it->second->GetId(),
					it->second->GetType(),
				});
			}
		}
		return result;
	}

	ObserverList<PictureObserver> m_observers;
	std::unordered_map<std::string, std::unique_ptr<Figure> > m_shapes;
	std::vector<std::string> m_order;
};
}