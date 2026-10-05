#ifndef L3_FILEDIALOG_H
#define L3_FILEDIALOG_H

#include "../IO/portable-file-dialogs.h"
#include <optional>
#include <string>

class FileDialog {
public:
	std::optional<std::string> AskSavePath() {
		auto result = pfd::save_file(
			"Сохранить документ", "",
			{"Text files", "*.txt"}
		).result();
		if (result.empty()) return std::nullopt;
		return result;
	}

	std::optional<std::string> AskOpenPath() {
		auto results = pfd::open_file(
			"Открыть документ", "",
			{"Text files", "*.txt"}
		).result();
		if (results.empty()) return std::nullopt;
		return results[0];
	}

	void ShowError(const std::string& message) {
		pfd::message("Ошибка", message, pfd::choice::ok, pfd::icon::error);
	}
};

#endif