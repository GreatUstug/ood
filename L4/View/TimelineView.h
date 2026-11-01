#ifndef L4_TIMELINEVIEW_H
#define L4_TIMELINEVIEW_H
#include "Commands/ICommand.h"
#include "Model/Picture.h"
#include "Model/EditorState.h"
#include "Model/ShapeInfo.h"
#include "Observers/PictureObserver.h"
#include "Observers/EditorStateObserver.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"

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

		const auto& order = m_pic.GetOrder();
		for (std::size_t i = 0; i < order.size(); ++i)
		{
			const auto& id = order[i];
			if (!m_pic.HasShape(id)) continue;
			double t = m_pic.GetShape(id).GetTime();
			bool selected = m_selected.contains(id);

			sf::RectangleShape bar({ 100.f, ROW_H - 4.f });
			bar.setPosition({ (float)TimeToX(t), RowY((int)i) });
			bar.setFillColor(selected ? sf::Color(198, 120, 255)
									  : sf::Color(180, 180, 220));
			bar.setOutlineColor(sf::Color::Black);
			bar.setOutlineThickness(1.f);
			m_window.draw(bar);

			sf::Text text(m_font);
			text.setString(id.substr(0, 10));
			text.setCharacterSize(11);
			text.setFillColor(sf::Color::Black);
			text.setPosition({ (float)TimeToX(t) + 4.f, (float)RowY((int)i) + 2.f });
			m_window.draw(text);
		}

		float cx = (float)TimeToX(m_state.currentTime);
		sf::RectangleShape line({ 2.f, m_h });
		line.setPosition({ cx, m_y });
		line.setFillColor(sf::Color(255, 80, 80));
		m_window.draw(line);
	}

	bool HitTest(float px, float py) const {
		return px >= m_x && px <= m_x + m_w && py >= m_y && py <= m_y + m_h;
	}

	int RowAt(float py) const {
		int idx = (int)((py - m_y - ROW_PAD) / ROW_H);
		int count = (int)m_pic.GetOrder().size();
		return (idx >= 0 && idx < count) ? idx : -1;
	}

	bool HandleClick(float px, float py) {
		if (!HitTest(px, py)) return false;
		int row = RowAt(py);
		if (row < 0) {
			m_state.currentTime = std::max(0.0, XToTime(px));
			return true;
		}
		const std::string& id = m_pic.GetOrder()[row];
		double t = m_pic.GetShape(id).GetTime();
		double clickTime = XToTime(px);
		if (clickTime >= t && clickTime <= t + 1.0) { // 1.0 - условная длина полосы
			m_state.SetSelectedIds({ id });
			m_draggingTime = true;
			m_dragId = id;
			m_dragStartMouseTime = clickTime;
			m_dragStartShapeTime = t;
			m_dragCurrentTime = t;
			return true;
		}
		m_state.currentTime = std::max(0.0, clickTime);
		return true;
	}

	void UpdatePreview(float px, float py) {
		if (!m_draggingTime || !m_pic.HasShape(m_dragId)) return;
		double clickTime = XToTime(px);
		double newTime = m_dragStartShapeTime + (clickTime - m_dragStartMouseTime);
		if (newTime < 0) newTime = 0;
		m_dragCurrentTime = newTime;
		m_pic.SetShapeTimes({ m_dragId }, { newTime });
	}

	std::unique_ptr<ICommand> Release() {
		if (!m_draggingTime) return nullptr;
		m_draggingTime = false;
		if (!m_pic.HasShape(m_dragId)) return nullptr;
		if (std::abs(m_dragCurrentTime - m_dragStartShapeTime) < 1e-9) return nullptr;

		return std::make_unique<SetTimeCommand>(
			m_pic, std::vector<std::string>{m_dragId}, std::vector<double>{m_dragCurrentTime}, true);
	}

private:
	double TimeToX(double t) const { return m_x + t * m_pixelsPerSecond; }
	double XToTime(double px) const { return (px - m_x) / m_pixelsPerSecond; }
	float RowY(int index) const { return m_y + ROW_PAD + index * ROW_H; }

	int RowAt(float py) const;

	shapes::Picture& m_pic;
	EditorState& m_state;
	sf::RenderWindow& m_window;
	sf::Font& m_font;

	float m_x, m_y, m_w, m_h;
	double m_pixelsPerSecond = 100.0;

	std::vector<ShapeInfo> m_shapes;
	std::set<std::string> m_selected;

	bool m_draggingTime = false;
	std::string m_dragId;
	double m_dragStartMouseTime = 0.0;
	double m_dragStartShapeTime = 0.0;
	double m_dragCurrentTime = 0.0;
};
#endif