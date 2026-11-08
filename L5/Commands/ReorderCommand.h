//
// Created by maxim on 02.11.2026.
//

#ifndef L4_REORDERCOMMAND_H
#define L4_REORDERCOMMAND_H
#include "ICommand.h"
#include "Model/Picture.h"

class ReorderCommand : public ICommand
{
public:
	ReorderCommand(shapes::Picture& pic, std::string id, std::size_t oldIdx, std::size_t newIdx)
		: m_picture(pic), m_id(std::move(id)), m_oldIdx(oldIdx), m_newIdx(newIdx)
	{
	}

	void Execute() override { m_picture.MoveShapeToIndex(m_id, m_newIdx); }
	void Unexecute() override { m_picture.MoveShapeToIndex(m_id, m_oldIdx); }
	std::vector<std::string> AffectedIds() const override { return { m_id }; }

private:
	shapes::Picture& m_picture;
	std::string m_id;
	std::size_t m_oldIdx, m_newIdx;
};
#endif //L4_REORDERCOMMAND_H