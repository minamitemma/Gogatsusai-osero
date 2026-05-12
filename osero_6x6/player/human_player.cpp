#include <iostream>
#include <stdexcept>
#include <string>

#include "human_player.hpp"

namespace reversi
{

CellPosition HumanPlayer::thinkNextMove(const Board& board)
{
	while (true) {
		std::cout << "Where? ";
		std::string position_str;
		std::cin >> position_str;
		if (!std::cin) {
			std::exit(0);
		}
		try {
			auto move = parseCellPosition(position_str);
			if (board.isLegalMove(move, getSide())) {
				return move;
			}
			std::cout << position_str << " is not a legal move!\n"
			          << std::endl;
		} catch (const std::invalid_argument& ex) {
			std::cout << ex.what() << "\n"
			          << std::endl;
			continue;
		}
	}
}

}  // namespace reversi
