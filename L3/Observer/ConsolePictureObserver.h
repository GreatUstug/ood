//
// Created by maxim on 21.09.2026.
//

#ifndef FIGURES_CONSOLEPICTUREOBSERVER_H
#define FIGURES_CONSOLEPICTUREOBSERVER_H

#include "IPictureObserver.h"
#include <ostream>

class ConsolePictureObserver : public IPictureObserver {
public:
	explicit ConsolePictureObserver(std::ostream& out) : m_out(out) {}

	void OnPictureChanged(size_t shapesCount) override {
		m_out << "Picture changed. Shapes: " << shapesCount << "\n";
	}

private:
	std::ostream& m_out;
};
#endif //FIGURES_CONSOLEPICTUREOBSERVER_H