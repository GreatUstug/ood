// Shapes/Picture.h
#pragma once
#include "Bounds.h"
#include "IFigure.h"
#include "Figures/IShapeGeometry.h"
#include "../gfx/ICanvas.h"

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

namespace shapes
{
	class Picture {
public:
		Picture() {}

		void AddShape(std::unique_ptr<IFigure> figure) {
			const std::string id = figure->GetId();
			if (m_shapes.contains(id)) {
				throw std::invalid_argument("Shape with this ID already exists");
			}
			m_order.push_back(id);
			m_shapes[id] = std::move(figure);
		}

    void MoveShape(const std::string& id, double dx, double dy) {
        auto it = m_shapes.find(id);
        if (it == m_shapes.end()) throw std::invalid_argument("Shape not found. MoveShape isn't available.");
        it->second->Move(dx, dy);
    }

    void MovePicture(double dx, double dy) {
        for (auto& [id, shape] : m_shapes) {
            shape->Move(dx, dy);
        }
    }

    void DeleteShape(const std::string& id) {
        if (!m_shapes.contains(id)) throw std::invalid_argument("Shape not found. DeleteShape isn't available");
        m_shapes.erase(id);
        m_order.erase(std::remove(m_order.begin(), m_order.end(), id), m_order.end());
    }

    void EditShapeColor(const std::string& id, const std::string& color) {
        auto it = m_shapes.find(id);
        if (it == m_shapes.end()) throw std::invalid_argument("Shape not found. Edit color of this shape isn't available");
        it->second->SetColor(color);
    }

	void ChangeShape(const std::string& id, std::unique_ptr<IShapeGeometry> geometry) {
    	auto it = m_shapes.find(id);
    	if (it == m_shapes.end()) throw std::invalid_argument("Shape not found. ChangeShape isn't available");
    	it->second->SetGeometry(std::move(geometry));
    }

    std::vector<std::string> GetAllShapesInfo() const {
        std::vector<std::string> result;
        for (const auto& id : m_order) {
            auto it = m_shapes.find(id);
            if (it != m_shapes.end()) {
                result.push_back(it->second->GetInfo());
            }
        }
        return result;
    }

	void DrawShape(const std::string& id, gfx::ICanvas& canvas) const {
    	auto it = m_shapes.find(id);
    	if (it == m_shapes.end()) throw std::invalid_argument("Shape with this id not found. Draw isn't available");
    	it->second->Draw(canvas);
    }

	void DrawPicture(gfx::ICanvas& canvas) const {
    	for (const auto& id : m_order) {
    		auto it = m_shapes.find(id);
    		if (it != m_shapes.end()) it->second->Draw(canvas);
    	}
    }

	IFigure& GetShape(const std::string& id) const{
		auto it = m_shapes.find(id);
		if (it == m_shapes.end()) throw std::invalid_argument("Shape with this id not found.");
		return *it->second;
	}
	bool HasShape(const std::string& id) const {
		return m_shapes.contains(id);
	}
		void SetShapeBounds(const std::string& id, const Bounds	& b) {
			auto it = m_shapes.find(id);
			if (it == m_shapes.end()) throw std::invalid_argument("Shape not found");
			it->second->SetBounds(b);
		}
	std::string HitTest(double x, double y) const {
		for (auto it = m_order.rbegin(); it != m_order.rend(); ++it) {
			auto shapeIt = m_shapes.find(*it);
			if (shapeIt != m_shapes.end() && shapeIt->second->HitTest(x, y)) {
				return *it;
			}
		}
		return {};
	}
	void ReplaceWith(Picture&& other) {
		m_shapes = std::move(other.m_shapes);
		m_order  = std::move(other.m_order);
	}
private:
		std::unordered_map<std::string, std::unique_ptr<IFigure>> m_shapes;
		std::vector<std::string> m_order;
};
}