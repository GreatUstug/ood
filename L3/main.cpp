#include "Controller/EditorController.h"
#include "State/EditorState.h"
#include "gfx/SFMLCanvas.h"
#include <optional>
#include <SFML/Graphics.hpp>
#include "State/DrawSelectionFrame.h"
#include "Shapes/Picture.h"
#include "Shapes/Figures/Ellipse.h"
#include "Shapes/Figures/Rectangle.h"
#include "Shapes/Figures/Triangle.h"
#include "View/EditorView.h"

int main() {
	const unsigned W = 800;
	const unsigned H = 600;
	sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600)), "Editor");
	SFMLCanvas canvas(window);

	sf::Font font;
	if (!font.openFromFile("arial.ttf")) {
		throw std::runtime_error("Font not found: arial.ttf");
	}

	shapes::Picture picture;
	EditorState state;
	Toolbar toolbar;

	EditorController controller(picture, state, window, toolbar, W, H);
	EditorView view(picture, state, canvas, window, font, toolbar);
	while (window.isOpen()) {
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
			controller.HandleEvent(*event);
		}
		window.clear();
		view.Render();
		window.display();
	}
	return 0;
}