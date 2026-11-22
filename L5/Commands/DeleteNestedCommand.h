//
// Created by maxim on 23.11.2026.
//

#ifndef L5_DELETENESTEDCOMMAND_H
#define L5_DELETENESTEDCOMMAND_H

#include "ICommand.h"
#include "Model/Picture.h"
#include "Model/GroupFigure.h"

#include <memory>
#include <vector>
#include <string>

class DeleteNestedCommand : public ICommand
{
public:
	DeleteNestedCommand(shapes::Picture& picture, std::vector<std::string> ids)
		: m_picture(picture), m_ids(std::move(ids)) {}

	~DeleteNestedCommand() override = default;

	void Execute() override
	{
		ClonePicture();

		for (const auto& id : m_ids)
		{
			if (m_picture.HasShapeDeep(id))
				m_picture.ExtractFigureDeep(id);
		}

		SimplifyGroups();
	}

	void Unexecute() override
	{
		m_picture.ReplaceWith(ClonePictureFromSnapshot());
	}

	[[nodiscard]] std::vector<std::string> AffectedIds() const override
	{
		return m_ids;
	}

private:
	shapes::Picture& m_picture;
	std::vector<std::string> m_ids;

	std::vector<std::unique_ptr<shapes::Figure>> m_snapshot;
	std::vector<std::string> m_snapshotOrder;


	void ClonePicture()
	{
		m_snapshot.clear();
		m_snapshotOrder.clear();
		for (const auto& id : m_picture.GetOrder())
		{
			if (m_picture.HasShape(id))
			{
				m_snapshotOrder.push_back(id);
				m_snapshot.push_back(m_picture.GetShape(id).Clone());
			}
		}
	}

	shapes::Picture ClonePictureFromSnapshot()
	{
		shapes::Picture result;
		for (std::size_t i = 0; i < m_snapshotOrder.size(); ++i)
		{
			result.AddShape(m_snapshot[i]->Clone(), i);
		}
		return result;
	}

	void SimplifyGroups()
	{
		bool changed = true;
		while (changed)
		{
			changed = false;
			std::vector<shapes::GroupFigure*> groups;
			CollectAllGroups(groups);

			for (auto* g : groups)
			{
				if (!m_picture.HasShapeDeep(g->GetId())) continue;
				if (g->GetChildren().size() >= 2) continue;

				auto parent = m_picture.FindParent(g->GetId());
				std::size_t index = 0;
				if (parent)
				{
					for (const auto& c : parent->GetChildren())
					{
						if (c->GetId() == g->GetId()) break;
						++index;
					}
				}
				else
				{
					index = m_picture.IndexOf(g->GetId());
				}

				auto groupFig = m_picture.ExtractFigureDeep(g->GetId());
				if (!groupFig) continue;

				auto children = groupFig->GetChildIds();
				for (const auto& childId : children)
				{
					if (auto c = groupFig->ExtractChild(childId))
					{
						if (parent)
							parent->InsertChild(index++, std::move(c));
						else
							m_picture.AddShape(std::move(c), index++);
					}
				}
				changed = true;
			}
		}
	}

	void CollectAllGroups(std::vector<shapes::GroupFigure*>& out)
	{
		for (const auto& id : m_picture.GetOrder())
		{
			if (m_picture.HasShape(id))
			{
				if (auto* g = m_picture.GetShape(id).AsGroup())
					CollectGroupsRecursive(g, out);
			}
		}
	}

	void CollectGroupsRecursive(shapes::GroupFigure* g, std::vector<shapes::GroupFigure*>& out)
	{
		out.push_back(g);
		for (const auto& child : g->GetChildren())
		{
			if (auto* sub = child->AsGroup())
				CollectGroupsRecursive(sub, out);
		}
	}
};

#endif //L5_DELETENESTEDCOMMAND_H