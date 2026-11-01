#ifndef L4_SETTIMECOMMAND_H
#define L4_SETTIMECOMMAND_H
#include "ICommand.h"
#include "Model/Picture.h"

class SetTimeCommand : public ICommand
{
public:
	SetTimeCommand(shapes::Picture& pic,
		std::vector<std::string> ids,
		std::vector<double> newTimes)
		: m_picture(pic), m_ids(std::move(ids)), m_newTimes(std::move(newTimes))
	{
		for (const auto& id : m_ids)
			m_oldTimes.push_back(m_picture.GetShape(id).GetTime());
	}

	void Execute() override { m_picture.SetShapeTimes(m_ids, m_newTimes); }
	void Unexecute() override { m_picture.SetShapeTimes(m_ids, m_oldTimes); }
	std::vector<std::string> AffectedIds() const override { return m_ids; }

private:
	shapes::Picture& m_picture;
	std::vector<std::string> m_ids;
	std::vector<double> m_newTimes;
	std::vector<double> m_oldTimes;
};
#endif