// Ataxx Game 

#ifndef ATAXX_GAME_H
#define ATAXX_GAME_H

#include <string>
#include "Game.h"

class AtaxxGame : public Game
{
private:
  int width;
  int height;

public:
  AtaxxGame(int, int);

private:
  virtual MoveGraph build_move_graph(int) const;

  std::string get_name_move(int, int, int, int) const;

  shared_dfa_ptr get_positions_can_place(int) const;
  shared_dfa_ptr get_positions_can_place(int, int, int) const;
  shared_dfa_ptr get_positions_end() const;
  shared_dfa_ptr get_positions_legal() const;
  // shared_dfa_ptr get_positions_can_move(int side, int x0, int y0, int x1, int y1) const;
  // shared_dfa_ptr get_flip_conditions(int, int, int) const;

public:
  virtual shared_dfa_ptr build_positions_lost(int) const;
  virtual shared_dfa_ptr build_positions_won(int) const;
  virtual DFAString get_position_initial() const;
  virtual std::string position_to_string(const DFAString&) const;
// validation
  virtual std::vector<DFAString> validate_moves(int, DFAString) const;
  virtual int validate_result(int, DFAString) const;
};



#endif