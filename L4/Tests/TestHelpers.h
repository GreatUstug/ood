//
// Created by maxim on 26.10.2026.
//

#ifndef L4_TESTHELPERS_CPP_H
#define L4_TESTHELPERS_CPP_H

#pragma once
#include "Model/Picture.h"
#include "Model/MakeShapeUtils.h"
#include "Commands/CommandHistory.h"
#include "Commands/AddCommand.h"

#include <string>

namespace testhelp
{

inline std::string AddRect(shapes::Picture& pic, CommandHistory& h,
						   const shapes::Bounds& b = {10,10,50,50})
{
	auto id = shapes::MakeId("rect");
	auto fig = shapes::MakeShape("rectangle", id, b);
	h.Execute(std::make_unique<AddCommand>(pic, std::move(fig)));
	return id;
}

inline std::string AddEllipse(shapes::Picture& pic, CommandHistory& h,
							  const shapes::Bounds& b = {100,100,60,40})
{
	auto id = shapes::MakeId("ellipse");
	auto fig = shapes::MakeShape("ellipse", id, b);
	h.Execute(std::make_unique<AddCommand>(pic, std::move(fig)));
	return id;
}

inline std::string AddShape(shapes::Picture& pic, CommandHistory& h,
							const std::string& type,
							const shapes::Bounds& b)
{
	auto id = shapes::MakeId(type);
	auto fig = shapes::MakeShape(type, id, b);
	h.Execute(std::make_unique<AddCommand>(pic, std::move(fig)));
	return id;
}

}

#endif //L4_TESTHELPERS_CPP_H