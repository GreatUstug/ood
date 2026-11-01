//
// Created by maxim on 01.11.2026.
//

#ifndef L4_COLORCOMMAND_H
#define L4_COLORCOMMAND_H
#include "ICommand.h"
#include "Model/Picture.h"
#include "View/gfx/ICanvas.h"

class ColorCommand : public ICommand
{
public:
	ColorCommand(shapes::Picture& picture, std::vector<std::string> ids, const gfx::Color& newColor, shapes::ColorTarget target)
		: m_picture(picture), m_ids(std::move(ids)), m_newColor(newColor), m_target(target)
	{
		m_oldColors.reserve(m_ids.size());
		for (const auto& id : m_ids)
		{
			if (target == shapes::ColorTarget::Fill)
				m_oldColors.push_back(m_picture.GetShape(id).GetFillColor());
			else
				m_oldColors.push_back(m_picture.GetShape(id).GetStrokeColor());
		}
	};

	~ColorCommand() override = default;

	void Execute() override
	{
		m_picture.ColorShapes(m_ids, m_newColor, m_target);
	};

	void Unexecute() override
	{
		m_picture.ColorShapes(m_ids, m_oldColors, m_target);
	};

	std::vector<std::string> AffectedIds() const override
	{
		return m_ids;
	};

private:
	shapes::Picture& m_picture;
	std::vector<std::string> m_ids;
	gfx::Color m_newColor;
	std::vector<gfx::Color> m_oldColors;
	shapes::ColorTarget m_target;
};
#endif //L4_COLORCOMMAND_H