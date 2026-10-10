#ifndef L3_DOCUMENTIO_H
#define L3_DOCUMENTIO_H

#include "LoadService.h"
#include "SaveService.h"
#include "Model/Picture.h"

#include <fstream>
#include <stdexcept>
#include <string>

class DocumentIO {
public:
	static void Save(const shapes::Picture& picture, const std::string& path) {
		std::ofstream file(path);
		if (!file)
			throw std::runtime_error("Не удалось открыть файл для записи: " + path);
		SaveService::Save(picture, file);
		if (!file)
			throw std::runtime_error("Ошибка записи в файл: " + path);
	}

	static shapes::Picture Load(const std::string& path) {
		std::ifstream file(path);
		if (!file)
			throw std::runtime_error("Не удалось открыть файл: " + path);
		try {
			return LoadService::Load(file);
		} catch (const std::exception& e) {
			throw std::runtime_error("Некорректный файл: " + std::string(e.what()));
		}
	}
};

#endif