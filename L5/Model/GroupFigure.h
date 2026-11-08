//
// Created by maxim on 09.11.2026.
//

#ifndef L5_GROUPFIGURE_H
#define L5_GROUPFIGURE_H
#include "Figure.h"
#include <vector>
#include <memory>
#include <algorithm>

namespace shapes
{
class GroupFigure : public Figure
{
public:
	GroupFigure(const std::string& id, const std::string& fillColor, const std::string& strokeColor)
		: m_id(id), m_fillColor(gfx::Color::ParseToRGB(fillColor)), m_strokeColor(gfx::Color::ParseToRGB(strokeColor))
	{
	}

	const std::string& GetId() const override { return m_id; }
	std::string GetType() const override { return "group"; }

	double GetTime() const override { return m_time; }
	void SetTime(double t) override { m_time = t; }
	double GetDuration() const override { return m_duration; }
	void SetDuration(double d) override { m_duration = d; }

	gfx::Color GetFillColor() const override { return m_fillColor; }
	gfx::Color GetStrokeColor() const override { return m_strokeColor; }

	void SetFillColor(const gfx::Color& c) override {
		m_fillColor = c;
		for (auto& child : m_children) child->SetFillColor(c);
	}
	void SetStrokeColor(const gfx::Color& c) override {
		m_strokeColor = c;
		for (auto& child : m_children) child->SetStrokeColor(c);
	}

	Bounds GetBounds() const override {
		if (m_children.empty()) return {0, 0, 0, 0};
		Bounds total = m_children[0]->GetBounds();
		for (size_t i = 1; i < m_children.size(); ++i) {
			Bounds b = m_children[i]->GetBounds();
			double minX = std::min(total.x, b.x);
			double minY = std::min(total.y, b.y);
			double maxX = std::max(total.x + total.width, b.x + b.width);
			double maxY = std::max(total.y + total.height, b.y + b.height);
			total = { minX, minY, maxX - minX, maxY - minY };
		}
		return total;
	}

	void Move(double dx, double dy) override {
		for (auto& child : m_children) child->Move(dx, dy);
	}

	void SetBounds(const Bounds& newB) override {
		Bounds oldB = GetBounds();
		if (oldB.width == 0 || oldB.height == 0) return;

		double sx = newB.width / oldB.width;
		double sy = newB.height / oldB.height;

		for (auto& child : m_children) {
			Bounds cb = child->GetBounds();
			Bounds newCb;
			newCb.x = newB.x + (cb.x - oldB.x) * sx;
			newCb.y = newB.y + (cb.y - oldB.y) * sy;
			newCb.width = cb.width * sx;
			newCb.height = cb.height * sy;
			child->SetBounds(newCb);
		}
	}

	bool HitTest(double x, double y) const override {
		for (const auto& child : m_children) {
			if (child->HitTest(x, y)) return true;
		}
		return false;
	}

	void Accept(IShapeGeometryVisitor& v) const override {
		v.Visit(*this); // Visitor сам рекурсивно нарисует детей
	}

	std::string GetInfo() const override {
		return "group " + m_id + " " + m_fillColor.ParseToString() + " " + m_strokeColor.ParseToString()
			 + " " + std::to_string(m_time) + " " + std::to_string(m_duration);
	}

	std::unique_ptr<Figure> Clone() const override {
		auto clonedGroup = std::make_unique<GroupFigure>(m_id, m_fillColor.ParseToString(), m_strokeColor.ParseToString());
		clonedGroup->SetTime(m_time);
		clonedGroup->SetDuration(m_duration);
		for (const auto& child : m_children) {
			clonedGroup->AddChild(child->Clone());
		}
		return clonedGroup;
	}

	void AddChild(std::unique_ptr<Figure> child) { m_children.push_back(std::move(child)); }
	std::unique_ptr<Figure> ExtractChild(const std::string& id) {
		auto it = std::find_if(m_children.begin(), m_children.end(), [&](const auto& c) { return c->GetId() == id; });
		if (it != m_children.end()) {
			auto fig = std::move(*it);
			m_children.erase(it);
			return fig;
		}
		return nullptr;
	}
	const std::vector<std::unique_ptr<Figure>>& GetChildren() const { return m_children; }

private:
	std::string m_id;
	std::vector<std::unique_ptr<Figure>> m_children;
	gfx::Color m_fillColor;
	gfx::Color m_strokeColor;
	double m_time = 0.0;
	double m_duration = 1.0;
};
}
#endif //L5_GROUPFIGURE_H