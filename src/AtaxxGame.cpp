// Ataxx Beginning Implementation

// UP TO THIS POINT 6/12/25: have implemented basic skeleton using orthello as a guide, have indicated
// where there is still code needed to be implemented, will continue to work on it

#include "AtaxxGame.h"

#include "DFAUtil.h"
#include "GameUtil.h"

#include <sstream>
#include <stdexcept>
#include <cassert>
#include <vector>

#define ASSERT_SIDE_TO_MOVE(side) assert((0 <= side) && (side <= 1))
#define CALCULATE_LAYER(x, y) ((x) + (this->width) * (y))

static std::vector<std::pair<int, int>> directions({{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}});

AtaxxGame::AtaxxGame(int width_in, int height_in)
    : Game("ataxx_" + std::to_string(width_in) + "x" + std::to_string(height_in),
           dfa_shape_t(size_t(width_in * height_in), 3)),
      width(width_in),
      height(height_in)
// should there be any board constraints to consider?
// orthello has some, but idk if Ataxx really needs it
{
}

// something has to be wrong with the build_move_graph
// best step now is to go through it function by function and see what exactly might be causing
// an issue
MoveGraph AtaxxGame::build_move_graph(int side_to_move) const
{
  MoveGraph move_graph(get_shape());

  move_graph.add_node("begin");
  move_graph.add_node("begin+1");
  move_graph.add_edge("begin legal", "begin", "begin+1"); //, get_positions_legal());
  // move_graph.add_edge("begin legal", "begin", "begin+1", DFAUtil::get_accept(get_shape()));

  std::vector<std::vector<std::string>> to_names(width * height);

  // for each piece used as a source
  for (int x_src = 0; x_src < width; ++x_src)
  {
    for (int y_src = 0; y_src < height; ++y_src)
    {

      // check 5x5 region around source (can move 2 steps, so 25 minus source pos)
      for (int dx = -2; dx <= 2; ++dx)
      {
        for (int dy = -2; dy <= 2; ++dy)
        {
          if (dx == 0 && dy == 0)
            continue;

          int x_dst = x_src + dx;
          int y_dst = y_src + dy;
          // check to see if dest is on the board
          if (x_dst < 0 || x_dst >= width || y_dst < 0 || y_dst >= height)
            continue;
          std::string move_name = get_name_move(x_src, y_src, x_dst, y_dst);

          change_vector changes(size_t(width * height));

          int src_layer = CALCULATE_LAYER(x_src, y_src);
          int dst_layer = CALCULATE_LAYER(x_dst, y_dst);

          // clone: destination adjacent, keep og
          if (std::abs(dx) <= 1 && std::abs(dy) <= 1)
          {
            changes[size_t(src_layer)] = change_type(1 + side_to_move, 1 + side_to_move);
          }
          else
          {
            // jump: destination 2 squares away, delete og
            changes[size_t(src_layer)] = change_type(1 + side_to_move, 0);
          }

          // place a "new" piece at destination every time (since we decide to delete or not delete og)
          changes[size_t(dst_layer)] = change_type(0, 1 + side_to_move);

          move_graph.add_node(move_name, changes); //, pre_conditions, post_conditions);
          move_graph.add_edge("begin+1", move_name);
          to_names[dst_layer].push_back(move_name);
#if 0
          std::string prev_name = move_name;
          // flip adjacent opponent pieces around destination (all 8 directions)
          for (int dx_adj = -1; dx_adj <= 1; ++dx_adj) {
            for (int dy_adj = -1; dy_adj <= 1; ++dy_adj) {
              if (dx_adj == 0 && dy_adj == 0) continue;
              int x_adj = x_dst + dx_adj;
              int y_adj = y_dst + dy_adj;

              if (x_adj < 0 || x_adj >= width || y_adj < 0 || y_adj >= height)
                continue;

              

              int adj_layer = CALCULATE_LAYER(x_adj, y_adj);
              changes[size_t(adj_layer)] = change_type(2 - side_to_move, 1 + side_to_move); // enemy to us
            }
          }
#endif

#if 0
          // check precondictions aka source must be ours, destination must be empty
          shared_dfa_ptr pre_src = DFAUtil::get_fixed(get_shape(), src_layer, 1 + side_to_move);
          shared_dfa_ptr pre_dst = DFAUtil::get_fixed(get_shape(), dst_layer, 0);
          shared_dfa_ptr pre_condition = DFAUtil::get_intersection(pre_src, pre_dst);

          // add the move node and add an edge
          move_edge_condition_vector pre_conditions = {pre_condition};
          move_edge_condition_vector post_conditions = {};
          // ask about the empty post_conditions if this will be a problem, kept
          // giving an error because add_node needed expects 4 arguments
#endif
        }
      }
    }
  }

  std::vector<std::string> flipped_names;
  for (int layer = 0; layer < width * height; ++layer)
  {
    std::string to_name = "to=" + std::to_string(layer);
    move_graph.add_node(to_name);
    for (auto move_name : to_names[layer])
    {
      move_graph.add_edge(move_name, to_name);
    }

    int x_dst = layer % width;
    int y_dst = layer / width;
    assert(CALCULATE_LAYER(x_dst, y_dst) == layer);

    std::string prev_name = to_name;
    // flip adjacent opponent pieces around destination (all 8 directions)
    for (int dx_adj = -1; dx_adj <= 1; ++dx_adj)
    {
      for (int dy_adj = -1; dy_adj <= 1; ++dy_adj)
      {
        if (dx_adj == 0 && dy_adj == 0)
          continue;
        int x_adj = x_dst + dx_adj;
        int y_adj = y_dst + dy_adj;

        if (x_adj < 0 || x_adj >= width || y_adj < 0 || y_adj >= height)
          continue;

        int adj_layer = CALCULATE_LAYER(x_adj, y_adj);
        change_vector flip(size_t(width * height));

        flip[size_t(adj_layer)] = change_type(2 - side_to_move, 1 + side_to_move); // enemy to us
        std::string flip_name = "to=" + std::to_string(layer) + ",flip=" + std::to_string(adj_layer) + ",true";
        move_graph.add_node(flip_name, flip);
        move_graph.add_edge(flip_name, prev_name, flip_name);

        shared_dfa_ptr not_flip_condition = DFAUtil::get_inverse(DFAUtil::get_fixed(get_shape(), adj_layer, 2 - side_to_move));
        std::string not_flip_name = "to=" + std::to_string(layer) + ",flip=" + std::to_string(adj_layer) + ",false";
        move_graph.add_node(not_flip_name);
        move_graph.add_edge(not_flip_name, prev_name, not_flip_name, not_flip_condition);

        std::string flipped = "to=" + std::to_string(layer) + ",flip=" + std::to_string(adj_layer) + ",done";
        move_graph.add_node(flipped);
        move_graph.add_edge(flip_name + " to " + flipped, flip_name, flipped);
        move_graph.add_edge(not_flip_name + " to " + flipped, not_flip_name, flipped);

        prev_name = flipped;
      }
    }
    flipped_names.push_back(prev_name);
  }

  move_graph.add_node("end+1");
  for (auto flipped_name : flipped_names)
  {
    move_graph.add_edge(flipped_name, "end+1");
  }

  move_graph.add_node("end");
  move_graph.add_edge("end legal", "end+1", "end", get_positions_legal());
  // move_graph.add_edge("end legal", "end+1", "end", DFAUtil::get_accept(get_shape()));

  // pass edge if we have no moves but opponent does (taken from othello code)
  shared_dfa_ptr pass_condition = DFAUtil::get_difference(
      get_positions_can_place(1 - side_to_move),
      get_positions_can_place(side_to_move));

  move_graph.add_edge("pass", "begin+1", "end+1", pass_condition);

  return move_graph;
}

std::string AtaxxGame::get_name_move(int x1, int y1, int x2, int y2) const
{
  return "move from x=" + std::to_string(x1) + ",y=" + std::to_string(y1) + " to x=" + std::to_string(x2) + ",y=" + std::to_string(y2);
  // maybe a better way to name uniquely but this makes most sense to me for now (same as othello code)
}

DFAString AtaxxGame::get_position_initial() const
{
  std::vector<int> state(size_t(width * height), 0);
  // standard Ataxx game: a piece in every corner, same player should have both pieces opposite of one another
  state[CALCULATE_LAYER(0, 0)] = 1;                  // black
  state[CALCULATE_LAYER(width - 1, height - 1)] = 1; // black
  state[CALCULATE_LAYER(width - 1, 0)] = 2;          // white
  state[CALCULATE_LAYER(0, height - 1)] = 2;         // white

  return DFAString(get_shape(), state);
}

std::string AtaxxGame::position_to_string(const DFAString &string_in) const
{
  std::ostringstream output;
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      auto layer = CALCULATE_LAYER(x, y);
      switch (string_in[layer])
      {
      case 0:
        output << '.';
        break;
      case 1:
        output << 'b';
        break;
      case 2:
        output << 'w';
        break;
      }
    }
    output << "\n";
  }
  return output.str();
}
// check all positions a player can do to see if the player can make any move
shared_dfa_ptr AtaxxGame::get_positions_can_place(int side_to_move) const
{
  return load_or_build("can_place_" + std::to_string(side_to_move), [&]()
                       {
    std::vector<shared_dfa_ptr> moves;
    for (int x = 0; x < width; ++x) {
      for (int y = 0; y < height; ++y) {
        moves.push_back(get_positions_can_place(side_to_move, x, y));
      }
    }
    return DFAUtil::get_union_vector(get_shape(), moves); });
}
// returns all valid board setups
// does this make all states without a piece in the 4 corners invalid?
// ask about this logic because the constraints may be too much here with
// requiring all four corners to be ocupied with a piece

shared_dfa_ptr AtaxxGame::get_positions_legal() const
{
  return load_or_build("legal", [&]()
                       {
    // corners
    //std::vector<int> required_positions = {
    //  CALCULATE_LAYER(0, 0),
    //  CALCULATE_LAYER(width - 1, height - 1),
    //  CALCULATE_LAYER(0, height - 1),
    //  CALCULATE_LAYER(width - 1, 0)
    //};

    //std::vector<shared_dfa_ptr> constraints;
    //for (int pos : required_positions) {
    //  constraints.push_back(DFAUtil::get_inverse(DFAUtil::get_fixed(get_shape(), pos, 0)));
    //}

    //return DFAUtil::get_intersection_vector(get_shape(), constraints);

    // instead of constraining corners or pieces, constrain # of pieces 
    // since each player needs at min 1 piece to be playing
    // shared_dfa_ptr has_black = DFAUtil::get_count_character(get_shape(), 1, 1);
    // shared_dfa_ptr has_white = DFAUtil::get_count_character(get_shape(), 2, 1);

    // return DFAUtil::get_intersection_vector(get_shape(), {has_black, has_white});

    return DFAUtil::get_accept(get_shape()); });
}

// check specific piece if it can move
shared_dfa_ptr AtaxxGame::get_positions_can_place(int side, int x_src, int y_src) const
{
  return load_or_build("can_place_" + std::to_string(side) + "_at_" + std::to_string(x_src) + "," + std::to_string(y_src), [&]()
                       {
    std::vector<shared_dfa_ptr> choices;

    for (int dx = -2; dx <= 2; ++dx) {
      for (int dy = -2; dy <= 2; ++dy) {
        if (dx == 0 && dy == 0) continue;
        int x_dst = x_src + dx;
        int y_dst = y_src + dy;

        if (x_dst < 0 || x_dst >= width || y_dst < 0 || y_dst >= height)
          continue;

        auto src_layer = CALCULATE_LAYER(x_src, y_src);
        auto dst_layer = CALCULATE_LAYER(x_dst, y_dst);

        shared_dfa_ptr pre_src = DFAUtil::get_fixed(get_shape(), src_layer, 1 + side);
        shared_dfa_ptr pre_dst = DFAUtil::get_fixed(get_shape(), dst_layer, 0);

        choices.push_back(DFAUtil::get_intersection(pre_src, pre_dst));
      }
    }

    return DFAUtil::get_union_vector(get_shape(), choices); });
}

shared_dfa_ptr AtaxxGame::get_positions_end() const
{
  return load_or_build("end", [&]()
                       {
    // game ends when all cells filled or one player has no pieces
    shared_dfa_ptr no_blanks = DFAUtil::get_count_character(get_shape(), 0, 0);
    shared_dfa_ptr black_dead = DFAUtil::get_count_character(get_shape(), 1, 0);
    shared_dfa_ptr white_dead = DFAUtil::get_count_character(get_shape(), 2, 0);
    return DFAUtil::get_union_vector(get_shape(), {no_blanks, black_dead, white_dead}); });
}
#if 0
shared_dfa_ptr AtaxxGame::build_positions_won(int side) const
{
  return load_or_build("won_" + std::to_string(side), [&]()
                       {
    shared_dfa_ptr end = get_positions_end();

    shared_dfa_ptr mine = DFAUtil::get_count_character(get_shape(), 1 + side, 1, width * height);
    shared_dfa_ptr opp = DFAUtil::get_count_character(get_shape(), 2 - side, 0, width * height - 1);

    return DFAUtil::get_intersection(end, DFAUtil::get_intersection(mine, opp)); });
}
#endif

shared_dfa_ptr AtaxxGame::build_positions_won(int side) const
{
  int opp = 1 - side;
  shared_dfa_ptr end_positions = get_positions_end();

  std::vector<shared_dfa_ptr> winning_cases;
  for (int my_count = 1; my_count <= width * height; ++my_count)
  {
    shared_dfa_ptr mine = DFAUtil::get_count_character(get_shape(), 1 + side, my_count);
    shared_dfa_ptr opp_constraint = DFAUtil::get_count_character(get_shape(), 1 + opp, 0, my_count - 1);
    winning_cases.push_back(DFAUtil::get_intersection(mine, opp_constraint));
  }

  shared_dfa_ptr win_positions = DFAUtil::get_union_vector(get_shape(), winning_cases);
  return DFAUtil::get_intersection(end_positions, win_positions);
}

// games won - games lost
shared_dfa_ptr AtaxxGame::build_positions_lost(int side_to_move) const
{
  
  return get_positions_won(1 - side_to_move);
}
