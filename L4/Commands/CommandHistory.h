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

	std::vector<std::string> Undo()
	{
		if (m_undoList.empty())
			return {};
		auto cmd = std::move(m_undoList.back());
		m_undoList.pop_back();
		auto ids = cmd->AffectedIds();
		cmd->Unexecute();
		m_redoList.push_back(std::move(cmd));
		return ids;
	}

	std::vector<std::string> Redo()
	{
		if (m_redoList.empty())
			return {};
		auto cmd = std::move(m_redoList.back());
		m_redoList.pop_back();
		auto ids = cmd->AffectedIds();
		cmd->Execute();
		m_undoList.push_back(std::move(cmd));
		return ids;
	}

	void Clear()
	{
		m_undoList.clear();
		m_redoList.clear();
	}

private:
	std::vector<std::unique_ptr<ICommand> > m_undoList;
	std::vector<std::unique_ptr<ICommand> > m_redoList;
};
#endif //L4_COMMANDHISTORY_H