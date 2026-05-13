#include <vector>

#include "sample_computer_player.hpp"

namespace reversi
{

CellPosition SampleComputerPlayer::thinkNextMove(const Board& board)
{
	// choose the first legal move
	return board.getAllLegalMoves(getSide()).front();
}

}  // namespace reversi
