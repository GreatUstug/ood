//
// Created by maxim on 11.10.2026.
//

#ifndef L3_SHAPELISTVIEW_H
#define L3_SHAPELISTVIEW_H

#include "Model/Picture.h"
#include "Model/EditorState.h"
#include "Model/ShapeInfo.h"
#include "Observer/PictureObserver.h"
#include "Observer/EditorStateObserver.h"
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
        m_dirty = true;
    }

    void OnSelectionChanged(const std::string& selectedId) override {
        m_selectedId = selectedId;
        m_dirty = true;
    }

    void Render() {
        sf::RectangleShape bg({m_width, m_height});
        bg.setPosition({m_x, m_y});
        bg.setFillColor(sf::Color(240, 240, 240));
        bg.setOutlineColor(sf::Color(180, 180, 180));
        bg.setOutlineThickness(1.f);
        m_window.draw(bg);

        // TODO: строки
    }

    bool HitTest(float px, float py) const {
        return px >= m_x && px <= m_x + m_width
            && py >= m_y && py <= m_y + m_height;
    }

private:
    shapes::Picture& m_picture;
    EditorState&     m_state;
    sf::RenderWindow& m_window;
    sf::Font&        m_font;

    float m_x, m_y, m_width, m_height;

    std::vector<ShapeInfo> m_cachedShapes;
    std::string m_selectedId;
    bool m_dirty = true;
};

#endif //L3_SHAPELISTVIEW_H
