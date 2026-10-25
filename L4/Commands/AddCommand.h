//
// Created by maxim on 25.10.2026.
//

#ifndef L4_ADDCOMMAND_H
#define L4_ADDCOMMAND_H
#include "ICommand.h"
#include "cmake-build-debug/_deps/harfbuzz-src/src/hb-cplusplus.hh"
#include "Model/Picture.h"
#include <memory>
#include <string>
#include <Model/Figure.h>

class AddCommand : public ICommand
{
public:
	AddCommand(shapes::Picture& picture, const std::unique_ptr<shapes::Figure>& figure):
	m_picture(picture), m_figure(figure), m_id(figure->GetId())
	{

	};

	virtual ~AddCommand() = default;

	void Execute() override
	{
		m_picture.AddShape(std::move(m_figure));
	};

	void Unexecute() override
	{
		m_figure = m_picture.ExtractFigure(m_id);
	};

private:
	shapes::Picture& m_picture;
	std::unique_ptr<shapes::Figure> m_figure;
	std::string m_id;
};
#endif //L4_ADDCOMMAND_H