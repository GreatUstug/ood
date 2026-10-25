//
// Created by maxim on 25.10.2026.
//

#ifndef L4_COMMANDHISTORY_H
#define L4_COMMANDHISTORY_H
#include "ICommand.h"

#include <memory>
#include <vector>

class CommandHistory
{
public:
	CommandHistory() = default;

	virtual ~CommandHistory() = default;

	void Execute(std::unique_ptr<ICommand> command)
	{
		command->Execute();
		m_undoList.push_back(std::move(command));
		m_redoList.clear();
	}

	bool Undo()
	{
		if (m_undoList.empty())
			return false;
		m_redoList.push_back(std::move(m_undoList.back()));
		m_undoList.pop_back();
		return true;
	}

	bool Redo()
	{
		if (m_redoList.empty()) return false;
		m_redoList.push_back(std::move(m_redoList.back()));
		m_redoList.pop_back();
		return true;
	}

private:
	std::vector<std::unique_ptr<ICommand> > m_undoList;
	std::vector<std::unique_ptr<ICommand> > m_redoList;
};
#endif //L4_COMMANDHISTORY_H