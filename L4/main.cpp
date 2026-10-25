#include "Controller/EditorController.h"
#include "Controller/EditorWindowController.h"
#include "View/gfx/SFMLCanvas.h"
#include <SFML/Graphics.hpp>
#include "Model/Picture.h"

int main()
{

	FileDialog dialog;
	sf::Font font;
	if (!font.openFromFile("arial.ttf"))
	{
		throw std::runtime_error("Font not found: arial.ttf");
	}
	std::vector<std::unique_ptr<EditorWindowController> > windows;
	windows.push_back(std::make_unique<EditorWindowController>(font, dialog));

	while (!windows.empty())
	{
		std::vector<std::unique_ptr<EditorWindowController> > toAdd;
		for (auto& window : windows)
		{
			if (!window->IsOpen())
			{
				continue;
			}
			window->HandleEvents();
			if (window->IsOpen())
			{
				window->Render();
			}
			switch (window->TakePendingCommand())
			{
			case AppCommand::NewWindow:
				toAdd.push_back(std::make_unique<EditorWindowController>(font, dialog));
				break;
			case AppCommand::OpenInNewWindow: {
				auto path = dialog.AskOpenPath();
				if (path)
				{
					auto picture = DocumentIO::Load(*path);
					// FIXME: пофиксить 43 строчку и добавить легкое смещение на создание.
					auto newWindow = std::make_unique<EditorWindowController>(font, dialog, sf::Vector2i{0, 0}, *path);
					newWindow->ReplaceDocument(std::move(picture));
					toAdd.push_back(std::move(newWindow));
				}
				break;
			}
			case AppCommand::None:
			default:
				break;
			}

		}
		for (auto& window : toAdd)
		{
			windows.push_back(std::move(window));
		}
		windows.erase(
			std::ranges::remove_if(windows,
				[](const auto& w) { return !w->IsOpen(); }).begin(),
			windows.end());
	}

	return 0;
}

// int main()
// {
// 	std::string str;
// 	std::getline(std::cin, str);
// 	std::unordered_map<size_t, size_t> actors;
// 	size_t id = 0;
// 	actors.reserve(std::stoull(str));
// 	while (std::getline(std::cin, str))
// 	{
// 		actors[++id]
// 	}
// }
