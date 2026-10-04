#include "editor/EditorState.h"
#include "gfx/SFMLCanvas.h"
#include <optional>
#include <SFML/Graphics.hpp>
#include "editor/DrawSelectionFrame.h"
#include "Shapes/Picture.h"
#include "Shapes/Figures/Ellipse.h"
#include "Shapes/Figures/Rectangle.h"
#include "Shapes/Figures/Triangle.h"

int main() {
	sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600)), "Figures");
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
	while (window.isOpen()) {
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
			{
				if (mouse->button == sf::Mouse::Button::Left)
				{
					sf::Vector2f pos = window.mapPixelToCoords(
				{mouse->position.x, mouse->position.y});

					std::string hit = picture.HitTest(pos.x, pos.y);
					if (hit == state.selectedId && state.HasSelection())
					{
						auto bounds = picture.GetShape(hit).GetBounds();
						state.isDragging = true;
						state.dragOffsetX = pos.x - bounds.x;
						state.dragOffsetY = pos.y - bounds.y;
					} else
					{
						state.selectedId = hit;
						state.isDragging = false;
					}
				}
			}
			if (const auto* moved = event->getIf<sf::Event::MouseMoved>())
			{
				if (state.isDragging && state.HasSelection())
				{
					sf::Vector2f pos = window.mapPixelToCoords({moved->position.x, moved->position.y
				});
					auto bounds = picture.GetShape(state.selectedId).GetBounds();
					double targetX = pos.x + state.dragOffsetX;
					double targetY = pos.y + state.dragOffsetY;
					const double W = 800, H = 600;
					if (targetX < 0) targetX = 0;
					if (targetY < 0) targetY = 0;
					if (targetX + bounds.width > W)  targetX = W - bounds.width;
					if (targetY + bounds.height > H) targetY = H - bounds.height;

					double dx = targetX - bounds.x;
					double dy = targetY - bounds.y;
					if (dx != 0.0 && dy != 0.0)
					{
						picture.MoveShape(state.selectedId, dx, dy);
					}
				}
			}
			if (const auto* rel = event->getIf<sf::Event::MouseButtonReleased>())
			{
				if (rel->button == sf::Mouse::Button::Left)
				{
					state.isDragging = false;
				}
			}
		}
		window.clear();
		picture.DrawPicture(canvas);
		if (state.HasSelection() && picture.HasShape(state.selectedId)) {
			auto bounds = picture.GetShape(state.selectedId).GetBounds();
			DrawSelectionFrame(canvas, bounds);
		}
		window.display();
	}
	return 0;
}