#include "ConnectFour.hpp"

ConnectFour::ConnectFour(Settings& settings) :
	mSettings(settings), 
	mMoveCount(0)
{
	this->mBoard.reserve(mSettings.mBoardWidth);
	for (int i = 0; i < mSettings.mBoardWidth; ++i) {
		mBoard.push_back(std::vector<Token>());
		mBoard.back().reserve(mSettings.mBoardHeight);
	}
}

std::thread ConnectFour::startGameInstance()
{
	return std::thread([this] { runGame(); });
}

void ConnectFour::runGame(void)
{
	//load sound effects
	sf::SoundBuffer drop;
	sf::SoundBuffer win;
	sf::SoundBuffer sideBar;
	drop.loadFromMemory(&Sounds::pop_wav, Sounds::pop_wav_len); 
	win.loadFromMemory(&Sounds::win_wav, Sounds::win_wav_len);
	sideBar.loadFromMemory(&Sounds::click_mp3, Sounds::click_mp3_len);
	sf::Sound dropSound(drop);
	sf::Sound winSound(win);
	sf::Sound sideBarSound(sideBar);
	sideBarSound.setVolume(20);

	//load font
	sf::Font font;
	font.loadFromMemory(&Fonts::PixelifySans_ttf, Fonts::PixelifySans_ttf_len);
	
	/*********LEFT OF SCREEN**********/

	//load sidebar
	const int sideBarButtonCount = 3;
	std::array<sf::RectangleShape, sideBarButtonCount> sideBarBoxes;
	std::array<sf::Text, sideBarButtonCount> sideBarTexts;
	{
		sideBarTexts[0].setString("Main\nMenu");
		sideBarTexts[1].setString("Reset\nBoard");
		sideBarTexts[2].setString("Reset\nScores");
		for (int i = 0; i < sideBarButtonCount; ++i) {
			sideBarBoxes[i] = sf::RectangleShape(sf::Vector2f(UnitSizes::tileSize * UnitSizes::sideBarWidth, UnitSizes::tileSize * mSettings.mBoardHeight / sideBarButtonCount));
			sideBarBoxes[i].setPosition(0, i * UnitSizes::tileSize * mSettings.mBoardHeight / sideBarButtonCount);
			sideBarBoxes[i].setOutlineThickness(UnitSizes::outlineThickness); 
			sideBarBoxes[i].setOutlineColor(sf::Color(50, 50, 50));
			sideBarTexts[i].setFont(font);
			sideBarTexts[i].setOrigin(sideBarTexts[i].getLocalBounds().width / 2.f, sideBarTexts[i].getLocalBounds().height / 2.f);
			sideBarTexts[i].setPosition(UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f, UnitSizes::tileSize * mSettings.mBoardHeight * (i + .5f) / sideBarButtonCount);
			sideBarTexts[i].move(0, -.1f * UnitSizes::tileSize);
		}
	}

	/*********RIGHT OF SCREEN**********/

	//load scoreboard
	sf::RenderTexture scoreBoardTexture;
	{
		scoreBoardTexture.create((unsigned int)UnitSizes::tileSize * mSettings.mPlayerCount, (unsigned int) UnitSizes::tileSize * mSettings.mBoardHeight);
		scoreBoardTexture.clear();

		//create scoreboard background
		sf::RectangleShape scoreBoardBackGround(sf::Vector2f(UnitSizes::tileSize * mSettings.mPlayerCount, UnitSizes::tileSize * mSettings.mBoardHeight));
		scoreBoardBackGround.setFillColor(sf::Color(100, 100, 100));
		scoreBoardBackGround.setOutlineThickness(UnitSizes::outlineThickness);
		scoreBoardBackGround.setOutlineColor(sf::Color(50, 50, 50));

		scoreBoardTexture.draw(scoreBoardBackGround);

		//create main header
		sf::Text header("Scoreboard", font);
		header.setOrigin(header.getLocalBounds().width / 2.f, header.getLocalBounds().height / 2.f);
		header.setPosition(UnitSizes::tileSize * mSettings.mPlayerCount / 2.f, UnitSizes::tileSize / 2.f);

		scoreBoardTexture.draw(header);

		//create individual labels for each player
		for (int i = 0; i < mSettings.mPlayerInfo.size(); ++i) {
			auto label = sf::Text("P" + std::to_string(i + 1), font);
			label.setFillColor(mSettings.mPlayerInfo[i].getToken().getColor());
			label.setOrigin(label.getLocalBounds().width / 2.f, label.getLocalBounds().height / 2.f);
			label.setPosition((.5f + i) * UnitSizes::tileSize, UnitSizes::tileSize);

			scoreBoardTexture.draw(label);
		}

		scoreBoardTexture.display();
	}

	sf::Sprite scoreBoard(scoreBoardTexture.getTexture());
	scoreBoard.setPosition(UnitSizes::tileSize * (UnitSizes::sideBarWidth + mSettings.mBoardWidth), 0);


	//controls diagrams
	sf::RenderTexture gameControlTexture, sideBarControlTexture;
	{
		gameControlTexture.create((unsigned int)UnitSizes::tileSize * UnitSizes::sideBarWidth, (unsigned int)UnitSizes::tileSize * (mSettings.mBoardHeight < 4 ? mSettings.mBoardHeight : UnitSizes::sideBarWidth));
		gameControlTexture.clear();
		sideBarControlTexture.create((unsigned int)UnitSizes::tileSize * UnitSizes::sideBarWidth, (unsigned int)UnitSizes::tileSize * (mSettings.mBoardHeight < 4 ? mSettings.mBoardHeight : UnitSizes::sideBarWidth));
		sideBarControlTexture.clear();

		sf::RectangleShape controlDiagramBackground(sf::Vector2f(UnitSizes::tileSize * UnitSizes::sideBarWidth, UnitSizes::tileSize * (float)(mSettings.mBoardHeight < 4 ? mSettings.mBoardHeight : UnitSizes::sideBarWidth)));
		controlDiagramBackground.setFillColor(sf::Color(100, 100, 100));
		controlDiagramBackground.setOutlineThickness(UnitSizes::outlineThickness);
		controlDiagramBackground.setOutlineColor(sf::Color(50, 50, 50));
		gameControlTexture.draw(controlDiagramBackground);
		sideBarControlTexture.draw(controlDiagramBackground);

		//arrows corresponding to directions (ie user inputs via wasd/arrows)
		sf::ConvexShape arrow(3);
		arrow.setPoint(0, sf::Vector2f(0, UnitSizes::tileSize * .3f));
		arrow.setPoint(1, sf::Vector2f(UnitSizes::tileSize * .15f, 0));
		arrow.setPoint(2, sf::Vector2f(UnitSizes::tileSize * .3f, UnitSizes::tileSize * .3f));
		arrow.setOrigin(sf::Vector2f(UnitSizes::tileSize * .15f, UnitSizes::tileSize * .5f));
		arrow.setPosition(UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f, UnitSizes::tileSize * UnitSizes::sideBarWidth / 2.f);
		
		gameControlTexture.draw(arrow);
		sideBarControlTexture.draw(arrow);

		//add these arrows to both versions of the diagram
		for (int i = 0; i < 3; ++i) {
			arrow.rotate(90);
			gameControlTexture.draw(arrow);
			sideBarControlTexture.draw(arrow);
		}

		//labels to pair with each arrow indicating function
		std::array<sf::Text, 4> controlLabels;

		controlLabels[0] = sf::Text("Sidebar", font); //up
		controlLabels[1] = sf::Text("Drop", font); //down
		controlLabels[2] = sf::Text("Scroll", font); //left
		controlLabels[3] = sf::Text("Scroll", font); //right

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
			gameControlTexture.draw(label);
		}

		gameControlTexture.display();

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
			sideBarControlTexture.draw(label);
		}

		sideBarControlTexture.display();
	}

	sf::Sprite gameControlVisual(gameControlTexture.getTexture());
	sf::Sprite sideBarControlVisual(sideBarControlTexture.getTexture());
	
	//if the board is fewer than 4 tiles tall, place diagrams to the right of scoreboard instead of below to avoid overlap
	if (mSettings.mBoardHeight < 4) 
		gameControlVisual.setPosition(UnitSizes::tileSize * ((UnitSizes::sideBarWidth * 2) + mSettings.mBoardWidth), 0);
	else 
		gameControlVisual.setPosition(UnitSizes::tileSize * (UnitSizes::sideBarWidth + mSettings.mBoardWidth), UnitSizes::tileSize * (mSettings.mBoardHeight - UnitSizes::sideBarWidth));
	
	sideBarControlVisual.setPosition(gameControlVisual.getPosition());

	/*********MIDDLE OF SCREEN*********/

	sf::RenderTexture boardBackgroundTexture;
	{
		boardBackgroundTexture.create((unsigned int)UnitSizes::tileSize * mSettings.mBoardWidth, (unsigned int)UnitSizes::tileSize * mSettings.mBoardHeight);
		boardBackgroundTexture.clear();

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
			boardBackgroundTexture.draw(backgroundStripes[i % backgroundStripes.size()]);
		}

		boardBackgroundTexture.display();
	}

	sf::Sprite boardBackground(boardBackgroundTexture.getTexture());
	boardBackground.setPosition(UnitSizes::tileSize * UnitSizes::sideBarWidth, 0);

	//load selection bar and set initial color (based on first player)
	sf::RectangleShape selectionBar(sf::Vector2f(UnitSizes::tileSize / 2.f, UnitSizes::tileSize * mSettings.mBoardHeight));
	{
		selectionBar.setFillColor(mSettings.mPlayerInfo[0].getToken().getColor());
		selectionBar.setPosition(UnitSizes::tileSize * (UnitSizes::sideBarWidth + .25f), 0);
		selectionBar.setOutlineThickness(UnitSizes::outlineThickness);
		selectionBar.setOutlineColor(sf::Color(0, 0, 0, 100));
	}

	/**********************************/
	
	int selectedCol = 0;
	int currPlayer = 0;
	int roundWinner = -1;

	bool inSidebar = false;
	int sideBarSelection = -1;
	int lastSideBarSelection = 0;

	//game window
	sf::RenderWindow window(sf::VideoMode(
		(unsigned int)UnitSizes::tileSize * (UnitSizes::sideBarWidth + mSettings.mBoardWidth + (unsigned int)mSettings.mPlayerCount + (mSettings.mBoardHeight < 4 ? UnitSizes::sideBarWidth : 0)), //x
		(unsigned int)UnitSizes::tileSize * mSettings.mBoardHeight), //y
		"Connect Four");
	
	//game loop
	while (window.isOpen()) {

		//event handling
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {

			case sf::Event::Closed:
				window.close();
				resetBoard();
				break;

			case sf::Event::KeyPressed:
				switch (event.key.scancode) {

				case sf::Keyboard::Scan::Left:
				case sf::Keyboard::Scan::A:
					if (inSidebar) {
						//select
						sideBarSound.play();
						switch (sideBarSelection) {
						case 0: //main menu
							window.close();
							break;
						case 1: //reset board
							resetBoard();
							break;
						case 2: //reset scores
							resetScores();
							break;
						}
					}
					else {
						//scroll selected column left
						if (--selectedCol < 0) selectedCol = mSettings.mBoardWidth - 1;
						selectionBar.setPosition((selectedCol + UnitSizes::sideBarWidth + .25f) * UnitSizes::tileSize, 0);
					}
					break;

				case sf::Keyboard::Scan::Right:
				case sf::Keyboard::Scan::D:
					if (inSidebar) {
						//exit sidebar
						sideBarSound.play();
						inSidebar = false;
						lastSideBarSelection = sideBarSelection;
						sideBarSelection = -1;
					}
					else {
						//scroll selected column right
						if (++selectedCol >= mSettings.mBoardWidth) selectedCol = 0;
						selectionBar.setPosition((selectedCol + UnitSizes::sideBarWidth + .25f) * UnitSizes::tileSize, 0);
					}
					break;

				case sf::Keyboard::Scan::Up:
				case sf::Keyboard::Scan::W:
					if (inSidebar) {
						//scroll up
						sideBarSound.play();
						if (--sideBarSelection < 0) sideBarSelection = sideBarButtonCount - 1;
					}
					else {
						//enter sidebar
						sideBarSound.play();
						inSidebar = true;
						sideBarSelection = lastSideBarSelection;
					}
					break;

				case sf::Keyboard::Scan::Down:
				case sf::Keyboard::Scan::S:
					if (inSidebar) {
						//scroll down
						sideBarSound.play();
						if (++sideBarSelection >= sideBarButtonCount) sideBarSelection = 0;

					}
					else {
						//make move
						if (makeMove(currPlayer, selectedCol)) {
							dropSound.play();

							//win checking
							if (checkForWin(selectedCol, (int)mBoard[selectedCol].size() - 1))
								roundWinner = currPlayer;
							//checks for draws, resets board if is filled
							else if (++mMoveCount >= mSettings.mBoardHeight * mSettings.mBoardWidth)
								resetBoard();

							//switches to the next player's turn
							if (++currPlayer >= mSettings.mPlayerCount) currPlayer = 0;

							//recolors the selection bar appropriately (based on the new current player)
							selectionBar.setFillColor(mSettings.mPlayerInfo[currPlayer].getToken().getColor());
						}
					}
					break;
				}
				break;
			}
		}
		
		//begin rendering display
		window.clear();

		//board background
		window.draw(boardBackground);

		//selection indicator
		if(!inSidebar) window.draw(selectionBar);
	
		//board
		for (const auto& col : mBoard) {
			for (const auto& token : col) {
				window.draw(token.getTokenGraphic());
			}
		}

		//scoreboard
		window.draw(scoreBoard);
		for (int i = 0; i < mSettings.mPlayerCount; ++i) {
			sf::Text score(std::to_string(mSettings.mPlayerInfo[i].getWinCount()), font);
			score.setOrigin(score.getLocalBounds().width / 2, score.getLocalBounds().height / 2);
			//vertically aligned with labels
			score.setPosition(UnitSizes::tileSize * (UnitSizes::sideBarWidth + mSettings.mBoardWidth + .5f + i), UnitSizes::tileSize * 1.5f);
			window.draw(score);
		}

		//controls diagram
		if (inSidebar)
			window.draw(sideBarControlVisual);
		else 
			window.draw(gameControlVisual);

		//sidebar
		for (int i = 0; i < sideBarButtonCount; ++i) {
			//sets color to indicate selection
			if (i == sideBarSelection) sideBarBoxes[i].setFillColor(sf::Color(60, 60, 60));
			else sideBarBoxes[i].setFillColor(sf::Color(100, 100, 100));
		}
		for (auto& sideBarBox : sideBarBoxes)
			window.draw(sideBarBox);
		for (auto& sideBarText : sideBarTexts)
			window.draw(sideBarText);

		//win handling
		if (roundWinner != -1) {
			winSound.play();

			mSettings.mPlayerInfo[roundWinner].incWinCount();

			//win message 
			sf::Text winText("Player " + std::to_string(roundWinner + 1) + " wins!\n(Press any key to continue)", font);
			winText.setPosition(UnitSizes::tileSize * (UnitSizes::sideBarWidth + .5f), UnitSizes::tileSize * .5f);
			winText.setOutlineThickness(4.f);
			winText.setOutlineColor(sf::Color::Black);
			window.draw(winText);

			window.display();

			resetBoard();
			roundWinner = -1;

			//pause on win until keypress
			do window.waitEvent(event);
			while (event.type != sf::Event::KeyPressed);
		}
		else window.display();
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
}

void ConnectFour::resetScores(void)
{
	for (auto& player : mSettings.mPlayerInfo) player.resetWinCount();
}
