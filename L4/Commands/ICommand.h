//
// Created by maxim on 25.10.2026.
//

#ifndef L4_ICOMMAND_H
#define L4_ICOMMAND_H
#include <string>
#include <vector>

class ICommand
{
public:
	virtual ~ICommand() = default;

	virtual void Execute() = 0;

	virtual void Unexecute() = 0;
	virtual std::vector<std::string> AffectedIds() const = 0;
};
#endif //L4_ICOMMAND_H