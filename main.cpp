/*
* Buffon's Needle Program
* A simple program to approximate pi using the Buffon's
* Needle Problem. More information about the method at:
* https://en.wikipedia.org/wiki/Buffon%27s_needle_problem
* 
* Not the best code I've written, but I just wanted to 
* hack together something quickly.
* 
* Author: stati30241
* Date:   March 14, 2023
* Youtube: https://www.youtube.com/channel/UC7Lx000LLDX6CU0qsyHGfQw
*/


#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include <cmath>
#include <ctime>
#include <numbers>
#include <random>
#include <string>


int main() {
	// Window
	sf::RenderWindow window{ sf::VideoMode{ 1200u, 650u }, "Buffon's Needle Problem", sf::Style::Close };
	ImGui::SFML::Init(window);
	sf::Clock clock;

	// Text
	sf::Font font;
	font.loadFromFile("cour.ttf");
	sf::Text text{ "", font };
	text.setPosition(10.0f, 10.0f);
	text.setStyle(sf::Text::Style::Bold);
	sf::RectangleShape textBackground{ { 600.0f, 50.0f } };
	textBackground.setFillColor(sf::Color::Black);

	// Random numbers
	std::mt19937 randGen{ static_cast<unsigned int>(std::time(nullptr)) };
	std::uniform_real_distribution<float> xDist{ 0.0f, static_cast<float>(window.getSize().x) },
		yDist{ 0.0f, static_cast<float>(window.getSize().y) }, angleDist{ 0.0f, std::numbers::pi_v<float> * 2.0f };

	// Needles and stuff
	float t = 60.0f;
	float l = 60.0f;
	bool dropping = false;
	int dropMultiplier = 1;
	size_t intersectingNeedles = 0;
	sf::VertexArray* needles = new sf::VertexArray{ sf::Lines };

	// Main game loop
	while (window.isOpen()) {
		// Event loop
		sf::Event sfmlEvent;
		ImGui::SFML::ProcessEvent(sfmlEvent);

		while (window.pollEvent(sfmlEvent)) {
			switch (sfmlEvent.type) {
			case sf::Event::Closed:
				window.close();
				break;
			}
		}

		// GUI
		ImGui::SFML::Update(window, clock.restart());
		ImGui::Begin("Settings");
		ImGui::SliderFloat("t", &t, 10.0f, 300.0f);
		ImGui::SliderFloat("l", &l, 10.0f, t);
		ImGui::InputInt("Drop Multiplier", &dropMultiplier);
		if (ImGui::Button(dropping ? "Stop" : "Start")) dropping = !dropping;
		if (ImGui::Button("Reset")) { dropping = false; needles->clear(); intersectingNeedles = 0; }
		ImGui::End();

		for (size_t n = 0; n < dropMultiplier && dropping; ++n) {
			// Calculates a random needle
			sf::Vector2f startPos{ xDist(randGen), yDist(randGen) };
			float angle = angleDist(randGen);
			sf::Vector2f endPos = startPos + sf::Vector2f{ l * cos(angle), l * sin(angle) };

			// Checks if the needle is intersecting a line
			sf::Color needleColor = sf::Color::Yellow;
			for (float i = 0.0f; i <= window.getSize().x; i += t) {
				if ((startPos.x < i && endPos.x > i) || (startPos.x > i && endPos.x < i)) {
					intersectingNeedles++;
					needleColor = sf::Color::Green;
				}
			}

			// Adds the needle
			needles->append({ startPos, needleColor });
			needles->append({ endPos, needleColor });
		}

		// Calculates pi and updates the text
		double p = static_cast<double>(intersectingNeedles) / (needles->getVertexCount() / 2.0);
		double pi = (2.0 * l) / (static_cast<double>(t) * p);
		text.setString("Needles: " + std::to_string(needles->getVertexCount() / 2) + "   Pi: " + std::to_string(pi));

		// Clears the window
		window.clear();

		// Renders the strips of wood
		sf::VertexArray strips{ sf::Lines };
		for (float i = 0.0f; i <= window.getSize().x; i += t) {
			strips.append({ { i, 0.0f } });
			strips.append({ { i, static_cast<float>(window.getSize().y) } });
		}
		window.draw(strips);

		// Renders the needles and the text
		window.draw(*needles);
		window.draw(textBackground);
		window.draw(text);

		// Display
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return 0;
}
