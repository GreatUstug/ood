//
// Created by maxim on 09.11.2026.
//

#ifndef L5_GROUPCOMMAND_H
#define L5_GROUPCOMMAND_H
#include "ICommand.h"
#include "Model/GroupFigure.h"
#include "Model/MakeShapeUtils.h"
#include "Model/Picture.h"

class GroupCommand : public ICommand
{
public:
	GroupCommand(shapes::Picture& picture, std::vector<std::string> groupId)
		: m_picture(picture), m_ids(std::move(groupId))
	{
		m_groupId = shapes::MakeId("group");
	}

	~GroupCommand() override = default;

	void Execute() override
	{
		std::vector<std::string> sortedIds = m_ids;
		std::sort(sortedIds.begin(), sortedIds.end(), [this](const std::string& a, const std::string& b) {
			return m_picture.IndexOf(a) < m_picture.IndexOf(b);
		});
		m_oldIndices.clear();
		for (const auto& id : sortedIds)
		{
			m_oldIndices.push_back(m_picture.IndexOf(id));
		}

		std::size_t insertIndex = m_oldIndices[0];

		auto group = std::make_unique<shapes::GroupFigure>(m_groupId, "#c678ff", "#000000");
		for (const auto& id : sortedIds)
		{
			group->AddChild(m_picture.ExtractFigure(id));
		}
		m_picture.AddShape(std::move(group), insertIndex);
	};

	void Unexecute() override
	{
		auto group = m_picture.ExtractFigure(m_groupId);

		for (const auto& id : m_ids) {
			auto child = group->ExtractChild(id);
			if (child) {
				m_picture.AddShape(std::move(child));
			}
		}
	};

	std::vector<std::string> AffectedIds() const override
	{
		return { m_groupId };
	};

private:
	shapes::Picture& m_picture;
	std::vector<std::string> m_ids;
	std::string m_groupId;
	std::vector<std::size_t> m_oldIndices;
};
#endif //L5_GROUPCOMMAND_H