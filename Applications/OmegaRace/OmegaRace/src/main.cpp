#include "Game.h"

int main(int argc, const char* argv[]) {
    std::unique_ptr<omegarace::Game> game = std::make_unique<omegarace::Game>();
    game->OnExecute();
    return 0;
}
