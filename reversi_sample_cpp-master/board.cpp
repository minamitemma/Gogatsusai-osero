#include <array>
#include <cassert>
#include <ostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "board.hpp"

namespace reversi
{

Side getOpponentSide(const Side side)
{
	switch (side) {
	case Side::BLACK:
		return Side::WHITE;
	case Side::WHITE:
		return Side::BLACK;
	default:
		assert(false);
		return side;
	}
}

std::ostream& operator<<(std::ostream& os, const Side side)
{
	switch (side) {
	case Side::BLACK:
		os << "black";
		break;
	case Side::WHITE:
		os << "white";
		break;
	default:
		assert(false);
	}
	return os;
}

CellState getOwnState(Side side)
{
	switch (side) {
	case Side::BLACK:
		return CellState::BLACK;
	case Side::WHITE:
		return CellState::WHITE;
	default:
		assert(false);
		return CellState::EMPTY;
	}
}

std::ostream& operator<<(std::ostream& os, CellPosition position)
{
	if (Board::boundsCheck(position)) {
		os << static_cast<char>('a' + position.x) << static_cast<char>('1' + position.y);
	} else {
		os << '(' << position.x << ',' << position.y << ')' << std::endl;
	}
	return os;
}

CellPosition parseCellPosition(const std::string& str)
{
	if (str.size() != 2) {
		throw std::invalid_argument{"invalid format \"" + str + "\""};
	}
	char col = str.at(0);
	char row = str.at(1);
	// assume ascii character code
	if (col < 'a' || col > 'h' || row < '1' || row > '8') {
		throw std::invalid_argument{"invalid format \"" + str + "\""};
	}
	int x = col - 'a';
	int y = row - '1';
	return {x, y};
}

namespace
{

constexpr std::array<std::tuple<int, int>, 8> DIRECTIONS = {{
    std::make_tuple(-1, -1),
    std::make_tuple(-1, 0),
    std::make_tuple(-1, 1),
    std::make_tuple(0, -1),
    std::make_tuple(0, 1),
    std::make_tuple(1, -1),
    std::make_tuple(1, 0),
    std::make_tuple(1, 1),
}};

int countBoundedDisks(const Board& board, CellPosition position, const Side side, const std::tuple<int, int> direction)
{
	assert(board.get(position) == CellState::EMPTY);
	int num_bounded_disks = 0;
	while (true) {
		position.x += std::get<0>(direction);
		position.y += std::get<1>(direction);
		if (!Board::boundsCheck(position)) {
			return 0;
		}
		if (board.get(position) == CellState::EMPTY) {
			return 0;
		}
		if (board.get(position) == getOwnState(side)) {
			break;
		}
		assert(board.get(position) == getOwnState(getOpponentSide(side)));
		++num_bounded_disks;
	}
	return num_bounded_disks;
}

bool canFlip(const Board& board, const CellPosition position, const Side side, const std::tuple<int, int> direction)
{
	return countBoundedDisks(board, position, side, direction) > 0;
}

}  // namespace

Board::Board() : m_states()
{
	set({3, 3}, CellState::WHITE);
	set({3, 4}, CellState::BLACK);
	set({4, 3}, CellState::BLACK);
	set({4, 4}, CellState::WHITE);
}

CellState Board::get(const CellPosition position) const
{
	assert(boundsCheck(position));
	return m_states[static_cast<std::size_t>(position.y)][static_cast<std::size_t>(position.x)];
}

void Board::set(const CellPosition position, const CellState state)
{
	assert(boundsCheck(position));
	m_states[static_cast<std::size_t>(position.y)][static_cast<std::size_t>(position.x)] = state;
}

bool Board::isLegalMove(const CellPosition position, const Side side) const
{
	if (!boundsCheck(position)) {
		return false;
	}
	if (get(position) != CellState::EMPTY) {
		return false;
	}
	for (auto&& direction : DIRECTIONS) {
		if (canFlip(*this, position, side, direction)) {
			return true;
		}
	}
	return false;
}

std::vector<CellPosition> Board::getAllLegalMoves(const Side side) const
{
	std::vector<CellPosition> legal_moves;
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			if (isLegalMove({x, y}, side)) {
				legal_moves.push_back({x, y});
			}
		}
	}
	return legal_moves;
}

void Board::placeDisk(const CellPosition position, const Side side)
{
	assert(isLegalMove(position, side));
	for (auto&& direction : DIRECTIONS) {
		int num_bounded_disks = countBoundedDisks(*this, position, side, direction);
		int flip_x = position.x;
		int flip_y = position.y;
		for (int i = 0; i < num_bounded_disks; ++i) {
			flip_x += std::get<0>(direction);
			flip_y += std::get<1>(direction);
			set({flip_x, flip_y}, getOwnState(side));
		}
	}
	set(position, getOwnState(side));
}

int Board::count(const CellState target) const
{
	int count = 0;
	for (auto&& row : m_states) {
		for (auto&& disk : row) {
			if (disk == target) {
				++count;
			}
		}
	}
	return count;
}

bool Board::boundsCheck(CellPosition position)
{
	if (0 <= position.y && position.y < HEIGHT) {
		if (0 <= position.x && position.x < WIDTH) {
			return true;
		}
	}
	return false;
}

std::ostream& operator<<(std::ostream& os, const Board& board)
{
	os << "  a b c d e f g h";
	for (int y = 0; y < Board::HEIGHT; ++y) {
		os << '\n'
		   << (y + 1);
		for (int x = 0; x < Board::WIDTH; ++x) {
			os << ' ';
			CellState state = board.get({x, y});
			switch (state) {
			case CellState::EMPTY:
				os << '-';
				break;
			case CellState::BLACK:
				os << 'B';
				break;
			case CellState::WHITE:
				os << 'W';
				break;
			}
		}
	}
	return os;
}

}  // namespace reversi
