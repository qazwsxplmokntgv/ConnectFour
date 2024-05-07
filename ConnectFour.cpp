#include "ConnectFour.hpp"

ConnectFour::ConnectFour(const Settings& settings)
{
	this->mBoardHeight = settings.mBoardHeight;
	this->mBoardWidth = settings.mBoardWidth;
	this->mWinRequirement = settings.mWinRequirement;
	this->mPlayers = settings.mPlayerInfo;
	this->mBoard.reserve(mBoardWidth);
	for (int i = 0; i < mBoardWidth; ++i) {
		mBoard.push_back(std::vector<Token>());
		mBoard.back().reserve(mBoardHeight);
	}
}

void ConnectFour::runGame(void)
{
	sf::RenderWindow window(
		sf::VideoMode((unsigned int)tokenProp::radius * 2 * (mBoardWidth + (unsigned int)mPlayers.size()), (unsigned int)tokenProp::radius * 2 * (mBoardHeight + 1)),
		"Connect Four"
	);
	
	//minimizes white window flash while below assets are being loaded
	window.clear(sf::Color::Black);

	int selectedCol = 0;
	int currPlayer = 0;
	int roundWinner = -1;

	auto selectionBar = sf::RectangleShape(sf::Vector2f(tokenProp::radius * 2, (float)window.getSize().y));
	selectionBar.setFillColor(mPlayers[0].getToken().getColor());

	sf::Font font;
	font.loadFromFile("PixelifySans.ttf");

	//scoreboard header
	auto scoreBoard = sf::Text("Scoreboard", font);
	scoreBoard.setOrigin(scoreBoard.getLocalBounds().width / 2, scoreBoard.getLocalBounds().height / 2);
	scoreBoard.setPosition(2 * tokenProp::radius * (mBoardWidth + (mPlayers.size() / 2.f)), tokenProp::radius);

	//labels for each player's section of the scoreboard
	std::vector<sf::Text> playerScoreLabels;
	playerScoreLabels.reserve(mPlayers.size());
	for (int i = 0; i < mPlayers.size(); ++i) {
		auto label = sf::Text("P" + std::to_string(i + 1), font);
		label.setFillColor(mPlayers[i].getToken().getColor());
		label.setOrigin(label.getLocalBounds().width / 2, label.getLocalBounds().height / 2);
		label.setPosition(((2 * (mBoardWidth + i)) + 1) * tokenProp::radius, 2 * tokenProp::radius);
		
		playerScoreLabels.push_back(label);
	}

	//sound
	sf::SoundBuffer drop;
	sf::SoundBuffer win;
	drop.loadFromFile(""); //TODO: find a good drop sound
	win.loadFromFile("300105-SYMBOL-WIN-REVEAL-01-Sounds-of-China.wav");
	auto dropSound = sf::Sound(drop);
	auto winSound = sf::Sound(win);

	//game loop
	while (window.isOpen()) {

		//event handling
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {

			case sf::Event::Closed:
				window.close();
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
						if (checkForWin(selectedCol, (int)mBoard[selectedCol].size() - 1)) {
							roundWinner = currPlayer;
							winSound.play();
						}
						else dropSound.play();

						if (++currPlayer >= mPlayers.size()) currPlayer = 0;
						selectionBar.setFillColor(mPlayers[currPlayer].getToken().getColor());
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
		window.draw(scoreBoard);
		
		for (auto& label : playerScoreLabels) 
			window.draw(label);

		//draws each row
		for (int row = mBoardHeight - 1; row >= 0; --row) {
			//draws each token in a row
			for (int col = 0; col < mBoardWidth; ++col) {
				//if this row contains a token in this col
				if (mBoard[col].size() > row) {
					window.draw(mBoard[col][row].getTokenGraphic());
				}
			}
		}

		//if a player wins, 
		if (roundWinner != -1) {

			mPlayers[roundWinner].incWinCount();

			//win message
			auto winText = sf::Text("Player " + std::to_string(roundWinner + 1) + " wins!\n(Press any key to continue)", font);
			winText.setOutlineThickness(4.f);
			winText.setOutlineColor(sf::Color::Black);
			window.draw(winText);


			//reset board
			roundWinner = -1;
			for (auto& col : mBoard) col.clear();
		}

		//draw each player's win count
		for (auto& player : mPlayers) {
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
	mBoard[column].push_back(mPlayers[player].getToken());
	
	mBoard[column].back().setPosition(
		(2 * tokenProp::radius) * (float)column, //x
		(2 * tokenProp::radius) * (mBoardHeight - ((float)mBoard[column].size() - 1)) //y
	);

	return true;
}

bool ConnectFour::checkForWin(int lastX, int lastY) const
{
	int toMatch = mBoard[lastX][lastY].getID();

	//if there are enough tokens in just this column for a potential win
	if (mBoard[lastX].size() >= mWinRequirement) {
		//scans down thru this column
		for (int i = 1; i <= mWinRequirement; ) {
			//continue if the sequence stays unbroken
			if (mBoard[lastX][(size_t)lastY - i].getID() != toMatch) break;
			//if a vertical sequence was found, report win
			if (++i >= mWinRequirement) return true;
		}
	}

	//counts corresponding to sequences(branches) reaching/passing thru this point, starting from the bottom left, left, and top left respectively
	//starts with 1 due to the inclusion of the parameter token
	int countsOfBranches[3] = { 1, 1, 1 };
	
	//checks left side of this column
	bool continueCheckingOnLeft[3] = { true, true, true };
	for (int i = 1; i < mWinRequirement; ++i) {

		//if there exists columns to the left no more than i tokens shorter than this column
		if (lastX - i >= 0 && (int)mBoard[(size_t)lastX - i].size() > lastY - i) {
			if (continueCheckingOnLeft[0] && lastY - i >= 0 && mBoard[(size_t)lastX - i][(size_t)lastY - i].getID() == toMatch) ++countsOfBranches[0];
			else continueCheckingOnLeft[0] = false;

			//if there exists columns to the left at least the height of this column
			if ((int)mBoard[(size_t)lastX - i].size() > lastY) {
				if (continueCheckingOnLeft[1] && mBoard[(size_t)lastX - i][lastY].getID() == toMatch) ++countsOfBranches[1];
				else continueCheckingOnLeft[1] = false;

				//if there exists columns to the left at least i tokens taller than this column
				if ((int)mBoard[(size_t)lastX - i].size() > lastY + i) {
					if (continueCheckingOnLeft[2] && mBoard[(size_t)lastX - i][(size_t)lastY + i].getID() == toMatch) ++countsOfBranches[2];
					else continueCheckingOnLeft[2] = false;
				}
			}
		}
	}
	//checks if a full sequence was foud on the left
	for (int count : countsOfBranches) if (count >= mWinRequirement) return true;

	bool continueCheckingOnRight[3] = { true, true, true };
	for (int i = 1; i < mWinRequirement; ++i) {

		//if there exists columns to the right && if those are no more than i tokens shorter than this column
		if (lastX + i < mBoardWidth && (int)mBoard[(size_t)lastX + i].size() > lastY - i) {
			if (continueCheckingOnRight[2] && lastY - i >= 0 && mBoard[(size_t)lastX + i][(size_t)lastY - i].getID() == toMatch) ++countsOfBranches[2];
			else continueCheckingOnRight[2] = false;

			//if there exists columns to the right at least the height of this column
			if ((int)mBoard[(size_t)lastX + i].size() > lastY) {
				if (continueCheckingOnRight[1] && mBoard[(size_t)lastX + i][lastY].getID() == toMatch) ++countsOfBranches[1];
				else continueCheckingOnRight[1] = false;

				//if there exists columns to the right at least i tokens taller than this column
				if ((int)mBoard[(size_t)lastX + i].size() > lastY + i) {
					if (continueCheckingOnRight[0] && mBoard[(size_t)lastX + i][(size_t)lastY + i].getID() == toMatch) ++countsOfBranches[0];
					else continueCheckingOnRight[0] = false;
				}
			}
			//checks if a full sequence has been found yet between the sum of the corresponding left and right branches
			for (int count : countsOfBranches) if (count >= mWinRequirement) return true;
		}
	}
	return false;
}
