#include "ShapesCommandHandler.h"
#include "gfx/SFMLCanvas.h"

#include <SFML/Graphics.hpp>
#include <stdexcept>

int main() {
	sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600)), "Figures");
	window.clear(sf::Color::White);
	window.display();

	SFMLCanvas canvas(window);

	auto pump = [&]() -> bool {
		if (!window.isOpen()) return false;

		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
				return false;
			}
		}

		window.clear(sf::Color::White);
		canvas.Render();
		window.display();
		return true;
	};

	shapes::Picture picture;
	CommandHandler::ShapesCommandHandler handler(picture, canvas, pump);
	handler.Execute();

	return 0;
}