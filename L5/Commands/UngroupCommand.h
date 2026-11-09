//
// Created by maxim on 09.11.2026.
//

#ifndef L5_UNGROUPCOMMAND_H
#define L5_UNGROUPCOMMAND_H
#include "ICommand.h"
#include "Model/GroupFigure.h"
#include "Model/Picture.h"

class UngroupCommand : public ICommand
{
public:
	UngroupCommand(shapes::Picture& picture, std::string groupId)
		: m_picture(picture), m_groupId(std::move(groupId))
	{
	}

	~UngroupCommand() override = default;

	void Execute() override
	{
		m_groupIndex = m_picture.IndexOf(m_groupId);
		m_childIds = m_picture.GetShape(m_groupId).GetChildIds();
		auto group = m_picture.ExtractFigure(m_groupId);
		for (const auto & m_childId : m_childIds)
		{
			if (auto child = group->ExtractChild(m_childId))
			{
				m_picture.AddShape(std::move(child), m_groupIndex);
			}
		}
	};

	void Unexecute() override
	{
		std::vector<std::unique_ptr<shapes::Figure>> children;
		for (const auto & m_childId : m_childIds)
		{
			children.push_back(m_picture.ExtractFigure(m_childId));
		}
		auto group = std::make_unique<shapes::GroupFigure>(m_groupId, "#c678ff", "#000000");
		for (auto & child : children)
		{
			group->AddChild(std::move(child));
		}
		m_picture.AddShape(std::move(group), m_groupIndex);
	};

	std::vector<std::string> AffectedIds() const override
	{
		return m_childIds;
	};
private:
	shapes::Picture& m_picture;
	std::string m_groupId;
	std::size_t m_groupIndex = 0;
	std::vector<std::string> m_childIds;
};
#endif //L5_UNGROUPCOMMAND_H