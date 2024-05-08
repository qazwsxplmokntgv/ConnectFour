#include "ConnectFour.hpp"

ConnectFour::ConnectFour(Settings& settings)
{
	this->mBoardHeight = settings.mBoardHeight;
	this->mBoardWidth = settings.mBoardWidth;
	this->mWinRequirement = settings.mWinRequirement;
	this->mPlayers = &settings.mPlayerInfo;


	this->mBoard.reserve(mBoardWidth);
	for (int i = 0; i < mBoardWidth; ++i) {
		mBoard.push_back(std::vector<Token>());
		mBoard.back().reserve(mBoardHeight);
	}
}

void ConnectFour::runGame(void)
{
	sf::RenderWindow window(
		sf::VideoMode((unsigned int)tokenProp::radius * 2 * (mBoardWidth + (unsigned int)mPlayers->size()), (unsigned int)tokenProp::radius * 2 * mBoardHeight),
		"Connect Four"
	);
	
	//minimizes white window flash while below assets are being loaded by setting to black immediately
	window.clear(sf::Color::Black);

	int selectedCol = 0;
	int currPlayer = 0;
	int roundWinner = -1;
	
	//for tracking game progress
	int moveCount = 0;
	int maxMoves = mBoardHeight * mBoardWidth;

	//load selection bar and set initial color (based on first player)
	auto selectionBar = sf::RectangleShape(sf::Vector2f(tokenProp::radius * 2, (float)window.getSize().y));
	selectionBar.setFillColor((*mPlayers)[0].getToken().getColor());

	//load sound effects
	sf::SoundBuffer drop;
	sf::SoundBuffer win;
	//drop.loadFromMemory(); //TODO: find a good drop sound
	win.loadFromMemory(&Sounds::__300105_SYMBOL_WIN_REVEAL_01_Sounds_of_China_wav, Sounds::__300105_SYMBOL_WIN_REVEAL_01_Sounds_of_China_wav_len);
	auto dropSound = sf::Sound(drop);
	auto winSound = sf::Sound(win);

	//load font
	sf::Font font;
	font.loadFromMemory(&Fonts::PixelifySans_ttf, Fonts::PixelifySans_ttf_len);

	//load scoreboard header
	auto scoreBoard = sf::Text("Scoreboard", font);
	scoreBoard.setOrigin(scoreBoard.getLocalBounds().width / 2.f, scoreBoard.getLocalBounds().height / 2.f);
	scoreBoard.setPosition(2 * tokenProp::radius * (mBoardWidth + (mPlayers->size() / 2.f)), tokenProp::radius);

	//generate labels for each player's section of the scoreboard
	std::vector<sf::Text> playerScoreLabels;
	playerScoreLabels.reserve(mPlayers->size());
	for (int i = 0; i < mPlayers->size(); ++i) {
		auto label = sf::Text("P" + std::to_string(i + 1), font);
		label.setFillColor((*mPlayers)[i].getToken().getColor());
		label.setOrigin(label.getLocalBounds().width / 2, label.getLocalBounds().height / 2);
		label.setPosition(((2 * (mBoardWidth + i)) + 1) * tokenProp::radius, 2 * tokenProp::radius);
		
		playerScoreLabels.push_back(label);
	}

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
					//scroll selected column left
					if (--selectedCol < 0) selectedCol = mBoardWidth - 1;
					selectionBar.setPosition(selectedCol * tokenProp::radius * 2, 0);
					break;

				case sf::Keyboard::Scan::Right:
				case sf::Keyboard::Scan::D:
					//scroll selected column right
					if (++selectedCol >= mBoardWidth) selectedCol = 0;
					selectionBar.setPosition(selectedCol * tokenProp::radius * 2, 0);
					break;

				case sf::Keyboard::Scan::Down:
				case sf::Keyboard::Scan::S:
				case sf::Keyboard::Scan::Space:
				case sf::Keyboard::Scan::Enter:
					//make move
					if (makeMove(currPlayer, selectedCol)) {
						//win checking
						if (checkForWin(selectedCol, (int)mBoard[selectedCol].size() - 1)) {
							roundWinner = currPlayer;
							winSound.play();
						}
						else {
							dropSound.play();

							//checks for draws, resets automatically if the board is filled
							if (++moveCount >= maxMoves) resetBoard();
						}

						//switches to the next player's turn
						if (++currPlayer >= mPlayers->size()) currPlayer = 0;
						//recolors the selection bar appropriately (based on the new current player)
						selectionBar.setFillColor((*mPlayers)[currPlayer].getToken().getColor());
					}
					break;
				}
				break;

			default:
				break;
			}
		}

		//rendering
		window.clear(sf::Color::Black);

		window.draw(selectionBar);
		

		//draws board
		for (const auto& col : mBoard) {
			for (const auto& token : col) {
				window.draw(token.getTokenGraphic());
			}
		}
		
		//if a player wins, 
		if (roundWinner != -1) {

			(*mPlayers)[roundWinner].incWinCount();

			//win message 
			//CURRENTLY DOES NOT PAUSE--effectively skips this message being shown for the time being
			auto winText = sf::Text("Player " + std::to_string(roundWinner + 1) + " wins!\n(Press any key to continue)", font);
			winText.setOutlineThickness(4.f);
			winText.setOutlineColor(sf::Color::Black);
			window.draw(winText);

			resetBoard();
			roundWinner = -1;
		}

		//draw scoreboard
		//main header
		window.draw(scoreBoard); 
		//player headers
		for (const auto& label : playerScoreLabels) 
			window.draw(label);
		//player scores
		for (auto& player : *mPlayers) {
			auto score = sf::Text(std::to_string(player.getWinCount()), font);
			score.setOrigin(score.getLocalBounds().width / 2, score.getLocalBounds().height / 2);
			score.setPosition(tokenProp::radius * (1.f + (2.f * (mBoardWidth + player.getToken().getID()))), tokenProp::radius * 3.f);
			window.draw(score);
		}

		window.display();
	}
}

bool ConnectFour::makeMove(int player, int column)
{
	//disallows placing tokens above the board
	if (mBoard[column].size() >= mBoardHeight)
		//indicates no move was successfully made
		return false;

	//adds a token to the appropriate column
	mBoard[column].push_back((*mPlayers)[player].getToken());
	
	//sets position for the token to be rendered in
	mBoard[column].back().setPosition(
		(2 * tokenProp::radius) * (float)column, //x
		(2 * tokenProp::radius) * (mBoardHeight - (float)mBoard[column].size()) //y
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
	if (mBoard[lastX].size() >= mWinRequirement) {
		//scans down thru this column from the toMatch token
		for (int i = 1; i <= mWinRequirement; ) {
			//continue iff the sequence stays unbroken
			if (mBoard[lastX][(size_t)lastY - i] != toMatch) break;
			//if a vertical sequence was found, report win
			if (++i >= mWinRequirement) return true;
		}
	}

	//counts corresponding to sequences(branches) reaching/passing thru this point, starting from the bottom left, left, and top left respectively
	//starts with 1 on account of the inclusion of the toMatch token towards a full sequence
	int countsOfBranches[3] = { 1, 1, 1 };
	
	//checks left side of this column
	bool continueCheckingOnLeft[3] = { true, true, true };
	for (int i = 1; i < mWinRequirement; ++i) {

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
	for (int count : countsOfBranches) if (count >= mWinRequirement) return true;

	//top right, right, bottom right (opposite corresponding left branches)
	bool continueCheckingOnRight[3] = { true, true, true };
	for (int i = 1; i < mWinRequirement; ++i) {

		//if there exists columns to the right && if those are no more than i tokens shorter than this column
		if (lastX + i < mBoardWidth && (int)mBoard[(size_t)lastX + i].size() > lastY - i) {
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
			//only checks if at least one branch continues to check
			for (int count : countsOfBranches) if (count >= mWinRequirement) return true;
		}
	}
	//no sufficient sequence was found
	return false;
}

void ConnectFour::resetBoard(void)
{
	//clears each column
	for (auto& col : mBoard) col.clear();
}
