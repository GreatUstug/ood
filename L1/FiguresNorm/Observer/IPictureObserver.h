//
// Created by maxim on 21.09.2026.
//

#ifndef FIGURES_IPICTUREOBSERVER_H
#define FIGURES_IPICTUREOBSERVER_H
#include <Shapes/Picture.h>
class IPictureObserver {
public:
	virtual ~IPictureObserver() = default;
	virtual void OnPictureChanged(const shapes::Picture& picture) = 0;
};

#endif //FIGURES_IPICTUREOBSERVER_H