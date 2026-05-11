#include <array>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "board.hpp"
#include "option_parser.hpp"
#include "player.hpp"
#include "player/human_player.hpp"
#include "player/sample_computer_player.hpp"

int main(int argc, char* argv[])
{
	using namespace reversi;

	const std::unordered_map<std::string, std::function<std::unique_ptr<Player>(Side)>> player_types = {
	    {"human", [](Side side) { return std::make_unique<HumanPlayer>(side); }},
	    {"sample", [](Side side) { return std::make_unique<SampleComputerPlayer>(side); }},
	};

	auto command_line_params = parseCommandLineOptions(argc, argv, player_types);

	std::array<std::unique_ptr<Player>, 2> players;
	players.at(static_cast<std::size_t>(Side::BLACK)) = std::move(command_line_params.black_player);
	players.at(static_cast<std::size_t>(Side::WHITE)) = std::move(command_line_params.white_player);

	Board board;

	int pass = 0; //相互パスの判定
	
	for (Side turn = Side::BLACK;; turn = getOpponentSide(turn)) {
		std::cout << board << "\n"
		          << std::endl;

		if (board.count(CellState::EMPTY) == 0) {
			// no empty cell
			break;
		}
		auto legal_moves = board.getAllLegalMoves(turn);
		if (legal_moves.empty()) {
		  // pass
		  pass++;
		  if(pass>=2){
		    printf("両者パスしたのでこの試合は終了です。\n");
		    return 0;
		  }
		  std::cout << "turn = " << turn << ", move = Pass\n"
			    << std::endl;
		  continue;
		}

		auto& turn_player = *players.at(static_cast<std::size_t>(turn));

		auto move = turn_player.thinkNextMove(board);
		if (!board.isLegalMove(move, turn)) {
			std::cout << "turn = " << turn << ", illegal move = " << move << std::endl;
			std::exit(1);
		}
		std::cout << "turn = " << turn << ", move = " << move << "\n"
		          << std::endl;

		board.placeDisk(move, turn);
		pass = 0; //ここでpassを0にして2連パス以外を通さないようにする
	}

	 //勝敗の判定
  int count_w = 0;
  int count_b = 0;
  for(int i = 0;i < Board::HEIGHT ;i++){
    for(int j = 0;j < Board::WIDTH ;j++){
      CellState state = board.get({i,j});
      switch(state){
      case CellState::EMPTY:
	break;
      case CellState::BLACK:
	count_b++;
	break;
      case CellState::WHITE:
	count_w++;
	break;
      }
    }
  }
  printf("白が %d 個、黒が %d 個なので",count_w,count_b);
  if(count_b > count_w){
    printf("黒の勝ちです。");
  }else if(count_b < count_w){
    printf("白の勝ちです。");
  }else{
    printf("引き分けです。");
  }
  printf("\n");

	return 0;
}
