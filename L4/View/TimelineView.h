#ifndef L4_TIMELINEVIEW_H
#define L4_TIMELINEVIEW_H
#include "Commands/ICommand.h"
#include "Commands/SetTimeCommand.h"
#include "Commands/ReorderCommand.h"
#include "Commands/SetDurationCommand.h"
#include "Model/Picture.h"
#include "Model/EditorState.h"
#include "Model/ShapeInfo.h"
#include "Observers/PictureObserver.h"
#include "Observers/EditorStateObserver.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"

#include <algorithm>
#include <cmath>
#include <memory>

class TimelineView : public PictureObserver, public EditorStateObserver
{
public:
	static constexpr float ROW_H = 26.f;
	static constexpr float ROW_PAD = 4.f;

	TimelineView(shapes::Picture& pic, EditorState& state,
		sf::RenderWindow& window, sf::Font& font,
		float x, float y, float w, float h)
		: m_pic(pic), m_state(state), m_window(window), m_font(font),
		  m_x(x), m_y(y), m_w(w), m_h(h)
	{
		m_pic.Subscribe(this);
		m_state.Subscribe(this);
	}

	~TimelineView() override
	{
		m_pic.Unsubscribe(this);
		m_state.Unsubscribe(this);
	}

	void OnPictureChanged(const std::vector<ShapeInfo>& s) override { m_shapes = s; }
	void OnSelectionChanged(const std::set<std::string>& ids) override { m_selected = ids; }

	void Render()
	{
		sf::RectangleShape bg({ m_w, m_h });
		bg.setPosition({ m_x, m_y });
		bg.setFillColor(sf::Color(245, 245, 245));
		bg.setOutlineColor(sf::Color(180, 180, 180));
		bg.setOutlineThickness(1.f);
		m_window.draw(bg);

		double startT = XToTime(m_x);
		double endT = XToTime(m_x + m_w);
		double step = (m_pixelsPerSecond > 150) ? 0.5 : 1.0;

		for (double t = std::floor(startT / step) * step; t <= endT; t += step)
		{
			float gx = (float)TimeToX(t);
			if (gx < m_x || gx > m_x + m_w)
				continue;

			sf::RectangleShape gridLine({ 1.f, m_h });
			gridLine.setPosition({ gx, m_y });
			gridLine.setFillColor(sf::Color(220, 220, 220));
			m_window.draw(gridLine);

			sf::Text label(m_font);
			label.setString(std::to_string((int)t) + "s");
			label.setCharacterSize(10);
			label.setFillColor(sf::Color(120, 120, 120));
			label.setPosition({ gx + 2.f, m_y + 2.f });
			m_window.draw(label);
		}

		const auto& order = m_pic.GetOrder();
		for (order.rbegin(); order.rbegin() != order.rend())
		{
			const auto& id = order[i];
			if (!m_pic.HasShape(id))
				continue;

			float rowY = RowY((int)i);
			if (rowY + ROW_H < m_y || rowY > m_y + m_h)
				continue;
Gth
			double t = m_pic.GetShape(id).GetTime();
			if (auto pv = m_state.preview.find(id); pv != m_state.preview.end() && pv->second.time)
				t = *pv->second.time;

			bool selected = m_selected.contains(id);

			double duration = m_pic.GetShape(id).GetDuration();
			if (auto pv = m_state.preview.find(id); pv != m_state.preview.end() && pv->second.duration)
				duration = *pv->second.duration;

			float barLeft = (float)TimeToX(t);
			float barRight = barLeft + (float)(duration * m_pixelsPerSecond);

			float clipLeft = std::max(barLeft, m_x);
			float clipRight = std::min(barRight, m_x + m_w);
			if (clipRight <= clipLeft)
				continue;

			sf::RectangleShape bar({ clipRight - clipLeft, ROW_H - 6.f });
			bar.setPosition({ clipLeft, rowY + 2.f });

			sf::Color fillColor = selected ? sf::Color(198, 120, 255) : sf::Color(160, 170, 210);
			sf::Color outlineColor = selected ? sf::Color(100, 50, 150) : sf::Color(100, 100, 120);
			bar.setFillColor(fillColor);
			bar.setOutlineColor(outlineColor);
			bar.setOutlineThickness(1.f);
			m_window.draw(bar);

			if (barLeft >= m_x && barLeft < m_x + m_w)
			{
				sf::Text text(m_font);
				text.setString(id.substr(0, 10));
				text.setCharacterSize(11);
				text.setFillColor(sf::Color::White);
				text.setPosition({ barLeft + 5.f, rowY + 4.f });
				m_window.draw(text);
			}
		}

		float cx = (float)TimeToX(m_state.currentTime);
		if (cx >= m_x && cx <= m_x + m_w)
		{
			sf::RectangleShape line({ 2.f, m_h });
			line.setPosition({ cx, m_y });
			line.setFillColor(sf::Color(255, 80, 80));
			m_window.draw(line);
		}
	}

	bool HitTest(float px, float py) const
	{
		return px >= m_x && px <= m_x + m_w && py >= m_y && py <= m_y + m_h;
	}

	int RowAt(float py) const
	{
		int idx = (int)((py - m_y - ROW_PAD) / ROW_H);
		int count = (int)m_pic.GetOrder().size();
		return (idx >= 0 && idx < count) ? idx : -1;
	}

		bool HandleClick(float px, float py)
	{
		if (!HitTest(px, py))
			return false;

		int row = RowAt(py);
		if (row < 0)
		{
			m_state.currentTime = std::max(0.0, XToTime(px));
			return true;
		}

		const std::string& id = m_pic.GetOrder()[row];
		double t = m_pic.GetShape(id).GetTime();
		double clickTime = XToTime(px);
		double duration = m_pic.GetShape(id).GetDuration();
		double rightEdge = t + duration;

		if (std::abs(px - TimeToX(rightEdge)) < 10.0)
		{
			m_state.SetSelectedIds({ id });
			m_dragMode = DragMode::Duration;
			m_pendingDrag = true;
			m_dragId = id;
			m_dragStartMouseTime = clickTime;
			m_dragStartShapeDuration = duration;
			m_dragCurrentDuration = duration;
			m_dragStartMouseY = py;
			return true;
		}

		if (clickTime >= t && clickTime <= rightEdge)
		{
			m_state.SetSelectedIds({ id });
			m_dragMode = DragMode::None;
			m_pendingDrag = true;
			m_dragId = id;
			m_dragStartMouseTime = clickTime;
			m_dragStartShapeTime = t;
			m_dragCurrentTime = t;
			m_dragStartRow = row;
			m_dragCurrentRow = row;
			m_dragStartMouseY = py;
			return true;
		}

		m_state.currentTime = std::max(0.0, clickTime);
		return true;
	}

	void UpdatePreview(float px, float py)
	{
		if (!m_pendingDrag || !m_pic.HasShape(m_dragId))
			return;

		if (m_dragMode == DragMode::None)
		{
			double dx = std::abs(px - TimeToX(m_dragStartMouseTime));
			double dy = std::abs(py - m_dragStartMouseY);
			constexpr double DEAD_ZONE = 3.0;
			if (dx < DEAD_ZONE && dy < DEAD_ZONE)
				return;
			m_dragMode = (dx >= dy) ? DragMode::Time : DragMode::Layer;
		}

		if (m_dragMode == DragMode::Time)
		{
			double clickTime = XToTime(px);
			double newTime = m_dragStartShapeTime + (clickTime - m_dragStartMouseTime);
			if (newTime < 0)
				newTime = 0;
			m_dragCurrentTime = newTime;
			m_state.preview[m_dragId].time = newTime;
		}
		else if (m_dragMode == DragMode::Layer)
		{
			int targetRow = RowAt(py);
			if (targetRow < 0)
				targetRow = (py < m_y) ? 0 : (int)m_pic.GetOrder().size() - 1;
			if (targetRow != m_dragCurrentRow)
			{
				m_pic.MoveShapeToIndex(m_dragId, static_cast<std::size_t>(targetRow));
				m_dragCurrentRow = targetRow;
			}
		}
		else if (m_dragMode == DragMode::Duration)
		{
			double clickTime = XToTime(px);
			double newDuration = m_dragStartShapeDuration + (clickTime - m_dragStartMouseTime);
			if (newDuration < 0.1) newDuration = 0.1;
			m_dragCurrentDuration = newDuration;
			m_state.preview[m_dragId].duration = newDuration;
		}
	}

	std::unique_ptr<ICommand> Release()
	{
		if (!m_pendingDrag)
			return nullptr;
		m_pendingDrag = false;

		auto mode = m_dragMode;
		m_dragMode = DragMode::None;

		if (!m_pic.HasShape(m_dragId))
			return nullptr;

		if (mode == DragMode::Time)
		{
			if (std::abs(m_dragCurrentTime - m_dragStartShapeTime) < 1e-9)
				return nullptr;

			m_state.preview.erase(m_dragId);

			return std::make_unique<SetTimeCommand>(
				m_pic, std::vector<std::string>{ m_dragId },
				std::vector<double>{ m_dragCurrentTime }, false);
		}

		if (mode == DragMode::Layer)
		{
			if (m_dragCurrentRow == m_dragStartRow)
				return nullptr;
			m_pic.MoveShapeToIndex(m_dragId, (std::size_t)m_dragStartRow);
			return std::make_unique<ReorderCommand>(
				m_pic, m_dragId,
				(std::size_t)m_dragStartRow,
				(std::size_t)m_dragCurrentRow);
		}

		if (mode == DragMode::Duration)
		{
			if (std::abs(m_dragCurrentDuration - m_dragStartShapeDuration) < 1e-9)
				return nullptr;
			m_state.preview.erase(m_dragId);
			return std::make_unique<SetDurationCommand>(
				m_pic, std::vector<std::string>{m_dragId},
				std::vector<double>{m_dragCurrentDuration});
		}
		return nullptr;
	}

	bool IsDragging() const { return m_pendingDrag; }

	void Zoom(double factor, double anchorX)
	{
		double newPps = std::clamp(m_pixelsPerSecond * factor, 10.0, 1000.0);
		if (newPps == m_pixelsPerSecond)
			return;
		double tAtCursor = XToTime(anchorX);
		m_pixelsPerSecond = newPps;
		m_timeOffset = tAtCursor - (anchorX - m_x) / m_pixelsPerSecond;
		if (m_timeOffset < 0)
			m_timeOffset = 0;
	}

private:
	enum class DragMode { None, Time, Layer, Duration };

	double TimeToX(double t) const { return m_x + (t - m_timeOffset) * m_pixelsPerSecond; }
	double XToTime(double px) const { return m_timeOffset + (px - m_x) / m_pixelsPerSecond; }
	float RowY(int index) const { return m_y + ROW_PAD + index * ROW_H; }

	shapes::Picture& m_pic;
	EditorState& m_state;
	sf::RenderWindow& m_window;
	sf::Font& m_font;

	float m_x, m_y, m_w, m_h;
	double m_pixelsPerSecond = 100.0;
	double m_timeOffset = 0.0;

	std::vector<ShapeInfo> m_shapes;
	std::set<std::string> m_selected;

	DragMode m_dragMode = DragMode::None;
	bool m_pendingDrag = false;
	std::string m_dragId;
	double m_dragStartMouseTime = 0.0;
	double m_dragStartShapeTime = 0.0;
	double m_dragCurrentTime = 0.0;
	int m_dragStartRow = -1;
	int m_dragCurrentRow = -1;
	double m_dragStartMouseY = 0.0;
	double m_dragStartShapeDuration = 0.0;
	double m_dragCurrentDuration = 0.0;
};
#endif