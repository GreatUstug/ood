//
// Created by maxim on 21.09.2026.
//

#ifndef FIGURES_IPICTUREOBSERVER_H
#define FIGURES_IPICTUREOBSERVER_H
#include <string>
#include <vector>

class PictureObserver {
public:
	virtual ~PictureObserver() = default;
	virtual void OnPictureChanged(const std::vector<ShapeInfo>& shapesInfo) = 0;
};

#endif //FIGURES_IPICTUREOBSERVER_H