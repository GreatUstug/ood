//
// Created by maxim on 25.10.2026.
//

#ifndef L4_DELETECOMMAND_H
#define L4_DELETECOMMAND_H
#include "ICommand.h"
#include "Model/Picture.h"

#include <algorithm>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

class DeleteCommand : public ICommand
{
public:
	DeleteCommand(shapes::Picture& picture, std::vector<std::string> ids)
		: m_picture(picture), m_ids(std::move(ids)) {}

	~DeleteCommand() override = default;

	void Execute() override
	{
		m_removed.clear();
		m_removed.reserve(m_ids.size());
		for (const auto& id : m_ids)
		{
			m_removed.push_back({id, m_picture.IndexOf(id), std::move(m_picture.ExtractFigure(id))});;
		}
	};

	void Unexecute() override
	{
		for (auto & [id, index, figure] : std::ranges::reverse_view(m_removed))
		{
			m_picture.InsertShape(index, std::move(figure), id);
		}
		m_removed.clear();
	};

private:
	struct Removed { std::string id; size_t index; std::unique_ptr<shapes::Figure> figure; };
	shapes::Picture& m_picture;
	std::vector<std::string> m_ids;
	std::vector<Removed> m_removed;
};
#endif //L4_DELETECOMMAND_H