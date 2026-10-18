#include "cmake-build-debug/_deps/harfbuzz-src/src/hb-cplusplus.hh"
#include "Controller/EditorController.h"
#include "Controller/EditorWindow.h"
#include "Model/EditorState.h"
#include "View/gfx/SFMLCanvas.h"
#include <optional>
#include <SFML/Graphics.hpp>
#include "View/DrawSelectionFrame.h"
#include "Model/Picture.h"
#include "View/EditorView.h"
#include "View/ShapeListView.h"

int main()
{

	sf::Font font;
	if (!font.openFromFile("arial.ttf"))
	{
		throw std::runtime_error("Font not found: arial.ttf");
	}
	std::vector<std::unique_ptr<EditorWindow>> windows;
	windows.push_back(std::make_unique<EditorWindow>(font));
	while (!windows.empty())
	{
		for (auto &window : windows)
		{
			if (!window->IsOpen()) continue;
			window->HandleEvents();
			if (window->IsOpen()) window->Render();
			windows.erase(
			std::ranges::remove_if(windows,
				[](const auto& w) { return !w->IsOpen(); }).begin(),
			windows.end());
		}
	}

	return 0;
}