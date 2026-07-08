#include "raylib.h"
#include <string>
using namespace std;

int main() {
	InitWindow(320, 240, "Zero");
	SetTargetFPS(120);

	while (!WindowShouldClose()) {
		BeginDrawing();
			ClearBackground(BLACK);
			string fpsLabel = "FPS: " + to_string(GetFPS());
			DrawText(fpsLabel.c_str(), 0, 0, 20, WHITE);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
