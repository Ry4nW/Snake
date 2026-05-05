#include <string>

#include "game.hpp"

int main(int argc, char** argv) {
    bool startWithAi = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--ai") startWithAi = true;
    }

    Game game(startWithAi);
    game.Run();
    return 0;
}
