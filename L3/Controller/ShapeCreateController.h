//
// Created by maxim on 12.10.2026.
//

#ifndef L3_SHAPECREATECONTROLLER_H
#define L3_SHAPECREATECONTROLLER_H

#include "Model/Picture.h"
#include "Model/EditorState.h"
#include "Model/Bounds.h"
#include "Model/ResizePolicy.h"
#include "Model/MakeShapeUtils.h"

#include "SFML/System/Vector2.hpp"

#include <string>

class ShapeCreateController
{
public:
    ShapeCreateController(shapes::Picture& picture,
                          EditorState& state,
                          int canvasWidth,
                          int canvasHeight)
        : m_picture(picture)
        , m_state(state)
        , m_canvasWidth(canvasWidth)
        , m_canvasHeight(canvasHeight)
    {
    }

    void Start(const std::string& type)
    {
        m_state.mode = EditorState::Mode::Creating;
        m_state.creatingType = type;
        m_state.hasCreateStart = false;
        m_state.ClearSelection();
    }

    void Begin(const sf::Vector2f& pos)
    {
        m_state.createStartX   = pos.x;
        m_state.createStartY   = pos.y;
        m_state.createCurrentX = pos.x;
        m_state.createCurrentY = pos.y;
        m_state.hasCreateStart = true;
    }

    void UpdatePreview(const sf::Vector2f& pos)
    {
        if (!m_state.hasCreateStart)
            return;
        m_state.createCurrentX = pos.x;
        m_state.createCurrentY = pos.y;
    }

    void Commit()
    {
        if (!m_state.hasCreateStart)
            return;

        shapes::Bounds b = shapes::BoundsFromPoints(
            m_state.createStartX, m_state.createStartY,
            m_state.createCurrentX, m_state.createCurrentY);

        if (b.width < shapes::MIN_SHAPE_SIZE)
            b.width = shapes::MIN_SHAPE_SIZE;
        if (b.height < shapes::MIN_SHAPE_SIZE)
            b.height = shapes::MIN_SHAPE_SIZE;

        b = shapes::ClampToCanvas(b, m_canvasWidth, m_canvasHeight);

        if (b.width < 1.0 || b.height < 1.0)
        {
            m_state.CancelCreating();
            return;
        }

        std::string id = shapes::MakeId(m_state.creatingType);
        auto figure = shapes::MakeShape(m_state.creatingType, id, b);
        if (!figure)
        {
            m_state.CancelCreating();
            return;
        }

        m_picture.AddShape(std::move(figure));
        m_state.SetSelectedId(id);
        m_state.CancelCreating();
    }

    void Cancel()
    {
        m_state.CancelCreating();
    }

private:
    shapes::Picture& m_picture;
    EditorState&     m_state;
    int m_canvasWidth;
    int m_canvasHeight;
};

#endif //L3_SHAPECREATECONTROLLER_H