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
		try {
		  std::cout << "本当に" << position_str << "でいいですか？(yes/no)\n" << std::endl;
		  std::string l1;
		  std::cin >> l1;
		  std::string l2 = "yes";
		  if(l1 == l2){
			auto move = parseCellPosition(position_str);
			if (board.isLegalMove(move, getSide())) {
				return move;
			}
			std::cout << position_str << " is not a legal move!\n"
			          << std::endl;
		  }else{
		    ;
		  }
		} catch (const std::invalid_argument& ex) {
			std::cout << ex.what() << "\n"
			          << std::endl;
			continue;
		}
	}
}

}  // namespace reversi
