// NormalPlayGame.cpp

#include "NormalPlayGame.h"

#include <sstream>

#include "DFAUtil.h"

NormalPlayGame::NormalPlayGame(std::string name_in, const dfa_shape_t& shape_in)
  : Game(name_in, shape_in)
{
}

shared_dfa_ptr NormalPlayGame::build_positions_losing(int side_to_move, int ply_max) const
{
  assert(ply_max >= 0);

  if(ply_max % 2)
    {
      // losses are always in an even number of ply
      --ply_max;
    }

  shared_dfa_ptr winning_soon =
    ((ply_max <= 0) ?
     DFAUtil::get_reject(get_shape()) :
     get_positions_winning(1 - side_to_move, ply_max - 1));

  shared_dfa_ptr not_winning_soon = DFAUtil::get_inverse(winning_soon);
  shared_dfa_ptr not_losing_soon = this->get_moves_backward(side_to_move, not_winning_soon);
  shared_dfa_ptr losing_soon = DFAUtil::get_inverse(not_losing_soon);
  return losing_soon;
}

shared_dfa_ptr NormalPlayGame::build_positions_lost(int side_to_move) const
{
  return get_positions_losing(side_to_move, 0);
}

shared_dfa_ptr NormalPlayGame::build_positions_winning(int side_to_move, int ply_max) const
{
  assert(ply_max >= 0);

  if(ply_max <= 0)
    {
      return DFAUtil::get_reject(get_shape());
    }

  if(ply_max % 2 == 0)
    {
      // wins are always in an odd number of ply
      --ply_max;
    }

  shared_dfa_ptr losing_soon = get_positions_losing(1 - side_to_move, ply_max - 1);
  shared_dfa_ptr winning_soon = this->get_moves_backward(side_to_move, losing_soon);
  return winning_soon;
}

std::optional<int> NormalPlayGame::validate_result(int side_to_move, DFAString position) const
{
  std::vector<DFAString> moves = validate_moves(side_to_move, position);

  if(moves.size() == 0)
    {
      return std::optional<int>(-1);
    }

  return std::optional<int>();
}
