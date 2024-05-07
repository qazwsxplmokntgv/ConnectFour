#include "Menu.hpp"

int Menu::runMenu(sf::RenderWindow& window)
{
	sf::Font font;
	font.loadFromFile("PixelifySans.ttf");

	auto title = sf::Text(mTitle, font);
	title.setOrigin(title.getLocalBounds().width / 2.f, title.getLocalBounds().height / 2.f);
	title.setPosition(window.getSize().x / 2.f, window.getSize().y / 8.f);

	while (window.isOpen()) {
		//event loop
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
				
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				switch (event.key.scancode) {
				case sf::Keyboard::Scan::Down:
				case sf::Keyboard::Scan::S:
					//scroll down
					if (++mCurrSelection >= mMenuOptions.size()) mCurrSelection = 0;
					break;
				case sf::Keyboard::Scan::Up:
				case sf::Keyboard::Scan::W:
					//scroll up
					if (--mCurrSelection < 0) mCurrSelection = (int)mMenuOptions.size() - 1;
					break;
				case sf::Keyboard::Scan::Right:
				case sf::Keyboard::Scan::D:
				case sf::Keyboard::Scan::Space:
				case sf::Keyboard::Scan::Enter:
					//advance (select)
					return mCurrSelection;
				case sf::Keyboard::Scan::Left:
				case sf::Keyboard::Scan::A:
				case sf::Keyboard::Scan::Escape:
					//back (exit)
					return -1;
					break;
				}
				break;

			default:
				break;
			}
		}

		//rendering
		window.clear(sf::Color::Black);

		window.draw(title);

		for (int i = 0; i < mMenuOptions.size(); ++i) {
			auto opt = sf::Text(mMenuOptions[i], font);
			opt.setOrigin(opt.getLocalBounds().width / 2, opt.getLocalBounds().height / 2);
			opt.setPosition(window.getSize().x / 2.f, (window.getSize().y / 3.f) + (i * window.getSize().y / (2.f * mMenuOptions.size())));
			if (i == mCurrSelection) opt.setFillColor(sf::Color(150, 150, 150));
			window.draw(opt);
		}

		window.display();
	}
	return -1;
}
