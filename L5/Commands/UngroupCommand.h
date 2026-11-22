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
		if (m_picture.HasShape(m_groupId))
		{
			const auto& g = m_picture.GetShape(m_groupId);
			m_childIds = g.GetChildIds();
		}
	}

	~UngroupCommand() override = default;

	void Execute() override
	{
		m_groupIndex = m_picture.IndexOf(m_groupId);

		m_groupFill     = m_picture.GetShape(m_groupId).GetFillColor();
		m_groupStroke   = m_picture.GetShape(m_groupId).GetStrokeColor();
		m_groupTime     = m_picture.GetShape(m_groupId).GetTime();
		m_groupDuration = m_picture.GetShape(m_groupId).GetDuration();
		m_childIds      = m_picture.GetShape(m_groupId).GetChildIds();

		auto group = m_picture.ExtractFigure(m_groupId);

		std::size_t insertAt = m_groupIndex;
		for (const auto& m_childId : m_childIds)
		{
			if (auto child = group->ExtractChild(m_childId))
			{
				m_picture.AddShape(std::move(child), insertAt++);
			}
		}
	};

	void Unexecute() override
	{
		std::vector<std::unique_ptr<shapes::Figure> > children;
		for (const auto& m_childId : m_childIds)
		{
			if (m_picture.HasShape(m_childId))
				children.push_back(m_picture.ExtractFigure(m_childId));
		}
		auto group = std::make_unique<shapes::GroupFigure>(
		   m_groupId,
		   m_groupFill.ParseToString(),
		   m_groupStroke.ParseToString());
		group->SetTime(m_groupTime);
		group->SetDuration(m_groupDuration);
		for (auto& child : children)
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

	Color m_groupFill{ 0, 0, 0, 255 };
	Color m_groupStroke{ 0, 0, 0, 255 };
	double m_groupTime = 0.0;
	double m_groupDuration = 1.0;
};
#endif //L5_UNGROUPCOMMAND_H