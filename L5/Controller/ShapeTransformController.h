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
#include "Model/Document.h"
#include "Model/Handle.h"
#include "Model/ResizePolicy.h"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "View/DrawSelectionFrame.h"

#include <ranges>

class ShapeTransformController
{
public:
	ShapeTransformController(Document& document,
		EditorState& state,
		int canvasWidth,
		int canvasHeight)
		: m_document(document)
		  , m_state(state)
		  , m_canvasWidth(canvasWidth)
		  , m_canvasHeight(canvasHeight)
	{
	}

	bool TryStartResize(const sf::Vector2f& pos)
	{
		if (!m_state.HasValidSelection(m_document.GetPicture()) || m_state.GetSelectedIds().size() > 1)
		{
			return false;
		}

		auto bounds = m_document.GetPicture().GetShape(m_state.GetPrimarySelectedId()).GetBounds();
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
		std::string hit = m_document.GetPicture().HitTest(pos.x, pos.y);

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
			if (m_document.GetPicture().HasShape(id))
				m_state.dragStartBounds[id] = m_document.GetPicture().GetShape(id).GetBounds();
		}
		m_state.lastAppliedDx = 0;
		m_state.lastAppliedDy = 0;
		m_state.mode = EditorState::Mode::Dragging;
	}

	void Update(const sf::Vector2f& pos)
	{
		if (!m_state.HasValidSelection(m_document.GetPicture()))
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

				m_document.GetHistory().Execute(std::make_unique<MoveShapesCommand>(
					m_document.GetPicture(), ids, dx, dy));
			}
		}
		else if (m_state.mode == EditorState::Mode::Resizing)
		{
			const std::string& id = m_state.GetPrimarySelectedId();
			if (!id.empty() && m_document.GetPicture().HasShape(id))
			{
				shapes::Bounds oldB = m_state.resizeStartBounds;
				auto pvIt = m_state.preview.find(id);

				if (pvIt != m_state.preview.end() && pvIt->second.bounds)
				{
					shapes::Bounds newB = *pvIt->second.bounds;

					constexpr double EPS = 1e-9;
					if (std::abs(oldB.x - newB.x) > EPS || std::abs(oldB.y - newB.y) > EPS ||
						std::abs(oldB.width - newB.width) > EPS || std::abs(oldB.height - newB.height) > EPS)
					{
						m_document.GetHistory().Execute(std::make_unique<ResizeCommand>(
							m_document.GetPicture(), id, oldB, newB));
					}
				}
			}
		}

		m_state.ClearPreview();
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
		for (const auto& [x, y, width, height] : m_state.dragStartBounds | std::views::values)
		{
			minDx = std::max(minDx, -x);
			maxDx = std::min(maxDx, (double)m_canvasWidth - (x + width));
			minDy = std::max(minDy, -y);
			maxDy = std::min(maxDy, (double)m_canvasHeight - (y + height));
		}

		double dx = std::clamp(rawDx, minDx, maxDx);
		double dy = std::clamp(rawDy, minDy, maxDy);

		for (const auto& id : ids)
		{
			shapes::Bounds newB = m_state.dragStartBounds.at(id);
			newB.x += dx;
			newB.y += dy;
			m_state.preview[id].bounds = newB;
		}
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
		m_state.preview[m_state.GetPrimarySelectedId()].bounds = b;
	}

	Document& m_document;
	EditorState& m_state;
	int m_canvasWidth;
	int m_canvasHeight;
};

#endif //L3_SHAPETRANSFORMCONTROLLER_H