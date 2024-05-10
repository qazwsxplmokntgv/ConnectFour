#include "Menu.hpp"

Menu::Menu(sf::RenderWindow& window, sf::Font& font, std::string title, std::vector<std::string> opts, bool isSubMenu, bool* shouldIncludeAdditionalElements) :
	mWindow(window),
	mFont(font),
	mCurrSelection(0),
	mIsSubMenu(isSubMenu),
	mPersistInMenu(true),
	mShouldIncludeAdditionalElements(shouldIncludeAdditionalElements) 
{
	//title
	mTitle = sf::Text(title, mFont);
	mTitle.setOrigin(mTitle.getLocalBounds().width / 2.f, mTitle.getLocalBounds().height / 2.f);
	mTitle.setPosition(mWindow.getSize().x / 2.f, mWindow.getSize().y / 8.f);

	//opt texts
	for (int i = 0; i < opts.size(); ++i) {
		auto opt = sf::Text(opts[i], mFont);
		opt.setOrigin(opt.getLocalBounds().width / 2, opt.getLocalBounds().height / 2);
		opt.setPosition(
			mWindow.getSize().x / 2.f, 
			//starting from 1/3 down the screen, 
			(mWindow.getSize().y / 3.f) + 
			//add i / (2 * n) window heights down, where n = total num of opts
			(i * mWindow.getSize().y / (2.f * opts.size()))
		);

		mOptTexts.push_back(opt);
	}
	
	//sound
	mKeypressSound.loadFromMemory(&Sounds::click_mp3, Sounds::click_mp3_len);
}
int Menu::runMenu()
{
	//if the elements of which ever menu type is being ran haven't been loaded, load them
	if (mAdditionalStaticElements.size() == 0) this->loadAdditionalStaticElements();
	if (mAdditionalDynamicText.size() == 0) this->loadAdditionalDynamicText();

	sf::Sound keypress(mKeypressSound);
	keypress.setVolume(20);

	//bounding boxes corresponding to each opt
	std::vector<sf::FloatRect> optBoundingBoxes;
	for (int i = 0; i < mOptTexts.size(); ++i) {
		optBoundingBoxes.push_back(sf::FloatRect(
			mWindow.getSize().x / 3.f,
			(mWindow.getSize().y / 3.f) + ((i - .5f) * mWindow.getSize().y / (2.f * mOptTexts.size())),
			mWindow.getSize().x / 3.f,
			(mWindow.getSize().y) / (2.f * mOptTexts.size())
		));
	}

	//run menu
	while (mPersistInMenu && mWindow.isOpen()) {
		//event loop
		sf::Event event;
		while (mWindow.pollEvent(event)) {
			switch (event.type) {
				
			case sf::Event::Closed:
				mCurrSelection = -1;
				mWindow.close();
				return mCurrSelection;

			case sf::Event::MouseMoved:
				if (mWindow.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)).x >= mWindow.getSize().x / 3.f
					&& mWindow.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)).x < mWindow.getSize().x * 2.f / 3.f) {
 					for (int i = 0; i < optBoundingBoxes.size(); ++i) {
						if (optBoundingBoxes[i].contains(mWindow.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)))) {
							mCurrSelection = i;
							break;
						}
					}
				}
				break;

			case sf::Event::MouseButtonPressed:
				switch (event.mouseButton.button) {
				
				case sf::Mouse::Left:
				case sf::Mouse::XButton2:
					if (mWindow.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)).x >= mWindow.getSize().x / 3.f
						&& mWindow.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)).x < mWindow.getSize().x * 2.f / 3.f) {
						for (int i = 0; i < optBoundingBoxes.size(); ++i) {
							if (optBoundingBoxes[i].contains(mWindow.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)))) {
								mCurrSelection = i;
								inputRight();
								break;
							}
						}
					}
					break;
				
				case sf::Mouse::Right:
				case sf::Mouse::XButton1:
					inputLeft();
					break;
				
				}
				break;

			case sf::Event::MouseWheelScrolled:
				for (int i = 0; i < event.mouseWheelScroll.delta; ++i) inputUp();
				for (int i = 0; i > event.mouseWheelScroll.delta; --i) inputDown();
				break;

			case sf::Event::KeyPressed:
				switch (event.key.scancode) {
				case sf::Keyboard::Scan::Down:
				case sf::Keyboard::Scan::S:
					//scroll down
					keypress.play();
					inputDown();
					break;
				case sf::Keyboard::Scan::Up:
				case sf::Keyboard::Scan::W:
					//scroll up
					keypress.play();
					inputUp();
					break;
				case sf::Keyboard::Scan::Right:
				case sf::Keyboard::Scan::D:
				case sf::Keyboard::Scan::Space:
				case sf::Keyboard::Scan::Enter:
					//advance (select)
					keypress.play(); // currently does not get an opportunity to play because of immediate return //TODO
					inputRight();
					break;
				case sf::Keyboard::Scan::Left:
				case sf::Keyboard::Scan::A:
				case sf::Keyboard::Scan::Escape:
					//back (exit)
					keypress.play();
					inputLeft();
					break;
				}
				break;
			}
		}

		//rendering
		mWindow.clear(sf::Color::Black);

		mWindow.draw(mTitle);

		for (int i = 0; i < mOptTexts.size(); ++i) {
			//bold the selection
			if (i == mCurrSelection)
				mOptTexts[i].setStyle(sf::Text::Style::Bold);
			else if (mOptTexts[i].getStyle() != sf::Text::Style::Regular)
				mOptTexts[i].setStyle(sf::Text::Style::Regular);

			//draw opts
			mWindow.draw(mOptTexts[i]);
		} 

		//draws any other elements (for specialized menus)
		if (mShouldIncludeAdditionalElements != nullptr && *mShouldIncludeAdditionalElements) {
			for (auto& text : mAdditionalDynamicText)
				mWindow.draw(text);
			for (auto& elem : mAdditionalStaticElements)
				mWindow.draw(*elem);
		}

		mWindow.display();
	}
	//resets this flag such that the menu continues if revisited
	mPersistInMenu = true;
	return mCurrSelection;
}

void Menu::resetSelection(void)
{
	mCurrSelection = 0;
}

void Menu::inputDown(void)
{
	if (++mCurrSelection >= mOptTexts.size()) mCurrSelection = 0;
}

void Menu::inputUp(void)
{
	if (--mCurrSelection < 0) mCurrSelection = (int)mOptTexts.size() - 1;
		}

void Menu::inputLeft(void)
{
	//prevents back button from outermost menu
	if (!mIsSubMenu) return;
	mPersistInMenu = false;
	mCurrSelection = -1;
	}

void Menu::inputRight(void)
{
	mPersistInMenu = false;
}

