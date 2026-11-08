//
// Created by maxim on 11.10.2026.
//

#ifndef L3_SHAPELISTVIEW_H
#define L3_SHAPELISTVIEW_H

#include "Model/Picture.h"
#include "Model/EditorState.h"
#include "Model/ShapeInfo.h"
#include "Observers/PictureObserver.h"
#include "Observers/EditorStateObserver.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"

class ShapeListView : public PictureObserver, public EditorStateObserver {
public:
    ShapeListView(shapes::Picture& picture,
                  EditorState& state,
                  sf::RenderWindow& window,
                  sf::Font& font,
                  float x, float y, float width, float height)
        : m_picture(picture)
        , m_state(state)
        , m_window(window)
        , m_font(font)
        , m_x(x), m_y(y), m_width(width), m_height(height)
    {
        m_picture.Subscribe(this);
        m_state.Subscribe(this);
    }

    ~ShapeListView() override {
        m_picture.Unsubscribe(this);
        m_state.Unsubscribe(this);
    }

    void OnPictureChanged(const std::vector<ShapeInfo>& shapes) override {
        m_cachedShapes = shapes;
    }

    void OnSelectionChanged(const std::set<std::string>& selectedIds) override {
        m_selectedIds = selectedIds;
    }

    void Render() {
        sf::RectangleShape bg({m_width, m_height});
        bg.setPosition({m_x, m_y});
        bg.setFillColor(sf::Color(240, 240, 240));
        bg.setOutlineColor(sf::Color(180, 180, 180));
        bg.setOutlineThickness(1.f);
        m_window.draw(bg);

        DrawRows();
    }

	void DrawRows()
    {
    	float rowY = m_y + ROW_PADDING;
    	for (auto shape = m_cachedShapes.rbegin(); shape != m_cachedShapes.rend(); ++shape) {
    		DrawRow(*shape, rowY);
    		rowY += ROW_HEIGHT;
    		if (rowY > m_y + m_height) break;
    	}
    }

	void DrawRow(const ShapeInfo& shape, float rowY)
    {
    	sf::RectangleShape rowBg({m_width - 2 * ROW_PADDING, ROW_HEIGHT - 2});
    	rowBg.setPosition({m_x + ROW_PADDING, rowY});
    	rowBg.setFillColor(m_selectedIds.contains(shape.id) ? sf::Color(198, 120, 255)
									: sf::Color(230, 230, 230));
    	rowBg.setOutlineColor(sf::Color(180, 180, 180));
    	rowBg.setOutlineThickness(1.f);
    	m_window.draw(rowBg);
    	sf::Text text(m_font);
    	text.setString(shape.id.substr(0, 12) + " (" + shape.type + ")");
    	text.setCharacterSize((unsigned)TEXT_SIZE);
    	text.setFillColor(sf::Color::Black);
    	text.setPosition({m_x + ROW_PADDING + 6.f, rowY + 6.f});
    	m_window.draw(text);
    }

    bool HitTest(float px, float py) const {
        return px >= m_x && px <= m_x + m_width
            && py >= m_y && py <= m_y + m_height;
    }

	bool HandleClick(float px, float py)
    {
    	int visualIndex = VisualIndexAt(px, py);
    	if (visualIndex < 0) return false;

    	int modelIndex = (int)m_cachedShapes.size() - 1 - visualIndex;
    	const std::string& id = m_cachedShapes[modelIndex].id;

    	const bool additive =
			sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)   ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);

    	if (additive) m_state.ToggleSelected(id);
    	else          m_state.SetSelectedIds({ id });
    	return true;
    }

	int VisualIndexAt(float px, float py) const
    {
    	if (px < m_x || px > m_x + m_width) return -1;
    	if (py < m_y + ROW_PADDING) return -1;

    	int index = (int)((py - m_y - ROW_PADDING) / ROW_HEIGHT);
    	if (index < 0 || index >= (int)m_cachedShapes.size()) return -1;
    	return index;
    }

private:
	static constexpr float ROW_HEIGHT = 28.f;
	static constexpr float ROW_PADDING = 4.f;
	static constexpr float TEXT_SIZE = 14.f;

    shapes::Picture& m_picture;
    EditorState&     m_state;
    sf::RenderWindow& m_window;
    sf::Font&        m_font;

    float m_x, m_y, m_width, m_height;

    std::vector<ShapeInfo> m_cachedShapes;
    std::set<std::string> m_selectedIds;
};

#endif //L3_SHAPELISTVIEW_H
