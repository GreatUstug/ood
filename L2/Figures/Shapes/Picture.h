// Shapes/Picture.h
#pragma once
#include "IFigure.h"
#include "Figures/IShapeGeometry.h"
#include "../gfx/ICanvas.h"
#include "../Observer/IPictureObserver.h"

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <unordered_map>

namespace shapes
{
	class Picture : public IFigureObserver {
public:
		Picture() {}
		void AddShape(std::unique_ptr<IFigure> figure) {
			const std::string id = figure->GetId();
			if (m_shapes.contains(id)) {
				throw std::runtime_error("Shape with this ID already exists");
			}
			figure->AddObserver(this);
			m_order.push_back(id);
			m_shapes[id] = std::move(figure);
			NotifyPictureObservers();
		}

    void MoveShape(const std::string& id, double dx, double dy) {
        auto it = m_shapes.find(id);
        if (it == m_shapes.end()) throw std::runtime_error("Shape not found");
        it->second->Move(dx, dy);
    }

    void MovePicture(double dx, double dy) {
        for (auto& [id, shape] : m_shapes) {
            shape->Move(dx, dy);
        }
    }

    void DeleteShape(const std::string& id) {
        if (!m_shapes.contains(id)) throw std::runtime_error("Shape not found");
			m_shapes.find(id)->second->RemoveObserver(this);
        m_shapes.erase(id);
        m_order.erase(std::remove(m_order.begin(), m_order.end(), id), m_order.end());
		NotifyPictureObservers();
    }

    void EditShapeColor(const std::string& id, const std::string& color) {
        auto it = m_shapes.find(id);
        if (it == m_shapes.end()) throw std::runtime_error("Shape not found");
        it->second->SetColor(color);
    }

	void ChangeShape(const std::string& id, std::unique_ptr<IShapeGeometry> geometry) {
    	auto it = m_shapes.find(id);
    	if (it == m_shapes.end()) throw std::runtime_error("Shape not found");
    	it->second->SetGeometry(std::move(geometry));
    }

    std::vector<std::string> ListAllShapes() const {
        std::vector<std::string> result;
        int index = 1;
        for (const auto& id : m_order) {
            auto it = m_shapes.find(id);
            if (it != m_shapes.end()) {
                result.push_back(std::to_string(index++) + " " + it->second->GetInfo());
            }
        }
        return result;
    }

		void DrawShape(const std::string& id, gfx::ICanvas& canvas) const {
    	auto it = m_shapes.find(id);
    	if (it == m_shapes.end()) throw std::runtime_error("Shape not found");
    	it->second->Draw(canvas);
    }

		void DrawPicture(gfx::ICanvas& canvas) const {
    	for (const auto& id : m_order) {
    		auto it = m_shapes.find(id);
    		if (it != m_shapes.end()) it->second->Draw(canvas);
    	}
    }

		IFigure& GetShape(const std::string& id) {
			auto it = m_shapes.find(id);
			if (it == m_shapes.end()) throw std::runtime_error("Shape not found");
			return *it->second;
		}


		void AddObserver(IPictureObserver* observer)    { m_observers.AddObserver(observer); }
		void RemoveObserver(IPictureObserver* observer) { m_observers.RemoveObserver(observer); }
		void OnShapeChanged() override {
			NotifyPictureObservers();
		}

private:
		void NotifyPictureObservers() {
			m_observers.Notify([this](IPictureObserver* o) {
				o->OnPictureChanged(m_shapes.size());
			});
		}
    std::unordered_map<std::string, std::unique_ptr<IFigure>> m_shapes;
    std::vector<std::string> m_order;
		ObserverList<IPictureObserver> m_observers;
};
}