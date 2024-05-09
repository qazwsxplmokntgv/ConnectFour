#include "ConnectFour.hpp"

ConnectFour::ConnectFour(Settings& settings) :
	mSettings(settings),
	mMoveCount(0),
	mCurrPlayer(0),
	mSelectedCol(0),
	mRoundWinner(-1),
	mInSidebar(false),
	mSideBarSelection(-1),
	mLastSideBarSelection(0)
{
	//font
	mFont.loadFromMemory(&Fonts::PixelifySans_ttf, Fonts::PixelifySans_ttf_len);

	//sound
	drop.loadFromMemory(&Sounds::pop_wav, Sounds::pop_wav_len);
	win.loadFromMemory(&Sounds::win_wav, Sounds::win_wav_len);
	sideBar.loadFromMemory(&Sounds::click_mp3, Sounds::click_mp3_len);
	mDropSound = sf::Sound(drop);
	mWinSound = sf::Sound(win);
	mSideBarSound = sf::Sound(sideBar);
	mSideBarSound.setVolume(20);

	//selection bar
	this->mSelectionBar = sf::RectangleShape(sf::Vector2f(UnitSizes::tileSize / 2.f, UnitSizes::tileSize * mSettings.mBoardHeight));
	this->mSelectionBar.setFillColor(mSettings.mPlayerInfo[0].getToken().getColor());
	this->mSelectionBar.setPosition(UnitSizes::tileSize * (UnitSizes::sideBarWidth + .25f), 0);
	this->mSelectionBar.setOutlineThickness(UnitSizes::outlineThickness);
	this->mSelectionBar.setOutlineColor(sf::Color(0, 0, 0, 100));

	//scoreboard
	this->scoreBoardTexture.create((unsigned int)UnitSizes::tileSize * mSettings.mPlayerCount, (unsigned int)UnitSizes::tileSize * mSettings.mBoardHeight);
	this->scoreBoardTexture.clear();
	{
		//create scoreboard background
		sf::RectangleShape scoreBoardBackGround(sf::Vector2f(UnitSizes::tileSize * mSettings.mPlayerCount, UnitSizes::tileSize * mSettings.mBoardHeight));
		scoreBoardBackGround.setFillColor(sf::Color(100, 100, 100));
		scoreBoardBackGround.setOutlineThickness(UnitSizes::outlineThickness);
		scoreBoardBackGround.setOutlineColor(sf::Color(50, 50, 50));

		this->scoreBoardTexture.draw(scoreBoardBackGround);

		//create main header
		sf::Text header("Scoreboard", mFont);
		header.setOrigin(header.getLocalBounds().width / 2.f, header.getLocalBounds().height / 2.f);
		header.setPosition(UnitSizes::tileSize * mSettings.mPlayerCount / 2.f, UnitSizes::tileSize / 2.f);

		this->scoreBoardTexture.draw(header);

		//create individual labels for each player
		for (int i = 0; i < mSettings.mPlayerInfo.size(); ++i) {
			auto label = sf::Text("P" + std::to_string(i + 1), mFont);
			label.setFillColor(mSettings.mPlayerInfo[i].getToken().getColor());
			label.setOrigin(label.getLocalBounds().width / 2.f, label.getLocalBounds().height / 2.f);
			label.setPosition((.5f + i) * UnitSizes::tileSize, UnitSizes::tileSize);

			this->scoreBoardTexture.draw(label);
		}
	}
	this->scoreBoardTexture.display();
	this->mScoreBoard = sf::Sprite(scoreBoardTexture.getTexture());
	this->mScoreBoard.setPosition(UnitSizes::tileSize * (UnitSizes::sideBarWidth + mSettings.mBoardWidth), 0);

	//controls diagram & text overlays
	this->controlDiagramTexture.create((unsigned int)UnitSizes::tileSize * UnitSizes::sideBarWidth, (unsigned int)UnitSizes::tileSize * (mSettings.mBoardHeight < 4 ? mSettings.mBoardHeight : UnitSizes::sideBarWidth));
	this->controlDiagramTexture.clear();
	{
		//diagram background
		sf::RectangleShape controlDiagramBackground(sf::Vector2f(UnitSizes::tileSize * UnitSizes::sideBarWidth, UnitSizes::tileSize * (float)(mSettings.mBoardHeight < 4 ? mSettings.mBoardHeight : UnitSizes::sideBarWidth)));
		controlDiagramBackground.setFillColor(sf::Color(100, 100, 100));
		controlDiagramBackground.setOutlineThickness(UnitSizes::outlineThickness);
		controlDiagramBackground.setOutlineColor(sf::Color(50, 50, 50));
		this->controlDiagramTexture.draw(controlDiagramBackground);

		//arrows corresponding to directions (ie user inputs via wasd/arrows)
		sf::ConvexShape arrow(3);
		arrow.setPoint(0, sf::Vector2f(0, UnitSizes::tileSize * .3f));
		arrow.setPoint(1, sf::Vector2f(UnitSizes::tileSize * .15f, 0));
		arrow.setPoint(2, sf::Vector2f(UnitSizes::tileSize * .3f, UnitSizes::tileSize * .3f));
		arrow.setOrigin(sf::Vector2f(UnitSizes::tileSize * .15f, UnitSizes::tileSize * .5f));
		arrow.setPosition(UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f, UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f);
		this->controlDiagramTexture.draw(arrow);
		for (int i = 0; i < 3; ++i) {
			arrow.rotate(90);
			this->controlDiagramTexture.draw(arrow);
		}

		this->controlDiagramTexture.display();
	}
	this->gameControlTexture.create((unsigned int)UnitSizes::tileSize * UnitSizes::sideBarWidth, (unsigned int)UnitSizes::tileSize * (mSettings.mBoardHeight < 4 ? mSettings.mBoardHeight : UnitSizes::sideBarWidth));
	this->gameControlTexture.clear(sf::Color::Transparent);
	this->sideBarControlTexture.create((unsigned int)UnitSizes::tileSize * UnitSizes::sideBarWidth, (unsigned int)UnitSizes::tileSize * (mSettings.mBoardHeight < 4 ? mSettings.mBoardHeight : UnitSizes::sideBarWidth));
	this->sideBarControlTexture.clear(sf::Color::Transparent);
	{
		//labels to pair with each arrow indicating function
		std::array<sf::Text, 4> controlLabels;

		controlLabels[0] = sf::Text("Sidebar", mFont); //up
		controlLabels[1] = sf::Text("Drop", mFont); //down
		controlLabels[2] = sf::Text("Scroll", mFont); //left
		controlLabels[3] = sf::Text("Scroll", mFont); //right

		//centers origins of each label based on the game controls strings
		for (auto& label : controlLabels)
			label.setOrigin(label.getLocalBounds().width / 2.f, label.getLocalBounds().height / 2.f);

		//rotates left and right labels to fit better
		controlLabels[2].rotate(90);
		controlLabels[3].rotate(270);

		const float distFromDiagramEdges = .3f * UnitSizes::tileSize;

		//places each label
		controlLabels[0].setPosition(UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f, distFromDiagramEdges);
		controlLabels[1].setPosition(UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f, (UnitSizes::tileSize * UnitSizes::sideBarWidth) - distFromDiagramEdges);
		controlLabels[2].setPosition(distFromDiagramEdges, UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f);
		controlLabels[3].setPosition((UnitSizes::tileSize * UnitSizes::sideBarWidth) - distFromDiagramEdges, UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f);

		//adjusts the labels to be slightly higher to appear more properly centered, and adds them to the diagram
		for (auto& label : controlLabels) {
			label.move(0, -.1f * UnitSizes::tileSize);
			this->gameControlTexture.draw(label);
		}

		this->gameControlTexture.display();

		//switches the contents of each label to the sidebar navigation version
		controlLabels[0].setString("Scroll");
		controlLabels[1].setString("Scroll");
		controlLabels[2].setString("Select");
		controlLabels[3].setString("Back");

		//adjusts label origins based on these new strings
		for (auto& label : controlLabels)
			label.setOrigin(label.getLocalBounds().width / 2.f, label.getLocalBounds().height / 2.f);

		//adjusts placement based on new origins to center
		controlLabels[0].setPosition(UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f, distFromDiagramEdges);
		controlLabels[1].setPosition(UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f, (UnitSizes::tileSize * UnitSizes::sideBarWidth) - distFromDiagramEdges);
		controlLabels[2].setPosition(distFromDiagramEdges, UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f);
		controlLabels[3].setPosition((UnitSizes::tileSize * UnitSizes::sideBarWidth) - distFromDiagramEdges, UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f);

		//adjusts the labels to be slightly higher to appear more properly centered, and adds them to the diagram
		for (auto& label : controlLabels) {
			label.move(0, -.1f * UnitSizes::tileSize);
			this->sideBarControlTexture.draw(label);
		}
		this->sideBarControlTexture.display();
	}

	this->mControlDiagram = sf::Sprite(controlDiagramTexture.getTexture());
	this->mGameControlOverlay = sf::Sprite(gameControlTexture.getTexture());
	this->mSideBarControlOverlay = sf::Sprite(sideBarControlTexture.getTexture());

	this->boardBackgroundTexture.create((unsigned int)UnitSizes::tileSize* mSettings.mBoardWidth, (unsigned int)UnitSizes::tileSize* mSettings.mBoardHeight);
	this->boardBackgroundTexture.clear(); 
	{
		//define stripes 1 tile tall that span the width of the board
		std::array<sf::RectangleShape, 2> backgroundStripes = {
			sf::RectangleShape(sf::Vector2f(UnitSizes::tileSize * mSettings.mBoardWidth, UnitSizes::tileSize)),
			sf::RectangleShape(backgroundStripes[0])
		};

		//assign each stripe a color
		backgroundStripes[0].setFillColor(sf::Color(25, 25, 25));
		backgroundStripes[1].setFillColor(sf::Color::Black);

		//place each tile from the bottom up
		for (int i = mSettings.mBoardHeight; i > 0; --i) {
			backgroundStripes[i % backgroundStripes.size()].setPosition(0, UnitSizes::tileSize * (i - 1));
			this->boardBackgroundTexture.draw(backgroundStripes[i % backgroundStripes.size()]);
		}
	}
	this->boardBackgroundTexture.display();

	this->mBoardBackground = sf::Sprite(boardBackgroundTexture.getTexture());
	this->mBoardBackground.setPosition(UnitSizes::tileSize* UnitSizes::sideBarWidth, 0);

	//if the board is fewer than 4 tiles tall, place diagrams to the right of scoreboard instead of below to avoid overlap
	if (mSettings.mBoardHeight < 4)
		this->mControlDiagram.setPosition(UnitSizes::tileSize * ((UnitSizes::sideBarWidth * 2) + mSettings.mBoardWidth), 0);
	else
		this->mControlDiagram.setPosition(UnitSizes::tileSize * (UnitSizes::sideBarWidth + mSettings.mBoardWidth), UnitSizes::tileSize * (mSettings.mBoardHeight - UnitSizes::sideBarWidth));

	this->mGameControlOverlay.setPosition(mControlDiagram.getPosition());
	this->mSideBarControlOverlay.setPosition(mControlDiagram.getPosition());

	//board
	this->mBoard.reserve(mSettings.mBoardWidth);
	for (int i = 0; i < mSettings.mBoardWidth; ++i) {
		mBoard.push_back(std::vector<Token>());
		mBoard.back().reserve(mSettings.mBoardHeight);
	}

	//sidebar
	mSideBarTexts[0].setString("Main\nMenu");
	mSideBarTexts[1].setString("Reset\nBoard");
	mSideBarTexts[2].setString("Reset\nScores");
	for (int i = 0; i < sideBarButtonCount; ++i) {
		mSideBarBoxes[i] = sf::RectangleShape(sf::Vector2f(UnitSizes::tileSize * UnitSizes::sideBarWidth, UnitSizes::tileSize * mSettings.mBoardHeight / sideBarButtonCount));
		mSideBarBoxes[i].setPosition(0, i * UnitSizes::tileSize * mSettings.mBoardHeight / sideBarButtonCount);
		mSideBarBoxes[i].setOutlineThickness(UnitSizes::outlineThickness);
		mSideBarBoxes[i].setOutlineColor(sf::Color(50, 50, 50));
		mSideBarTexts[i].setFont(mFont);
		mSideBarTexts[i].setOrigin(mSideBarTexts[i].getLocalBounds().width / 2.f, mSideBarTexts[i].getLocalBounds().height / 2.f);
		mSideBarTexts[i].setPosition(UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f, UnitSizes::tileSize * mSettings.mBoardHeight * (i + .5f) / sideBarButtonCount);
		mSideBarTexts[i].move(0, -.1f * UnitSizes::tileSize);
	}
}

std::thread ConnectFour::startGameInstance()
{
	return std::thread([this] { runGame(); });
}

void ConnectFour::runGame(void)
{
	//creates bounding boxes to determine position of mouse when sidebar is hovered
	mBoundingRegions[0].reserve(sideBarButtonCount);
	for (int i = 0; i < sideBarButtonCount; ++i)
		mBoundingRegions[0].push_back(sf::FloatRect(mSideBarBoxes[i].getGlobalBounds()));

	//creates bounding boxes to determine position of mouse when board is hovered
	mBoundingRegions[1].reserve(mSettings.mBoardWidth);
	for (int i = 0; i < mSettings.mBoardWidth; ++i)
		mBoundingRegions[1].push_back(sf::FloatRect(UnitSizes::tileSize * (UnitSizes::sideBarWidth + i), 0, UnitSizes::tileSize, UnitSizes::tileSize * mSettings.mBoardHeight));

	//open game window
	mWindow.create(sf::VideoMode(
		(unsigned int)UnitSizes::tileSize * (UnitSizes::sideBarWidth + mSettings.mBoardWidth + (unsigned int)mSettings.mPlayerCount + (mSettings.mBoardHeight < 4 ? UnitSizes::sideBarWidth : 0)), //x
		(unsigned int)UnitSizes::tileSize * mSettings.mBoardHeight), //y
		"Connect Four");
	
	//game loop
	while (mWindow.isOpen()) {

		//event handling
		sf::Event event;
		while (mWindow.pollEvent(event)) {
			switch (event.type) {

			case sf::Event::Closed:
				mWindow.close();
				resetBoard();
				break;

			case sf::Event::MouseMoved:
				//sidebar hovered
				if (event.mouseMove.x < UnitSizes::tileSize * UnitSizes::sideBarWidth) {
					for (int i = 0; i < sideBarButtonCount; ++i) {
						if (mBoundingRegions[0][i].contains((float)event.mouseMove.x, (float)event.mouseMove.y)) {
							mLastSideBarSelection = mSideBarSelection = i;
							mInSidebar = true;
							break;
						}
					}
				}
				else {
					mSideBarSelection = -1;
					mInSidebar = false;
					//board hovered
					if (event.mouseMove.x < UnitSizes::tileSize * (UnitSizes::sideBarWidth	+ mSettings.mBoardWidth)) {
						for (int i = 0; i < mSettings.mBoardWidth; ++i) {
							if (mBoundingRegions[1][i].contains((float)event.mouseMove.x, (float)event.mouseMove.y)) {
								mSelectedCol = i;
								break;
							}
						}
					} 
				}
				break;

			case sf::Event::KeyPressed:
				switch (event.key.scancode) {

				case sf::Keyboard::Scan::Left:
				case sf::Keyboard::Scan::A:
					if (mInSidebar) {
						//select
						mSideBarSound.play();
						executeSidebarSelection();
					}
					else {
						//scroll selected column left
						if (--mSelectedCol < 0) mSelectedCol = mSettings.mBoardWidth - 1;
					}
					break;

				case sf::Keyboard::Scan::Right:
				case sf::Keyboard::Scan::D:
					if (mInSidebar) {
						//exit sidebar
						mSideBarSound.play();
						mInSidebar = false;
						mLastSideBarSelection = mSideBarSelection;
						mSideBarSelection = -1;
					}
					else {
						//scroll selected column right
						if (++mSelectedCol >= mSettings.mBoardWidth) mSelectedCol = 0;
					}
					break;

				case sf::Keyboard::Scan::Up:
				case sf::Keyboard::Scan::W:
					if (mInSidebar) {
						//scroll up
						mSideBarSound.play();
						if (--mSideBarSelection < 0) mSideBarSelection = sideBarButtonCount - 1;
					}
					else {
						//enter sidebar
						mSideBarSound.play();
						mInSidebar = true;
						mSideBarSelection = mLastSideBarSelection;
					}
					break;

				case sf::Keyboard::Scan::Down:
				case sf::Keyboard::Scan::S:
					if (mInSidebar) {
						//scroll down
						mSideBarSound.play();
						if (++mSideBarSelection >= sideBarButtonCount) mSideBarSelection = 0;
					}
					else {
						//make move
						simulateMove();
					}
					break;
				}
				break;
			case sf::Event::MouseButtonPressed:
				switch (event.mouseButton.button) {
				case sf::Mouse::Left: //make move
					if (event.mouseButton.x < UnitSizes::tileSize * UnitSizes::sideBarWidth) {
						for (int i = 0; i < sideBarButtonCount; ++i) {
							if (mBoundingRegions[0][i].contains((float)event.mouseButton.x, (float)event.mouseButton.y)) {
								mLastSideBarSelection = mSideBarSelection = i;
								mInSidebar = true;
								mSideBarSound.play();
								executeSidebarSelection();
								break;
							}
						}
					}
					else {
						mSideBarSelection = -1;
						mInSidebar = false;
						//board hovered
						if (event.mouseMove.x < UnitSizes::tileSize * (UnitSizes::sideBarWidth + mSettings.mBoardWidth)) {
							for (int i = 0; i < mSettings.mBoardWidth; ++i) {
								if (mBoundingRegions[1][i].contains((float)event.mouseButton.x, (float)event.mouseButton.y)) {
									mSelectedCol = i;
									simulateMove();
									break;
								}
							}
						}
					}
					
				}
			}
		}
		
		//begin rendering display
		mWindow.clear();

		/*********LEFT OF SCREEN**********/

		//std::thread screenRenderLeft([this] {

			//sidebar
			for (int i = 0; i < sideBarButtonCount; ++i) {
				//sets color to indicate selection
				if (i == mSideBarSelection) mSideBarBoxes[i].setFillColor(sf::Color(60, 60, 60));
				else mSideBarBoxes[i].setFillColor(sf::Color(100, 100, 100));
			}
			for (auto& sideBarBox : mSideBarBoxes)
				mWindow.draw(sideBarBox);
			for (auto& sideBarText : mSideBarTexts)
				mWindow.draw(sideBarText);
		//});

		/*********MIDDLE OF SCREEN*********/

		//std::thread screenRenderMiddle([this] {

			//board background
			mWindow.draw(mBoardBackground);

			//selection indicator
			if (!mInSidebar) {
				mSelectionBar.setPosition((mSelectedCol + UnitSizes::sideBarWidth + .25f) * UnitSizes::tileSize, 0);
				mWindow.draw(mSelectionBar);
			}

			//board
			for (const auto& col : mBoard) {
				for (const auto& token : col) {
					mWindow.draw(token.getTokenGraphic());
				}
			}
		//});

		/*********RIGHT OF SCREEN**********/

		//std::thread screenRenderRight([this] {

			//scoreboard
			mWindow.draw(mScoreBoard);
			for (int i = 0; i < mSettings.mPlayerCount; ++i) {
				sf::Text score(std::to_string(mSettings.mPlayerInfo[i].getWinCount()), mFont);
				score.setOrigin(score.getLocalBounds().width / 2, score.getLocalBounds().height / 2);
				//vertically aligned with labels
				score.setPosition(UnitSizes::tileSize * (UnitSizes::sideBarWidth + mSettings.mBoardWidth + .5f + i), UnitSizes::tileSize * 1.5f);
				mWindow.draw(score);
			}

			//controls diagram
			mWindow.draw(mControlDiagram);
			if (mInSidebar) mWindow.draw(mSideBarControlOverlay);
			else mWindow.draw(mGameControlOverlay);
		//});

		//screenRenderLeft.join();
		//screenRenderMiddle.join();
		//screenRenderRight.join();

		//win handling
		if (mRoundWinner != -1) {
			mWinSound.play();

			mSettings.mPlayerInfo[mRoundWinner].incWinCount();

			//win message 
			sf::Text winText("Player " + std::to_string(mRoundWinner + 1) + " wins!\n(Press any key to continue)", mFont);
			winText.setPosition(UnitSizes::tileSize * (UnitSizes::sideBarWidth + .5f), UnitSizes::tileSize * .5f);
			winText.setOutlineThickness(4.f);
			winText.setOutlineColor(sf::Color::Black);
			mWindow.draw(winText);

			mWindow.display();

			resetBoard();

			//pause on win until keypress
			do {
				mWindow.waitEvent(event);
				if (event.type == sf::Event::Closed) mWindow.close();
			} while (event.type != sf::Event::KeyPressed && event.type != sf::Event::MouseButtonPressed);
		}
		else mWindow.display();
	}
}

bool ConnectFour::makeMove(int player, int column)
{
	//disallows placing tokens above the board
	if (mBoard[column].size() >= mSettings.mBoardHeight)
		//indicates no move was successfully made
		return false;

	//adds a token to the appropriate column
	mBoard[column].push_back((mSettings.mPlayerInfo)[player].getToken());
	
	//sets position for the token to be rendered in
	mBoard[column].back().setPosition(
		(UnitSizes::tileSize) * (column + UnitSizes::sideBarWidth), //x 
		(UnitSizes::tileSize) * (mSettings.mBoardHeight - mBoard[column].size()) //y
	);

	//indicates successful entry of a move
	return true;
}

void ConnectFour::simulateMove(void)
{
	if (makeMove(mCurrPlayer, mSelectedCol)) {
		mDropSound.play();

		//win checking
		if (checkForWin(mSelectedCol, (int)mBoard[mSelectedCol].size() - 1))
			mRoundWinner = mCurrPlayer;
		//checks for draws, resets board if is filled
		else if (++mMoveCount >= mSettings.mBoardHeight * mSettings.mBoardWidth)
			resetBoard();

		//switches to the next player's turn
		if (++mCurrPlayer >= mSettings.mPlayerCount) mCurrPlayer = 0;

		//recolors the selection bar appropriately (based on the new current player)
		mSelectionBar.setFillColor(mSettings.mPlayerInfo[mCurrPlayer].getToken().getColor());
	}
}

void ConnectFour::executeSidebarSelection()
{
	switch (mSideBarSelection) {
	case 0: //main menu
		mWindow.close();
		break;
	case 1: //reset board
		resetBoard();
		break;
	case 2: //reset scores
		resetScores();
		break;
	}
}

bool ConnectFour::checkForWin(int lastX, int lastY) const
{
	//token of player that most recently moved
	Token toMatch = mBoard[lastX][lastY];

	//checks for vertical win
	//if this column is tall enough for this possibility
	if (mBoard[lastX].size() >= mSettings.mWinRequirement) {
		//scans down thru this column from the toMatch token
		for (int i = 1; i <= mSettings.mWinRequirement; ) {
			//continue iff the sequence stays unbroken
			if (mBoard[lastX][(size_t)lastY - i] != toMatch) break;
			//if a vertical sequence was found, report win
			if (++i >= mSettings.mWinRequirement) return true;
		}
	}

	//counts corresponding to sequences(branches) reaching/passing thru this point, starting from the bottom left, left, and top left respectively
	//starts with 1 on account of the inclusion of the toMatch token towards a full sequence
	int countsOfBranches[3] = { 1, 1, 1 };
	
	//checks left side of this column
	bool continueCheckingOnLeft[3] = { true, true, true };
	for (int i = 1; i < mSettings.mWinRequirement; ++i) {

		//if there exists columns to the left no more than i tokens shorter than this column
		if (lastX - i >= 0 && (int)mBoard[(size_t)lastX - i].size() > lastY - i) {
			if (continueCheckingOnLeft[0] && lastY - i >= 0 && mBoard[(size_t)lastX - i][(size_t)lastY - i] == toMatch) ++countsOfBranches[0];
			else continueCheckingOnLeft[0] = false;

			//if there exists columns to the left at least the height of this column
			if ((int)mBoard[(size_t)lastX - i].size() > lastY) {
				if (continueCheckingOnLeft[1] && mBoard[(size_t)lastX - i][lastY] == toMatch) ++countsOfBranches[1];
				else continueCheckingOnLeft[1] = false;

				//if there exists columns to the left at least i tokens taller than this column
				if ((int)mBoard[(size_t)lastX - i].size() > lastY + i) {
					if (continueCheckingOnLeft[2] && mBoard[(size_t)lastX - i][(size_t)lastY + i] == toMatch) ++countsOfBranches[2];
					else continueCheckingOnLeft[2] = false;
				}
			}
		}
	}
	//checks if a full sequence was foud on the left
	for (int count : countsOfBranches) if (count >= mSettings.mWinRequirement) return true;

	//top right, right, bottom right (opposite corresponding left branches)
	bool continueCheckingOnRight[3] = { true, true, true };
	for (int i = 1; i < mSettings.mWinRequirement; ++i) {

		//if there exists columns to the right && if those are no more than i tokens shorter than this column
		if (lastX + i < mSettings.mBoardWidth && (int)mBoard[(size_t)lastX + i].size() > lastY - i) {
			if (continueCheckingOnRight[2] && lastY - i >= 0 && mBoard[(size_t)lastX + i][(size_t)lastY - i] == toMatch) ++countsOfBranches[2];
			else continueCheckingOnRight[2] = false;

			//if there exists columns to the right at least the height of this column
			if ((int)mBoard[(size_t)lastX + i].size() > lastY) {
				if (continueCheckingOnRight[1] && mBoard[(size_t)lastX + i][lastY] == toMatch) ++countsOfBranches[1];
				else continueCheckingOnRight[1] = false;

				//if there exists columns to the right at least i tokens taller than this column
				if ((int)mBoard[(size_t)lastX + i].size() > lastY + i) {
					if (continueCheckingOnRight[0] && mBoard[(size_t)lastX + i][(size_t)lastY + i] == toMatch) ++countsOfBranches[0];
					else continueCheckingOnRight[0] = false;
				}
			}
			//checks if a full sequence has been found yet between the sum of the corresponding left and right branches
			for (int count : countsOfBranches) if (count >= mSettings.mWinRequirement) return true;
		}
	}
	//no sufficient sequence was found
	return false;
}

void ConnectFour::resetBoard(void)
{
	//clears each column
	for (auto& col : mBoard) col.clear();

	//resets move counter
	mMoveCount = 0;

	//resets winner flag
	mRoundWinner = -1;
}

void ConnectFour::resetScores(void)
{
	for (auto& player : mSettings.mPlayerInfo) player.resetWinCount();
}
