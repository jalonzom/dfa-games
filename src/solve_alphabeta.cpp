#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <limits>
#include <cstdlib>
#include "AtaxxGame.h"
#include "DFAUtil.h"
#include "test_utils.h"
#include <sstream>
#include <iomanip>

std::random_device rand_dev;
std::mt19937 generator(rand_dev());
std::uniform_real_distribution<double> dist(-1.0, 1.0);
// random uniform num between [-1, 1], double check to see if this is how it should be done
// lowkey just googled how to get random numbers in c++ and used one from stack overflow that seemed legit
double random_heuristic()
{
    return dist(generator);
}

static shared_dfa_ptr winning_dfas[2] = {0, 0};
static shared_dfa_ptr losing_dfas[2] = {0, 0};

// apha-beta search
double alphabeta(const AtaxxGame &game, const DFAString &pos, int side, int depth,
                 double alpha, double beta, int &positions_evaluated, int &dfa_hits, int &heuristic_hits)
{

    // see if game is won or not at this point
    positions_evaluated++;
    int result = game.validate_result(side, pos);
    if (result != 0)
    {
        // if there is a forced win, return 100 if forced loss return -100
        return (result == 1) ? 100.0 : (result == -1) ? -100.0
                                                      : 0.0;
    }
    if (winning_dfas[side]->contains(pos))
    {
        // std::cout << "DFA says: WINNING\n";
        dfa_hits++;
        return 100.0;
    }
    if (losing_dfas[side]->contains(pos))
    {
        // std::cout << "DFA says: LOSING\n";
        dfa_hits++;
        return -100.0;
    }
    if (depth == 0)
    {
        heuristic_hits++;
        return random_heuristic();
    }

    std::vector<DFAString> children = game.validate_moves(side, pos);
    if (children.size() == 1 && children[0].to_string() == pos.to_string())
    {
        // no moves
        DFAString after_pass = pos; // same position, switch player
        int width = game.get_width();
        int height = game.get_height();
        int total_squares = width * height;
        // count pieces, remember 1 is black 2 is white 0 is empty
        int my_pieces = 0;
        char my_char = (side == 0) ? 1 : 2;

        for (int i = 0; i < total_squares; ++i)
        {
            if (after_pass[i] == my_char)
                my_pieces++;
        }
        // ask if this is really necessary bc in validate moves if it returns a position,
        // then we know that it has to have more pieces than opponent?
        if (my_pieces * 2 < total_squares)
        {
            // automatic loss
            return -100.0;
        }

        // else continue without decreasing depth
        return -alphabeta(game, after_pass, 1 - side, depth, -beta, -alpha, positions_evaluated, dfa_hits, heuristic_hits);
    }
    
    // had to google infinity too bruh
    // set best possible move to neg infinity so it'll update
    double best = -std::numeric_limits<double>::infinity();
    // for every possible move
    for (const DFAString &child : children)
    {
        // recursively go through children, flip to other side + negate score cuz opps best
        double score = -alphabeta(game, child, 1 - side, depth - 1, -beta, -alpha, positions_evaluated, dfa_hits, heuristic_hits);
        if (score > best)
            best = score;
        if (best > alpha)
            alpha = best;
        if (alpha >= beta)
            break; // PRUNE
    }

    return best;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: ./solve_alphabeta ataxx_WIDTHxHEIGHT [DEPTH] [DB]" << std::endl;
        return 1;
    }

    std::string game_name(argv[1]);
    int width, height;
    if (sscanf(game_name.c_str(), "ataxx_%dx%d", &width, &height) != 2)
    {
        std::cerr << "Invalid game name format. Use ataxx_WIDTHxHEIGHT" << std::endl;
        return 1;
    }

    int max_depth = (argc >= 3) ? std::atoi(argv[2]) : 4;
    int db_ply = (argc >= 4) ? std::atoi(argv[3]) : 0;

    AtaxxGame game(width, height);

    for (int side = 0; side < 2; ++side)
    {
        winning_dfas[side] = game.get_positions_winning(side, db_ply);
        losing_dfas[side] = game.get_positions_losing(side, db_ply);
    }

    DFAString initial = game.get_position_initial();
    int positions_evaluated = 0;
    int dfa_hits = 0;
    int heuristic_hits = 0;

    std::cout << "checking for P1 (black) let's hope you finally work..." << std::endl;
    std::cout << game.position_to_string(initial) << std::endl;

    double score = alphabeta(game, initial, 0, max_depth, -1000, 1000, positions_evaluated, dfa_hits, heuristic_hits);

    std::cout << "AlphaBeta result: " << score << "\n";
    std::cout << "Positions evaluated: " << positions_evaluated << "\n";
    std::cout << "Nodes found in DFA: " << dfa_hits << "\n";
    std::cout << "Nodes found using heuristic: " << heuristic_hits << "\n";

    return 0;
}
