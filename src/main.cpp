#include "core/Game.h"
#include <iostream>

int main(int argc, const char* argv[]) {
    std::cout << "Starting OmegaRace..." << std::endl;
    
    std::unique_ptr<omegarace::Game> game = std::make_unique<omegarace::Game>();
    std::cout << "Game object created, starting execution..." << std::endl;
    
    game->OnExecute();
    
    std::cout << "Game execution completed." << std::endl;
    return 0;
}
