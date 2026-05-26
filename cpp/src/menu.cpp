#include "menu.hpp"

#include <raylib.h>

namespace {

constexpr Color kBackground{30, 30, 30, 255};
constexpr Color kNormal{255, 255, 255, 255};
constexpr Color kSelected{255, 215, 0, 255};
constexpr Color kHint{160, 160, 160, 255};

void DrawCentered(const char* text, int y, int fontSize, Color color) {
    int width = MeasureText(text, fontSize);
    DrawText(text, (config::WindowWidth - width) / 2, y, fontSize, color);
}

bool TogglePressed() {
    return IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_A) || IsKeyPressed(KEY_D);
}

bool ConfirmPressed() {
    return IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_KP_ENTER);
}

}  // namespace

bool MenuController::ShowStart(GameSettings& settings) const {
    constexpr int kItemCount = 4;  // apples, player, play, quit
    int focus = 2;                 // start on "Play" so Enter alone is enough

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) focus = (focus + 1) % kItemCount;
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) focus = (focus - 1 + kItemCount) % kItemCount;

        if (focus == 0 && TogglePressed()) {
            settings.appleCount =
                (settings.appleCount == config::FewApples) ? config::ManyApples : config::FewApples;
        }
        if (focus == 1 && TogglePressed()) {
            settings.aiEnabled = !settings.aiEnabled;
        }
        if (ConfirmPressed()) {
            if (focus == 2) return true;   // Play
            if (focus == 3) return false;  // Quit
        }

        BeginDrawing();
        ClearBackground(kBackground);

        DrawCentered("SNAKE", 70, 56, kNormal);

        const char* appleLabel =
            settings.appleCount == config::FewApples ? "Apples:  <  Little  >" : "Apples:  <  Many  >";
        DrawCentered(appleLabel, 220, 28, focus == 0 ? kSelected : kNormal);

        const char* playerLabel = settings.aiEnabled ? "Player:  <  AI  >" : "Player:  <  Human  >";
        DrawCentered(playerLabel, 270, 28, focus == 1 ? kSelected : kNormal);

        DrawCentered("Play", 340, 32, focus == 2 ? kSelected : kNormal);
        DrawCentered("Quit", 390, 32, focus == 3 ? kSelected : kNormal);

        DrawCentered("Arrows to navigate, Enter to select", config::WindowHeight - 60, 18, kHint);

        EndDrawing();
    }
    return false;
}

bool MenuController::ShowGameOver(int score, int highScore) const {
    int focus = 0;  // 0 = play again, 1 = quit

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_W)) {
            focus = 1 - focus;
        }
        if (ConfirmPressed()) {
            return focus == 0;
        }

        BeginDrawing();
        ClearBackground(kBackground);

        DrawCentered("GAME OVER", 140, 48, kNormal);
        DrawCentered(TextFormat("Score: %d", score), 250, 28, kNormal);
        DrawCentered(TextFormat("Best: %d", highScore), 290, 28, kNormal);

        DrawCentered("Play Again", 380, 32, focus == 0 ? kSelected : kNormal);
        DrawCentered("Quit", 430, 32, focus == 1 ? kSelected : kNormal);

        DrawCentered("Arrows to navigate, Enter to select", config::WindowHeight - 60, 18, kHint);

        EndDrawing();
    }
    return false;
}
