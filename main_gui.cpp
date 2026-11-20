#include <raygui.h>
#include <raylib.h>
#include <cstring>
#include <string>
#include <iostream>
using namespace std;

enum AppScreen {
    SCREEN_LOGIN = 0,
    SCREEN_DASHBOARD
};

struct UserSession {
    char email[64];
    bool isLoggedIn;
};

enum LoginBox { BOX_NONE = 0, BOX_EMAIL, BOX_PASSWORD };


AppScreen UpdateAndDrawLoginScreen(Font customFont, UserSession &outUser)
{
    const int screenWidth = 1000;
    const int screenHeight = 1000;

    static LoginBox activeBox = BOX_EMAIL;
    static char emailText[64] = "\0";
    static char passwordText[64] = "\0";
    static int emailLetterCount = 0;
    static int passwordLetterCount = 0;

    // UI Element Bounds (adjusted for a 1000x1000 window layout)
    Rectangle emailBoxBounds = { (float)screenWidth/2 - 150, 350, 300, 40 };
    Rectangle passwordBoxBounds = { (float)screenWidth/2 - 150, 430, 300, 40 };
    Rectangle submitButtonBounds = { (float)screenWidth/2 - 150, 500, 300, 45 };

    Vector2 mousePoint = GetMousePosition();

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        if(CheckCollisionPointRec(mousePoint, emailBoxBounds)){
            activeBox = BOX_EMAIL;
        }else if(CheckCollisionPointRec(mousePoint, passwordBoxBounds)){
            activeBox = BOX_PASSWORD;
        }else{
            activeBox = BOX_NONE;
        }
    }

    if(IsKeyPressed(KEY_TAB)){
        if(activeBox == BOX_EMAIL){
            activeBox = BOX_PASSWORD;
        }else{
            activeBox = BOX_EMAIL;
        }
    }

    int key = GetCharPressed();
    while(key > 0){
        if(key >= 32 && key <= 125){
            if(activeBox == BOX_EMAIL && emailLetterCount < 63){
                emailText[emailLetterCount] = (char)key;
                emailText[emailLetterCount + 1] = '\0';
                emailLetterCount++;
            }else if(activeBox == BOX_PASSWORD && passwordLetterCount < 63){
                passwordText[passwordLetterCount] = (char)key;
                passwordText[passwordLetterCount + 1] = '\0';
                passwordLetterCount++;
            }
        }
        key = GetCharPressed();
    }

    if(IsKeyPressed(KEY_BACKSPACE)){
        if(activeBox == BOX_EMAIL && emailLetterCount > 0){
            emailLetterCount--;
            emailText[emailLetterCount] = '\0';
        }else if(activeBox == BOX_PASSWORD && passwordLetterCount > 0){
            passwordLetterCount--;
            passwordText[passwordLetterCount] = '\0';
        }
    }

    bool isClicked = CheckCollisionPointRec(mousePoint, submitButtonBounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if(isClicked || IsKeyPressed(KEY_ENTER)){
        strcpy(outUser.email, emailText);
        outUser.isLoggedIn = true;
        return SCREEN_DASHBOARD; // Fixed to match valid enum value
    }

    // DRAWING UI
    DrawTextEx(customFont, "CAMPUS SPACE BOOKER",
               (Vector2){ (float)(screenWidth/2 - MeasureTextEx(customFont, "CAMPUS SPACE BOOKER", 26, 1).x/2), 200 },
               26, 1, DARKBLUE);

    DrawTextEx(customFont, "Sign in with your student account",
               (Vector2){ (float)(screenWidth/2 - MeasureTextEx(customFont, "Sign in with your student account", 16, 1).x/2), 240 },
               16, 1, GRAY);

    // Email Field
    DrawTextEx(customFont, "Campus Email", (Vector2){ emailBoxBounds.x, emailBoxBounds.y - 22 }, 14, 1, DARKGRAY);
    DrawRectangleRec(emailBoxBounds, LIGHTGRAY);
    DrawRectangleLinesEx(emailBoxBounds, 2, (activeBox == BOX_EMAIL) ? BLUE : GRAY); // Added missing '?'

    if (emailLetterCount == 0 && activeBox != BOX_EMAIL)
        DrawTextEx(customFont, "student@university.edu", (Vector2){ emailBoxBounds.x + 10, emailBoxBounds.y + 10 }, 16, 1, GRAY);
    else{
        DrawTextEx(customFont, emailText, (Vector2){ emailBoxBounds.x + 10, emailBoxBounds.y + 10 }, 16, 1, BLACK);
    }

    // Password Field
    DrawTextEx(customFont, "Password", (Vector2){ passwordBoxBounds.x, passwordBoxBounds.y - 22 }, 14, 1, DARKGRAY);
    DrawRectangleRec(passwordBoxBounds, LIGHTGRAY);
    DrawRectangleLinesEx(passwordBoxBounds, 2, (activeBox == BOX_PASSWORD) ? BLUE : GRAY);

    if (passwordLetterCount == 0 && activeBox != BOX_PASSWORD)
    {
        DrawTextEx(customFont, "••••••••", (Vector2){ passwordBoxBounds.x + 10, passwordBoxBounds.y + 10 }, 16, 1, GRAY);
    }
    else
    {
        char maskedPassword[64] = "\0";
        for (int i = 0; i < passwordLetterCount; i++){
            maskedPassword[i] = '*';
        }
        maskedPassword[passwordLetterCount] = '\0';

        DrawTextEx(customFont, maskedPassword, (Vector2){ passwordBoxBounds.x + 10, passwordBoxBounds.y + 12 }, 16, 1, BLACK);
    }

    // Submit Button
    bool isHovered = CheckCollisionPointRec(mousePoint, submitButtonBounds);
    DrawRectangleRec(submitButtonBounds, isHovered ? BLUE : DARKBLUE);

    Vector2 btnTextSize = MeasureTextEx(customFont, "SIGN IN", 18, 1);
    DrawTextEx(customFont, "SIGN IN",
               (Vector2){ submitButtonBounds.x + (submitButtonBounds.width/2 - btnTextSize.x/2), submitButtonBounds.y + 13 },
               18, 1, WHITE);

    const char* hintText = "Press TAB to switch fields | Press ENTER to Submit";
    DrawTextEx(customFont, hintText,
               (Vector2){ (float)(screenWidth/2 - MeasureTextEx(customFont, hintText, 12, 1).x/2), 570 },
               12, 1, DARKGRAY);

    return SCREEN_LOGIN;
}




int main(){
    const int screenWidth = 1000;
    const int screenHeight = 1000;

    char customerName[64];

    // 1. Initialize window first so Raylib context is active
    InitWindow(screenWidth, screenHeight, "Smart Campus App");

    // 2. Load font AFTER window initialization
    Font customFont = LoadFontEx("Poppins-BlackItalic.ttf", 32, 0, 250);
    SetTextureFilter(customFont.texture, TEXTURE_FILTER_BILINEAR);
    AppScreen currentScreen = SCREEN_LOGIN;
    UserSession currentUser = { "", false };

    SetTargetFPS(60);
    while(!WindowShouldClose()){
         Rectangle backButton = { 40.0f, 50.0f, 100.0f, 42.0f };
         bool isHovered = CheckCollisionPointRec(GetMousePosition(), backButton)  && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

        if(currentScreen == SCREEN_LOGIN){
            currentScreen = UpdateAndDrawLoginScreen(customFont, currentUser);
        }else if(currentScreen == SCREEN_DASHBOARD){
            if (isHovered) {
                currentUser.isLoggedIn = false;
                currentScreen = SCREEN_LOGIN;
            }
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            if (currentScreen == SCREEN_LOGIN)
            {
                // Login UI is entirely handled inside UpdateAndDrawLoginScreen function
            }
            else if (currentScreen == SCREEN_DASHBOARD)
            {
                int i = 0;
                while(currentUser.email[i] != '\0'){
                    if(currentUser.email[i] != '@'){
                        customerName[i] = currentUser.email[i];
                    }else{
                        customerName[i] = '\0';
                        break;
                    }
                    i++;
                }
                //190, 100
                string welcome = "Welcome, " + string(customerName) + "!";
                Vector2 size = MeasureTextEx(customFont, welcome.c_str(), 24, 1);
                DrawTextEx(customFont, TextFormat("Welcome, %s!", customerName), (Vector2){ screenWidth/2 - size.x, 100}, 24, 1, BLACK);
                DrawRectangleRec(backButton, isHovered ? RED : MAROON);
                DrawRectangleLinesEx(backButton, 2, WHITE);
                DrawTextEx(customFont, "<- Back", (Vector2){ backButton.x + 20, backButton.y + 12 }, 16, 1, WHITE);

            }


        EndDrawing();
    }

    UnloadFont(customFont);
    CloseWindow();

    return 0;
}
