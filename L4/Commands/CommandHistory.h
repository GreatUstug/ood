#ifndef L4_COMMANDHISTORY_H
#define L4_COMMANDHISTORY_H
#include "ICommand.h"

#include <memory>
#include <vector>
#include <string>

class CommandHistory
{
public:
	CommandHistory() = default;
	virtual ~CommandHistory() = default;

	void Execute(std::unique_ptr<ICommand> command)
	{
		command->Execute();

		if (m_commandIndex < m_commands.size())
		{
			m_commands.erase(
				m_commands.begin() + static_cast<std::ptrdiff_t>(m_commandIndex),
				m_commands.end());
		}

		m_commands.push_back(std::move(command));
		m_commandIndex++;
	}

	std::vector<std::string> Undo()
	{
		if (m_commandIndex == 0)
			return {};

		m_commandIndex--;
		auto& cmd = m_commands[m_commandIndex];
		auto ids = cmd->AffectedIds();
		cmd->Unexecute();
		return ids;
	}

	std::vector<std::string> Redo()
	{
		if (m_commandIndex >= m_commands.size())
			return {};

		auto& cmd = m_commands[m_commandIndex];
		auto ids = cmd->AffectedIds();
		cmd->Execute();
		m_commandIndex++;
		return ids;
	}

	void Clear()
	{
		m_commandIndex = 0;
		m_commands.clear();
	}

private:
	std::size_t m_commandIndex = 0;
	std::vector<std::unique_ptr<ICommand>> m_commands;
};
#endif //L4_COMMANDHISTORY_H