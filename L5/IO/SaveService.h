#ifndef L5_SAVESERVICE_H
#define L5_SAVESERVICE_H
#include "Model/Picture.h"
#include <string>
#include <ostream>

class SaveService
{
public:
	static void Save(const shapes::Picture& picture, std::ostream& out)
	{
		for (const auto& id : picture.GetOrder())
		{
			if (picture.HasShape(id)) {
				SaveFigure(picture.GetShape(id), out, 0);
			}
		}
	}

private:
	static void SaveFigure(const shapes::Figure& fig, std::ostream& out, int indentLevel)
	{
		std::string indent(indentLevel * 4, ' ');
		out << indent << fig.GetInfo();

		if (fig.GetType() == "group") {
			out << " {\n";
			for (const auto& child : fig.GetChildren()) {
				SaveFigure(*child, out, indentLevel + 1);
			}
			out << indent << "}\n";
		} else {
			out << "\n";
		}
	}
};
#endif