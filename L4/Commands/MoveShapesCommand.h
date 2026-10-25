//
// Created by maxim on 26.10.2026.
//

#ifndef L4_MOVESHAPESCOMMAND_H
#define L4_MOVESHAPESCOMMAND_H
#include "ICommand.h"
#include "Model/Picture.h"

class MoveShapesCommand : public ICommand
{
public:
	MoveShapesCommand(shapes::Picture& picture, std::vector<std::string>& ids, double dx, double dy, bool alreadyApplied)
		: m_picture(picture), m_ids(std::move(ids)), m_dx(dx), m_dy(dy), m_alreadyApplied(alreadyApplied)
	{
	};
	void Execute() override
	{
		if (m_alreadyApplied)
		{
			m_alreadyApplied = false;
			return;
		}
		m_picture.MoveShapes(m_ids, m_dx, m_dy);
	};
	void Unexecute() override
	{
		m_picture.MoveShapes(m_ids, -m_dx, -m_dy);
	};

private:
	shapes::Picture& m_picture;
	std::vector<std::string> m_ids;
	double m_dx;
	double m_dy;
	bool m_alreadyApplied;
};
#endif //L4_MOVESHAPESCOMMAND_H