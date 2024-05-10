#include "ConnectFour.hpp"

ConnectFour::ConnectFour(Settings& settings) :
	mSettings(settings),
	mMoveCount(0),
	mCurrPlayer(0),
	mSelectedCol(0),
	mRoundWinner(-1),
	mHoveringControlDiagram(false),
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

	//control diagram arrow
	mControlDiagramArrow = sf::ConvexShape(3);
	mControlDiagramArrow.setPoint(0, sf::Vector2f(0, UnitSizes::tileSize * .3f));
	mControlDiagramArrow.setPoint(1, sf::Vector2f(UnitSizes::tileSize * .15f, 0));
	mControlDiagramArrow.setPoint(2, sf::Vector2f(UnitSizes::tileSize * .3f, UnitSizes::tileSize * .3f));
	mControlDiagramArrow.setOrigin(sf::Vector2f(UnitSizes::tileSize * .15f, UnitSizes::tileSize * .5f));

}

bool ConnectFour::runGame(void)
{
	reloadSizeDependentElements();

	//number of groups of bounding boxes
	constexpr static int boundingRegionCount = 3;
	std::array<std::vector<sf::FloatRect>, boundingRegionCount> boundingRegions;

	//creates bounding boxes to determine position of mouse when sidebar is hovered
	boundingRegions[0].reserve(sideBarButtonCount);
	for (int i = 0; i < sideBarButtonCount; ++i)
		boundingRegions[0].push_back(sf::FloatRect(mSideBarBoxes[i].getGlobalBounds()));

	//creates bounding boxes to determine position of mouse when board is hovered
	boundingRegions[1].reserve(mSettings.mBoardWidth);
	for (int i = 0; i < mSettings.mBoardWidth; ++i)
		boundingRegions[1].push_back(sf::FloatRect(UnitSizes::tileSize * (UnitSizes::sideBarWidth + i), 0, UnitSizes::tileSize, UnitSizes::tileSize * mSettings.mBoardHeight));

	//creates a bounding box for the control diagram
	boundingRegions[2] = { sf::FloatRect(mControlDiagram.getGlobalBounds()) };
	
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
				return false;

			case sf::Event::MouseMoved:
				mHoveringControlDiagram = false;
				//sidebar hovered
				if (mWindow.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)).x < UnitSizes::tileSize * UnitSizes::sideBarWidth) {
					for (int i = 0; i < sideBarButtonCount; ++i) {
						if (boundingRegions[0][i].contains(mWindow.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)))) {
							mLastSideBarSelection = mSideBarSelection = i;
							mInSidebar = true;
							break;
						}
					}
				}
				else {
					//board hovered
					if (mWindow.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)).x < UnitSizes::tileSize * (UnitSizes::sideBarWidth	+ mSettings.mBoardWidth)) {
						mSideBarSelection = -1;
						mInSidebar = false;
						for (int i = 0; i < mSettings.mBoardWidth; ++i) {
							if (boundingRegions[1][i].contains(mWindow.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)))) {
								mSelectedCol = i;
								break;
							}
						}
					} 
					//control diagram hovered
					else if (boundingRegions[2][0].contains(mWindow.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y)))) {
						mHoveringControlDiagram = true;
						const sf::Vector2i diagramCorner = mWindow.mapCoordsToPixel(boundingRegions[2][0].getPosition());
						//up or right
						if (event.mouseMove.x - diagramCorner.x > event.mouseMove.y - diagramCorner.y) {
							//up
							if ((event.mouseMove.x + event.mouseMove.y) - (diagramCorner.x + diagramCorner.y) < mWindow.mapCoordsToPixel(sf::Vector2f(UnitSizes::tileSize * UnitSizes::sideBarWidth, 0)).x) {
								mControlDiagramArrow.setRotation(0);
							}

							//right
							else {
								mControlDiagramArrow.setRotation(90);
							}
						}
						//down or left
						else {
							//left
							if ((event.mouseMove.x + event.mouseMove.y) - (diagramCorner.x + diagramCorner.y) > mWindow.mapCoordsToPixel(sf::Vector2f(UnitSizes::tileSize * UnitSizes::sideBarWidth, 0)).x) {
								mControlDiagramArrow.setRotation(180);
							}

							//down
							else {
								mControlDiagramArrow.setRotation(270);
							}
						}
					}
					else {
						mSideBarSelection = -1;
						mInSidebar = false;
					}
				}
				break;

			case sf::Event::MouseButtonPressed:
				switch (event.mouseButton.button) {
				case sf::Mouse::Left: //make move
					if (mWindow.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)).x < UnitSizes::tileSize * UnitSizes::sideBarWidth) {
						for (int i = 0; i < sideBarButtonCount; ++i) {
							if (boundingRegions[0][i].contains(mWindow.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)))) {
								mLastSideBarSelection = mSideBarSelection = i;
								mInSidebar = true;
								mSideBarSound.play();
								executeSidebarSelection();
								break;
							}
						}
					}
					else {
						
						//board hovered
						if (mWindow.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)).x < UnitSizes::tileSize * (UnitSizes::sideBarWidth + mSettings.mBoardWidth)) {
							mSideBarSelection = -1;
							mInSidebar = false;
							for (int i = 0; i < mSettings.mBoardWidth; ++i) {
								if (boundingRegions[1][i].contains(mWindow.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)))) {
									mSelectedCol = i;
									simulateMove();
									break;
								}
							}
						}
						//control diagram hovered
						else if (boundingRegions[2][0].contains(mWindow.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y)))) {
							mHoveringControlDiagram = true;
							const sf::Vector2i diagramCorner = mWindow.mapCoordsToPixel(boundingRegions[2][0].getPosition());
							//up or right
							if (event.mouseButton.x - diagramCorner.x > event.mouseButton.y - diagramCorner.y) {
								//up
								if ((event.mouseButton.x + event.mouseButton.y) - (diagramCorner.x + diagramCorner.y) < mWindow.mapCoordsToPixel(sf::Vector2f(UnitSizes::tileSize * UnitSizes::sideBarWidth, 0)).x) {
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
								}
									
								//right
								else {
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
								}
							}
							//down or left
							else {
								//left
								if ((event.mouseButton.x + event.mouseButton.y) - (diagramCorner.x + diagramCorner.y) < mWindow.mapCoordsToPixel(sf::Vector2f(UnitSizes::tileSize * UnitSizes::sideBarWidth, 0)).x) {
									if (mInSidebar) {
										//select
										mSideBarSound.play();
										executeSidebarSelection();
									}
									else {
										//scroll selected column left
										if (--mSelectedCol < 0) mSelectedCol = mSettings.mBoardWidth - 1;
									}
								}
									
								//down
								else {
									if (mInSidebar) {
										//scroll down
										mSideBarSound.play();
										if (++mSideBarSelection >= sideBarButtonCount) mSideBarSelection = 0;
									}
									else {
										//make move
										simulateMove();
									}
								}
							}
						}
						else {
							mSideBarSelection = -1;
							mInSidebar = false;
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
			}
		}
		
		//begin rendering display
		mWindow.clear();

		/*********LEFT OF SCREEN**********/

		//sidebar
		for (int i = 0; i < sideBarButtonCount; ++i) {
			//sets color to indicate selection
			if (i == mSideBarSelection) mSideBarBoxes[i].setFillColor(UIColors::selection);
			else mSideBarBoxes[i].setFillColor(UIColors::main);
		}
		for (auto& sideBarBox : mSideBarBoxes)
			mWindow.draw(sideBarBox);
		for (auto& sideBarText : mSideBarTexts)
			mWindow.draw(sideBarText);

		/*********MIDDLE OF SCREEN*********/

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

		/*********RIGHT OF SCREEN**********/

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
		if (mHoveringControlDiagram) mWindow.draw(mControlDiagramArrow);
		if (mInSidebar) mWindow.draw(mSideBarControlOverlay);
		else mWindow.draw(mGameControlOverlay);

		/**********************************/

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
	return true;
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
	const Token toMatch = mBoard[lastX][lastY];

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

void ConnectFour::reloadSizeDependentElements()
{
	//selection bar
	this->mSelectionBar = sf::RectangleShape(sf::Vector2f(UnitSizes::tileSize / 2.f, UnitSizes::tileSize * mSettings.mBoardHeight));
	this->mSelectionBar.setFillColor(mSettings.mPlayerInfo[0].getToken().getColor());
	this->mSelectionBar.setPosition(UnitSizes::tileSize * (UnitSizes::sideBarWidth + .25f), 0);
	this->mSelectionBar.setOutlineThickness(UnitSizes::outlineThickness);
	this->mSelectionBar.setOutlineColor(UIColors::outline);

	//scoreboard
	this->scoreBoardTexture.create((unsigned int)UnitSizes::tileSize * mSettings.mPlayerCount, (unsigned int)UnitSizes::tileSize * mSettings.mBoardHeight);
	this->scoreBoardTexture.clear();
	{
		//create scoreboard background
		sf::RectangleShape scoreBoardBackGround(sf::Vector2f(UnitSizes::tileSize * mSettings.mPlayerCount, UnitSizes::tileSize * mSettings.mBoardHeight));
		scoreBoardBackGround.setFillColor(UIColors::main);
		scoreBoardBackGround.setOutlineThickness(UnitSizes::outlineThickness);
		scoreBoardBackGround.setOutlineColor(UIColors::outline);

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

	//control diagram
	this->controlDiagramTexture.create((unsigned int)UnitSizes::tileSize * UnitSizes::sideBarWidth, (unsigned int)UnitSizes::tileSize * (mSettings.mBoardHeight < 4 ? mSettings.mBoardHeight : UnitSizes::sideBarWidth));
	this->controlDiagramTexture.clear();
	{
		//diagram background
		sf::RectangleShape controlDiagramBackground(sf::Vector2f(UnitSizes::tileSize * UnitSizes::sideBarWidth, UnitSizes::tileSize * (float)(mSettings.mBoardHeight < 4 ? mSettings.mBoardHeight : UnitSizes::sideBarWidth)));
		controlDiagramBackground.setFillColor(UIColors::main);
		controlDiagramBackground.setOutlineThickness(UnitSizes::outlineThickness);
		controlDiagramBackground.setOutlineColor(UIColors::outline);
		this->controlDiagramTexture.draw(controlDiagramBackground);

		//arrows corresponding to directions (ie user inputs via wasd/arrows)
		mControlDiagramArrow.setFillColor(sf::Color::White);
		mControlDiagramArrow.setPosition(UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f, UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f);
		for (int i = 0; i < 4; ++i) {
			this->controlDiagramTexture.draw(mControlDiagramArrow);
			mControlDiagramArrow.rotate(90);
		}

		this->controlDiagramTexture.display();
		
		//future use of this arrow will be to indicate mouse hovers
		mControlDiagramArrow.setFillColor(UIColors::selection);
	}
	//controls diagram text overlays
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

	if (mSettings.mBoardHeight < 4)
		this->mControlDiagram.setPosition(UnitSizes::tileSize * (UnitSizes::sideBarWidth * 2 + mSettings.mBoardWidth + (mSettings.mPlayerCount - UnitSizes::sideBarWidth)), 0);
	else
		this->mControlDiagram.setPosition(UnitSizes::tileSize * (UnitSizes::sideBarWidth + mSettings.mBoardWidth + (mSettings.mPlayerCount - UnitSizes::sideBarWidth)), UnitSizes::tileSize * (mSettings.mBoardHeight - UnitSizes::sideBarWidth));
	this->mGameControlOverlay.setPosition(mControlDiagram.getPosition());
	this->mSideBarControlOverlay.setPosition(mControlDiagram.getPosition());
	this->mControlDiagramArrow.move(mControlDiagram.getPosition());

	//board background
	this->boardBackgroundTexture.create((unsigned int)UnitSizes::tileSize * mSettings.mBoardWidth, (unsigned int)UnitSizes::tileSize * mSettings.mBoardHeight);
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
		const int offset = mSettings.mBoardHeight % backgroundStripes.size();
		for (int i = mSettings.mBoardHeight; i >= 0; --i) {
			backgroundStripes[(size_t)(i + offset) % backgroundStripes.size()].setPosition(0, UnitSizes::tileSize* (i - 1));
			this->boardBackgroundTexture.draw(backgroundStripes[(size_t)(i + offset) % backgroundStripes.size()]);
		}

		//notches along the width of the board to show the placement of the columns
		sf::RectangleShape notch(sf::Vector2f(UnitSizes::tileSize * .05f, UnitSizes::tileSize * .2f));
		notch.setOrigin(notch.getLocalBounds().width / 2.f, notch.getLocalBounds().height);
		notch.setFillColor(UIColors::main);
		for (int i = 1; i < mSettings.mBoardWidth; ++i) {
			notch.setPosition(UnitSizes::tileSize * i, UnitSizes::tileSize * mSettings.mBoardHeight);
			boardBackgroundTexture.draw(notch);
		}
	}
	this->boardBackgroundTexture.display();

	this->mBoardBackground = sf::Sprite(boardBackgroundTexture.getTexture());
	this->mBoardBackground.setPosition(UnitSizes::tileSize * UnitSizes::sideBarWidth, 0);

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
		mSideBarBoxes[i].setOutlineColor(UIColors::outline);
		mSideBarTexts[i].setFont(mFont);
		mSideBarTexts[i].setOrigin(mSideBarTexts[i].getLocalBounds().width / 2.f, mSideBarTexts[i].getLocalBounds().height / 2.f);
		mSideBarTexts[i].setPosition(UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f, UnitSizes::tileSize * mSettings.mBoardHeight * (i + .5f) / sideBarButtonCount);
		mSideBarTexts[i].move(0, -.1f * UnitSizes::tileSize);
	}
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
