#ifndef L4_SETDURATIONCOMMAND_H
#define L4_SETDURATIONCOMMAND_H
#include "ICommand.h"
#include "Model/Picture.h"
#include <string>
#include <vector>

class SetDurationCommand : public ICommand
{
public:
	SetDurationCommand(shapes::Picture& pic, std::vector<std::string> ids, std::vector<double> newDurations)
		: m_picture(pic), m_ids(std::move(ids)), m_newDurations(std::move(newDurations))
	{
		for (const auto& id : m_ids)
			m_oldDurations.push_back(m_picture.GetShape(id).GetDuration());
	}

	~SetDurationCommand() override = default;

	void Execute() override { m_picture.SetShapeDurations(m_ids, m_newDurations); }
	void Unexecute() override { m_picture.SetShapeDurations(m_ids, m_oldDurations); }
	std::vector<std::string> AffectedIds() const override { return m_ids; }

private:
	shapes::Picture& m_picture;
	std::vector<std::string> m_ids;
	std::vector<double> m_newDurations;
	std::vector<double> m_oldDurations;
};
#endif