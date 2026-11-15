#ifndef L5_GROUPCOMMAND_H
#define L5_GROUPCOMMAND_H
#include "ICommand.h"
#include "Model/GroupFigure.h"
#include "Model/MakeShapeUtils.h"
#include "Model/Picture.h"

#include <algorithm>
#include <memory>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

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
        m_children.clear();
        m_children.reserve(m_ids.size());
        for (const auto& id : m_ids)
        {
            if (!m_picture.HasShape(id))
                continue;
            m_children.emplace_back(id, m_picture.IndexOf(id));
        }
        std::ranges::sort(m_children, [](const auto& a, const auto& b) {
            return a.second < b.second;
        });

        auto group = std::make_unique<shapes::GroupFigure>(m_groupId, "#c678ff", "#000000");
        for (const auto& [id, idx] : m_children)
            group->AddChild(m_picture.ExtractFigure(id));
        m_picture.AddShape(std::move(group));
    }

    void Unexecute() override
    {
        auto group = m_picture.ExtractFigure(m_groupId);
        for (const auto& [id, idx] : m_children)
        {
            if (auto child = group->ExtractChild(id))
                m_picture.AddShape(std::move(child), idx);
        }
    }

    std::vector<std::string> AffectedIds() const override
    {
        return { m_groupId };
    }

private:
    shapes::Picture& m_picture;
    std::vector<std::string> m_ids;
    std::string m_groupId;
    std::vector<std::pair<std::string, std::size_t>> m_children;
};
#endif //L5_GROUPCOMMAND_H