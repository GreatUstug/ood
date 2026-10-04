//
// Created by maxim on 04.10.2026.
//

#ifndef L3_EDITORVIEW_H
#define L3_EDITORVIEW_H
#include "gfx/ICanvas.h"
#include "Shapes/Picture.h"
#include "State/DrawSelectionFrame.h"
#include "State/EditorState.h"
class EditorView
{
	public:
		EditorView(const shapes::Picture& picture, const EditorState& state, gfx::ICanvas& canvas):
	m_picture(picture), m_state(state), m_canvas(canvas){}
	void Render()
		{
			m_picture.DrawPicture(m_canvas);
			if (m_state.HasSelection() && m_picture.HasShape(m_state.selectedId)) {
				auto bounds = m_picture.GetShape(m_state.selectedId).GetBounds();
				DrawSelectionFrame(m_canvas, bounds);
			}
		};

private:
	const shapes::Picture& m_picture;
	const EditorState& m_state;
	gfx::ICanvas& m_canvas;
};
#endif //L3_EDITORVIEW_H
