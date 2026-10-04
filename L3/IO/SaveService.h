//
// Created by maxim on 04.10.2026.
//

#ifndef L3_SAVESERVICE_H
#define L3_SAVESERVICE_H
#include "Shapes/Picture.h"


#include <string>
class SaveService {
public:
	static void Save(const shapes::Picture& picture, std::ostream& out) {
		for (const auto& line : picture.ListAllShapes()) {
			out << line << "\n";
		}
	}
};
#endif //L3_SAVESERVICE_H
