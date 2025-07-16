#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <limits>
#include <cstdlib>
#include "AtaxxGame.h"

    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_real_distribution<double> dist(-1.0, 1.0);
// random uniform num between [-1, 1], double check to see if this is how it should be done
// lowkey just googled how to get random numbers in c++ and used one from stack overflow that seemed legit
double random_heuristic()
{
    return dist(generator);
}

// apha-beta search
// 0 = black (maximizing), 1 = white (minimizing)
double alphabeta(const AtaxxGame &game, const DFAString &pos, int side, int depth,
                 double alpha, double beta, int &positions_evaluated)
{
    // see if game is won or not at this point
    int result = game.validate_result(side, pos);
    if (result != 0 || depth == 0)
    {
        positions_evaluated++;
        // if there is a forced win, return 100 if forced loss return -100 else use random
        return (result == 1) ? 100.0 : (result == -1) ? -100.0
                                                      : random_heuristic();
    }

    std::vector<DFAString> children = game.validate_moves(side, pos);
    if (children.empty())
    {
        // no moves
        DFAString after_pass = pos; // same position, switch player
        return -alphabeta(game, after_pass, 1 - side, depth - 1, -beta, -alpha, positions_evaluated);
    }
    // had to google infinity too bruh
    // set best possible move to neg infinity so it'll update
    double best = -std::numeric_limits<double>::infinity();
    // for every possible move
    for (const DFAString &child : children)
    {
        // recursively go through children, flip to other side + negate score cuz opps best
        double score = -alphabeta(game, child, 1 - side, depth - 1, -beta, -alpha, positions_evaluated);
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
        std::cerr << "Usage: ./solve_alphabeta ataxx_WIDTHxHEIGHT [DEPTH]" << std::endl;
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

    AtaxxGame game(width, height);
    DFAString initial = game.get_position_initial();
    int positions_evaluated = 0;

    std::cout << "checking for P1 (black) let's hope you finally work..." << std::endl;
    std::cout << game.position_to_string(initial) << std::endl;

    double score = alphabeta(game, initial, 0, max_depth, -1000, 1000, positions_evaluated);

    std::cout << "AlphaBeta result: " << score << "\n";
    std::cout << "Positions evaluated: " << positions_evaluated << "\n";

    return 0;
}
