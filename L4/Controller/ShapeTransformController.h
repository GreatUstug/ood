//
// Created by maxim on 12.10.2026.
//

#ifndef L3_SHAPETRANSFORMCONTROLLER_H
#define L3_SHAPETRANSFORMCONTROLLER_H

#include "Commands/CommandHistory.h"
#include "Commands/MoveShapesCommand.h"
#include "Commands/ResizeCommand.h"
#include "Model/Picture.h"
#include "Model/EditorState.h"
#include "Model/Bounds.h"
#include "Model/Handle.h"
#include "Model/ResizePolicy.h"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "View/DrawSelectionFrame.h"

class ShapeTransformController
{
public:
	ShapeTransformController(shapes::Picture& picture,
		EditorState& state,
		CommandHistory& history,
		int canvasWidth,
		int canvasHeight)
		: m_picture(picture)
		  , m_state(state)
		  , m_commandHistory(history)
		  , m_canvasWidth(canvasWidth)
		  , m_canvasHeight(canvasHeight)
	{
	}

	bool TryStartResize(const sf::Vector2f& pos)
	{
		if (!m_state.HasValidSelection(m_picture) || m_state.GetSelectedIds().size() > 1)
		{
			return false;
		}

		auto bounds = m_picture.GetShape(m_state.GetPrimarySelectedId()).GetBounds();
		auto h = GetActualHitTestHandle(bounds, pos.x, pos.y);
		if (h == Handle::None)
		{
			return false;
		}

		m_state.activeHandle = h;
		m_state.resizeStartBounds = bounds;
		m_state.resizeStartMouseX = pos.x;
		m_state.resizeStartMouseY = pos.y;
		m_state.mode = EditorState::Mode::Resizing;
		return true;
	}

	bool IsAdditive()
	{
		return sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);
	}

	void HandleShapeClick(const sf::Vector2f& pos)
	{
		std::string hit = m_picture.HitTest(pos.x, pos.y);

		if (IsAdditive())
		{
			if (hit.empty())
				m_state.ClearSelection();
			else
				m_state.ToggleSelected(hit);
			m_state.mode = EditorState::Mode::Idle;
			return;
		}

		if (hit.empty())
		{
			m_state.ClearSelection();
			m_state.mode = EditorState::Mode::Idle;
			return;
		}

		if (m_state.IsSelected(hit))
			m_state.SetPrimarySelectedId(hit);
		else
			m_state.SetSelectedIds({ hit });

		m_state.dragAnchorX = pos.x;
		m_state.dragAnchorY = pos.y;
		m_state.dragStartBounds.clear();
		for (const auto& id : m_state.GetSelectedIds())
		{
			if (m_picture.HasShape(id))
				m_state.dragStartBounds[id] = m_picture.GetShape(id).GetBounds();
		}
		m_state.lastAppliedDx = 0;
		m_state.lastAppliedDy = 0;
		m_state.mode = EditorState::Mode::Dragging;
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
		if (m_state.mode == EditorState::Mode::Dragging)
		{
			double dx = m_state.lastAppliedDx;
			double dy = m_state.lastAppliedDy;
			if (dx != 0.0 || dy != 0.0)
			{
				std::vector<std::string> ids(
					m_state.GetSelectedIds().begin(), m_state.GetSelectedIds().end());
				m_commandHistory.Execute(std::make_unique<MoveShapesCommand>(
					m_picture, ids, dx, dy, true));
			}
		} else if (m_state.mode == EditorState::Mode::Resizing)
		{
			const std::string& id = m_state.GetPrimarySelectedId();
			if (!id.empty() && m_picture.HasShape(id))
			{
				shapes::Bounds oldB = m_state.resizeStartBounds;
				shapes::Bounds newB = m_picture.GetShape(id).GetBounds();

				constexpr double EPS = 1e-9;
				bool changed =
					std::abs(oldB.x      - newB.x)      > EPS ||
					std::abs(oldB.y      - newB.y)      > EPS ||
					std::abs(oldB.width  - newB.width)  > EPS ||
					std::abs(oldB.height - newB.height) > EPS;

				if (changed)
				{
					m_commandHistory.Execute(std::make_unique<ResizeCommand>(
						m_picture, id, oldB, newB, /*alreadyApplied=*/true));
				}
			}
		}

		m_state.activeHandle = Handle::None;
		m_state.dragStartBounds.clear();
		m_state.lastAppliedDx = 0;
		m_state.lastAppliedDy = 0;
		m_state.mode = EditorState::Mode::Idle;
	}

private:
	void Drag(const sf::Vector2f& position)
	{
		const auto& ids = m_state.GetSelectedIds();
		if (ids.empty())
			return;
		if (m_state.dragStartBounds.empty())
			return;

		double rawDx = position.x - m_state.dragAnchorX;
		double rawDy = position.y - m_state.dragAnchorY;

		double minDx = -1e9, maxDx = 1e9, minDy = -1e9, maxDy = 1e9;
		for (const auto& [id, b] : m_state.dragStartBounds)
		{
			minDx = std::max(minDx, -b.x);
			maxDx = std::min(maxDx, (double)m_canvasWidth - (b.x + b.width));
			minDy = std::max(minDy, -b.y);
			maxDy = std::min(maxDy, (double)m_canvasHeight - (b.y + b.height));
		}

		double dx = std::clamp(rawDx, minDx, maxDx);
		double dy = std::clamp(rawDy, minDy, maxDy);

		double deltaDx = dx - m_state.lastAppliedDx;
		double deltaDy = dy - m_state.lastAppliedDy;
		if (deltaDx == 0.0 && deltaDy == 0.0)
			return;

		std::vector idsVec(ids.begin(), ids.end());
		m_picture.MoveShapes(idsVec, deltaDx, deltaDy);
		m_state.lastAppliedDx = dx;
		m_state.lastAppliedDy = dy;
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
		m_picture.SetShapeBounds(m_state.GetPrimarySelectedId(), b);
	}

	shapes::Picture& m_picture;
	EditorState& m_state;
	CommandHistory& m_commandHistory;
	int m_canvasWidth;
	int m_canvasHeight;
};

#endif //L3_SHAPETRANSFORMCONTROLLER_H