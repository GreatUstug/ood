#include "ShapesCommandHandler.h"
#include "editor/EditorState.h"
#include "gfx/SFMLCanvas.h"
#include <optional>
#include <SFML/Graphics.hpp>
#include "editor/DrawSelectionFrame.h"

int main() {
	sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600)), "Figures");
	SFMLCanvas canvas(window);
	shapes::Picture picture;
	picture.AddShape(std::make_unique<shapes::IFigure>(
	   "rect1", "#ff0000",
	   std::make_unique<shapes::Rectangle>(100, 100, 200, 150)));

	picture.AddShape(std::make_unique<shapes::IFigure>(
		"circ1", "#00ff00",
		std::make_unique<shapes::Circle>(500, 300, 80)));

	picture.AddShape(std::make_unique<shapes::IFigure>(
		"tri1", "#0000ff",
		std::make_unique<shapes::Triangle>(200, 400, 350, 400, 275, 250)));
	EditorState state;
	state.selectedId = "rect1";
	while (window.isOpen()) {
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
		}

		picture.DrawPicture(canvas);
		if (state.HasSelection() && picture.HasShape(state.selectedId)) {
			auto bounds = picture.GetShape(state.selectedId).GetBounds();
			DrawSelectionFrame(canvas, bounds);
		}
		window.display();
	}
	return 0;
}