//
// Created by maxim on 12.10.2026.
//

#ifndef L3_SHAPETRANSFORMCONTROLLER_H
#define L3_SHAPETRANSFORMCONTROLLER_H

#include "Model/Picture.h"
#include "Model/EditorState.h"
#include "Model/Bounds.h"
#include "Model/Handle.h"
#include "Model/ResizePolicy.h"
#include "SFML/System/Vector2.hpp"
#include "View/DrawSelectionFrame.h"

class ShapeTransformController
{
public:
	ShapeTransformController(shapes::Picture& picture,
		EditorState& state,
		int canvasWidth,
		int canvasHeight)
		: m_picture(picture)
		  , m_state(state)
		  , m_canvasWidth(canvasWidth)
		  , m_canvasHeight(canvasHeight)
	{
	}

	bool TryStartResize(const sf::Vector2f& pos)
	{
		if (!m_state.HasValidSelection(m_picture))
			return false;

		auto bounds = m_picture.GetShape(m_state.GetSelectedId()).GetBounds();
		auto h = GetActualHitTestHandle(bounds, pos.x, pos.y);
		if (h == Handle::None)
			return false;

		m_state.activeHandle = h;
		m_state.resizeStartBounds = bounds;
		m_state.resizeStartMouseX = pos.x;
		m_state.resizeStartMouseY = pos.y;
		m_state.mode = EditorState::Mode::Resizing;
		return true;
	}

	void HandleShapeClick(const sf::Vector2f& pos)
	{
		std::string hit = m_picture.HitTest(pos.x, pos.y);
		if ((hit == m_state.GetSelectedId()) && m_state.HasValidSelection(m_picture))
		{
			auto bounds = m_picture.GetShape(hit).GetBounds();
			m_state.dragOffsetX = pos.x - bounds.x;
			m_state.dragOffsetY = pos.y - bounds.y;
			m_state.mode = EditorState::Mode::Dragging;
		}
		else
		{
			m_state.SetSelectedId(hit);
			m_state.mode = EditorState::Mode::Idle;
		}
	}

	void Update(const sf::Vector2f& pos)
	{
		if (!m_state.HasValidSelection(m_picture))
			return;

		switch (m_state.mode)
		{
		case EditorState::Mode::Dragging:
			Drag(pos);
			break;
		case EditorState::Mode::Resizing:
			Resize(pos);
			break;
		default:
			break;
		}
	}

	void Release()
	{
		m_state.activeHandle = Handle::None;
		m_state.mode = EditorState::Mode::Idle;
	}

private:
	void Drag(const sf::Vector2f& position)
	{
		auto bounds = m_picture.GetShape(m_state.GetSelectedId()).GetBounds();
		shapes::Bounds desired{
			position.x - m_state.dragOffsetX,
			position.y - m_state.dragOffsetY,
			bounds.width,
			bounds.height
		};

		auto target = shapes::ClampToCanvasByShift(desired, m_canvasWidth, m_canvasHeight);

		double dx = target.x - bounds.x;
		double dy = target.y - bounds.y;
		if (dx != 0.0 || dy != 0.0)
		{
			m_picture.MoveShape(m_state.GetSelectedId(), dx, dy);
		}
	}

	void Resize(const sf::Vector2f& position)
	{
		double dx = position.x - m_state.resizeStartMouseX;
		double dy = position.y - m_state.resizeStartMouseY;
		auto b = shapes::ResizeAndClamp(
			m_state.resizeStartBounds,
			m_state.activeHandle,
			dx, dy,
			m_canvasWidth, m_canvasHeight);
		m_picture.SetShapeBounds(m_state.GetSelectedId(), b);
	}

	shapes::Picture& m_picture;
	EditorState& m_state;
	int m_canvasWidth;
	int m_canvasHeight;
};

#endif //L3_SHAPETRANSFORMCONTROLLER_H