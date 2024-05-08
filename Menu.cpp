#include "Menu.hpp"

Menu::Menu(sf::RenderWindow& window, sf::Font& font, std::string title, std::vector<std::string> opts, bool isSubMenu, bool* shouldIncludeAdditionalElements) :
	mWindow(window),
	mFont(font),
	mMenuOptions(opts),
	mCurrSelection(0),
	mIsSubMenu(isSubMenu),
	mPersistInMenu(true),
	mShouldIncludeAdditionalElements(shouldIncludeAdditionalElements) 
{
	mTitle = sf::Text(title, mFont);
	mTitle.setOrigin(mTitle.getLocalBounds().width / 2.f, mTitle.getLocalBounds().height / 2.f);
	mTitle.setPosition(mWindow.getSize().x / 2.f, mWindow.getSize().y / 8.f);

	mKeypressSound.loadFromMemory(&Sounds::click_mp3, Sounds::click_mp3_len);
}
int Menu::runMenu()
{
	//if the elements of which ever menu type is being ran haven't been loaded, load them
	if (mAdditionalStaticElements.size() == 0) this->loadAdditionalStaticElements();
	if (mAdditionalDynamicText.size() == 0) this->loadAdditionalDynamicText();

	sf::Sound keypress(mKeypressSound);
	keypress.setVolume(20);

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

			case sf::Event::Resized:
				//disallows resizing
				//might change if I can get the text to not be super wonky at different sizes
				mWindow.setSize(sf::Vector2u(800, 600));
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

		for (int i = 0; i < mMenuOptions.size(); ++i) {
			auto opt = sf::Text(mMenuOptions[i], mFont);
			opt.setOrigin(opt.getLocalBounds().width / 2, opt.getLocalBounds().height / 2);
			opt.setPosition(mWindow.getSize().x / 2.f, (mWindow.getSize().y / 3.f) + (i * mWindow.getSize().y / (2.f * mMenuOptions.size())));
			if (i == mCurrSelection) {
				opt.setStyle(sf::Text::Bold);
			}
			mWindow.draw(opt);
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
	if (++mCurrSelection >= mMenuOptions.size()) mCurrSelection = 0;
}

void Menu::inputUp(void)
{
	if (--mCurrSelection < 0) mCurrSelection = (int)mMenuOptions.size() - 1;
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

