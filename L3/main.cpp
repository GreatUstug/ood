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
	shapes::Picture picture;
	picture.AddShape(std::make_unique<shapes::IFigure>(
	   "rect1", "#ff0000",
	   std::make_unique<shapes::Rectangle>(100, 100, 200, 150)));

	picture.AddShape(std::make_unique<shapes::IFigure>(
		"circ1", "#00ff00",
		std::make_unique<shapes::Ellipse>(500, 300, 100, 60)));

	picture.AddShape(std::make_unique<shapes::IFigure>(
		"tri1", "#0000ff",
		std::make_unique<shapes::Triangle>(200, 400, 350, 400, 275, 250)));
	EditorState state;
	EditorController controller(picture, state, window, W, H);
	EditorView view(picture, state, canvas);
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