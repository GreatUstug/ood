//
// Created by maxim on 26.10.2026.
//

#ifndef L4_RESIZECOMMAND_H
#define L4_RESIZECOMMAND_H

#include "ICommand.h"
#include "Model/Picture.h"
#include "Model/Bounds.h"

#include <string>

class ResizeCommand : public ICommand
{
public:
	ResizeCommand(shapes::Picture& picture,
				  std::string id,
				  shapes::Bounds oldBounds,
				  shapes::Bounds newBounds,
				  bool alreadyApplied)
		: m_picture(picture)
		, m_id(std::move(id))
		, m_oldBounds(oldBounds)
		, m_newBounds(newBounds)
		, m_alreadyApplied(alreadyApplied)
	{
	}

	~ResizeCommand() override = default;

	void Execute() override
	{
		if (m_alreadyApplied)
		{
			m_alreadyApplied = false;
			return;
		}
		m_picture.SetShapeBounds(m_id, m_newBounds);
	}

	void Unexecute() override
	{
		m_picture.SetShapeBounds(m_id, m_oldBounds);
	}

private:
	shapes::Picture& m_picture;
	std::string      m_id;
	shapes::Bounds   m_oldBounds;
	shapes::Bounds   m_newBounds;
	bool             m_alreadyApplied;
};

#endif //L4_RESIZECOMMAND_H