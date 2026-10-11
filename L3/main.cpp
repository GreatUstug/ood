#include "Controller/EditorController.h"
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
	const unsigned W = 800;
	const unsigned H = 600;
	const unsigned PANEL_W = 200;
	sf::RenderWindow window(sf::VideoMode(sf::Vector2u(W + PANEL_W, H)), "Editor");
	SFMLCanvas canvas(window);

	sf::Font font;
	if (!font.openFromFile("arial.ttf"))
	{
		throw std::runtime_error("Font not found: arial.ttf");
	}

	shapes::Picture picture;
	EditorState state;
	Toolbar toolbar;

	EditorView view(picture, state, canvas, window, font, toolbar);
	ShapeListView listView(picture, state, window, font, W, 0.f, PANEL_W, H);
	EditorController controller(picture, state, view, window, toolbar, listView, W, H);

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
			controller.HandleEvent(*event);
		}
		window.clear();
		view.Render();
		listView.Render();
		window.display();
	}
	return 0;
}