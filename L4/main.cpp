#include "Controller/EditorController.h"
#include "Controller/EditorWindowController.h"
#include "Model/DocumentRegistry.h"
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
	DocumentRegistry registry;
	std::vector<std::unique_ptr<EditorWindowController> > windows;
	windows.push_back(std::make_unique<EditorWindowController>(font, dialog, registry.Create()));

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
			case WindowAction::NewWindow:
				toAdd.push_back(std::make_unique<EditorWindowController>(font, dialog, registry.Create()));
				break;
			case WindowAction::OpenInNewWindow: {
				if (auto path = dialog.AskOpenPath())
				{
					auto picture = DocumentIO::Load(*path);
					// FIXME: пофиксить 43 строчку и добавить легкое смещение на создание.
					auto newWindow = std::make_unique<EditorWindowController>(font, dialog, registry.CreateFrom(std::move(picture), *path), sf::Vector2i{0, 0}, *path);
					toAdd.push_back(std::move(newWindow));
				}
				break;
			}
			case WindowAction::DuplicateWindow: {
				auto newWin = std::make_unique<EditorWindowController>(
					font, dialog, window->GetDocument(),
					sf::Vector2i{0, 0});
				toAdd.push_back(std::move(newWin));
				break;
			}
			case WindowAction::None:
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
		registry.Prune();
	}

	return 0;
}