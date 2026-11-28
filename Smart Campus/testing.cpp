#include <raygui.h>
#include <raylib.h>
#include <cstring>
#include <string>
#include<iostream>

int main(void)
{
    const int screenWidth = 400;
    const int screenHeight = 700;
    InitWindow(screenWidth, screenHeight, "Raylib Mobile Scrolling Example");

    // Define the scrollable viewport (like a phone app container)
    Rectangle viewBounds = { 20, 100, (float)screenWidth - 40, (float)screenHeight - 150 };

    // Total height of all content combined (e.g., 10 cards * 70 pixels each = 700 pixels content)
    float contentHeight = 700.0f;
    float scrollOffsetY = 0.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // ==========================================
        // 1. UPDATE: Handle Scrolling Input
        // ==========================================
        float wheelMove = GetMouseWheelMove();
        if (wheelMove != 0)
        {
            scrollOffsetY += wheelMove * 25.0f; // Scroll speed multiplier
        }

        // Clamp scrolling so the user can't scroll infinitely into empty space
        float maxScroll = contentHeight - viewBounds.height;
        if (scrollOffsetY < -maxScroll) scrollOffsetY = -maxScroll; // Can't scroll past the bottom
        if (scrollOffsetY > 0.0f) scrollOffsetY = 0.0f;             // Can't scroll past the top

        // ==========================================
        // 2. DRAWING
        // ==========================================
        BeginDrawing();
            ClearBackground(RAYWHITE);

            // App Header (Static, doesn't scroll)
            DrawRectangle(0, 0, screenWidth, 80, DARKBLUE);
            DrawText("Available Study Rooms", 25, 30, 20, WHITE);

            // --- START SCISSOR MODE (Clips overflow) ---
            BeginScissorMode((int)viewBounds.x, (int)viewBounds.y, (int)viewBounds.width, (int)viewBounds.height);
            {
                // Draw scrollable items relative to scrollOffsetY
                for (int i = 0; i < 10; i++)
                {
                    float cardY = viewBounds.y + i * 70.0f + scrollOffsetY;
                    Rectangle cardRect = { viewBounds.x, cardY, viewBounds.width - 10, 60 };

                    // Only draw if the card is actually visible inside the viewport (optimization)
                    if (cardY + cardRect.height >= viewBounds.y && cardY <= viewBounds.y + viewBounds.height)
                    {
                        DrawRectangleRounded(cardRect, 0.2f, 4, LIGHTGRAY);
                        DrawText(TextFormat("Room %d - 4 Seats", i + 1), (int)cardRect.x + 15, (int)cardRect.y + 20, 14, BLACK);
                    }
                }
            }
            EndScissorMode(); // --- END SCISSOR MODE ---

            // App Bottom Navigation Bar (Static)
            DrawRectangle(0, screenHeight - 50, screenWidth, 50, LIGHTGRAY);
            DrawText("Home   |   Search   |   Bookings   |   Profile", 50, screenHeight - 30, 12, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
