#include <optional>
#include <string>

#include "game.hpp"

int main(int argc, char** argv) {
    bool startWithAi = false;
    std::optional<std::string> recordPath;
    std::optional<std::string> replayPath;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--ai") {
            startWithAi = true;
        } else if (arg == "--record" && i + 1 < argc) {
            recordPath = argv[++i];
        } else if (arg == "--replay" && i + 1 < argc) {
            replayPath = argv[++i];
        }
    }

    Game game(startWithAi, recordPath, replayPath);
    game.Run();
    return 0;
}
