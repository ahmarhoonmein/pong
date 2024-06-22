#include <iostream>
#include "raylib.h"

using namespace std;

Color *Green = new Color{38,185,154,255};
Color *DarkGreen = new Color {20,160,133,255};
Color *LightGreen = new Color {129,204,184,255};
Color *Yellow = new Color {243,213,91,255};

int playerScore = 0;
int cpuScore = 0;

class Paddle;
class CPUPaddle;

class StartupMenu {
public:
    StartupMenu(const int screenWidth, const int screenHeight);
    bool Show();
private:
    const int screenWidth;
    const int screenHeight;
};

StartupMenu::StartupMenu(const int screenWidth, const int screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight) {}

bool StartupMenu::Show() {
    const int buttonWidth = 200;
    const int buttonHeight = 50;
    const int buttonSpacing = 20;
    const int startX = screenWidth / 2 - buttonWidth / 2;
    const int startY = screenHeight / 2 - (buttonHeight * 3 + buttonSpacing * 2) / 2;

    InitWindow(screenWidth, screenHeight, "Pong Game - Startup Menu");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(*Green);

        DrawText("Pong Game", screenWidth / 2 - MeasureText("Pong Game", 40) / 2, startY - 100, 40, *DarkGreen);

        if (CheckCollisionPointRec(GetMousePosition(), { (float)startX, (float)startY, (float)buttonWidth, (float)buttonHeight })) {
            DrawRectangle(startX, startY, buttonWidth, buttonHeight, *DarkGreen);
        } else {
            DrawRectangle(startX, startY, buttonWidth, buttonHeight, *LightGreen);
        }
        DrawText("Start Game", startX + 25, startY + 10, 20, *DarkGreen);

        if (CheckCollisionPointRec(GetMousePosition(), { (float)startX, (float)startY + buttonHeight + buttonSpacing, (float)buttonWidth, (float)buttonHeight })) {
            DrawRectangle(startX, startY + buttonHeight + buttonSpacing, buttonWidth, buttonHeight, *DarkGreen);
        } else {
            DrawRectangle(startX, startY + buttonHeight + buttonSpacing, buttonWidth, buttonHeight, *LightGreen);
        }
        if (CheckCollisionPointRec(GetMousePosition(), { (float)startX, (float)startY + (buttonHeight + buttonSpacing) * 2, (float)buttonWidth, (float)buttonHeight })) {
            DrawRectangle(startX, startY + (buttonHeight + buttonSpacing) * 2, buttonWidth, buttonHeight, *DarkGreen);
        } else {
            DrawRectangle(startX, startY + (buttonHeight + buttonSpacing) * 2, buttonWidth, buttonHeight, *LightGreen);
        }
        DrawText("Exit", startX + 25, startY + (buttonHeight + buttonSpacing) * 2 + 10, 20, *DarkGreen);

        EndDrawing();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(GetMousePosition(), { (float)startX, (float)startY, (float)buttonWidth, (float)buttonHeight })) {
                CloseWindow();
                return true; // Start the game
            } else if (CheckCollisionPointRec(GetMousePosition(), { (float)startX, (float)startY + (buttonHeight + buttonSpacing) * 2, (float)buttonWidth, (float)buttonHeight })) {
                CloseWindow();
                return false; // Exit the game
            }
        }
    }

    CloseWindow();
    return false;
}

class Frame {
protected:
    const int screenWidth;
    const int screenHeight;
public:
    Frame();
    ~Frame();
    int getScreenWidth();
    int getScreenHeight();
};

class Shapes : public Frame {
protected:
    const Color shapeColor;
    int xAxis;
    int yAxis;
public:
    Shapes() : shapeColor(WHITE) {}
    Color getShapeColor() {return shapeColor;}
};

class Ball : public Shapes {
    friend void blockPlayerCollision(Ball &ball, Paddle &paddle);
    friend void blockCPUCollision(Ball &ball, CPUPaddle &cpuPaddle);
private:
    int speedX;
    int speedY;
    const float radius;
public:
    Ball() : speedX(7), speedY(7), radius(20){
        xAxis =  screenWidth / 2;
        yAxis =  screenHeight / 2;
    }
    void Draw() {
        DrawCircle(xAxis, yAxis,radius,*Yellow);
    }

    void Update() {
        xAxis += speedX;
        yAxis += speedY;

        if (yAxis + radius >= GetScreenHeight() || yAxis - radius <= 0) {
            speedY *= -1;
        }

        // If the ball goes out from the left side, player receives a point
        if (xAxis - radius <= 0) {
            cpuScore++;
            ResetBall(false); // Reset ball from the middle towards the left side
        }

        // If the ball goes out from the right side, CPU receives a point
        if (xAxis + radius >= GetScreenWidth()) {
            playerScore++;
            ResetBall(true); // Reset ball from the middle towards the right side
        }
    }

    int getXAxis() {
        return xAxis;
    }

    int getYAxis() {
        return yAxis;
    }

    int getRadius() {
        return radius;
    }

    int getSpeedX() {
        return speedX;
    }

    int getSpeedY() {
        return speedY;
    }

    void setSpeedX(int speed) {
        speedX = speed;
    }

    void ResetBall(bool towardsLeft) {
        xAxis = GetScreenWidth() / 2;
        yAxis = GetScreenHeight() / 2;

        int speed_choices[2] = {-1, 1};
        if (towardsLeft) {
            speedX = -abs(speedX); // Move towards the left side
        } else {
            speedX = abs(speedX); // Move towards the right side
        }
        speedY *= speed_choices[GetRandomValue(0, 1)];
    }

};

class line : public Shapes {
private:
    const int xPosStart;
    const int yPosStart;
    const int xPosEnd;
    const int yPosEnd;
public:
    line() : xPosStart(screenWidth/2), yPosStart(0), xPosEnd(screenWidth/2), yPosEnd(screenHeight) {}

    void Draw() {
        DrawLine(xPosStart, yPosStart, xPosEnd, yPosEnd, shapeColor);
    }
};

class Paddle : public Frame{
    friend void blockPlayerCollision(Ball &ball, Paddle &paddle);
protected:
    int xAxis;
    int yAxis;
    const int width;
    const int height;
    int speed;
public:
    Paddle() : width(25), height(120) {
        xAxis = (screenWidth - width - 10);
        yAxis = (screenHeight/2) - (height/2);
        speed = 6;
    }

    void Draw() {
        DrawRectangle(xAxis, yAxis, width, height,WHITE);
    }

    void Update() {
        if(IsKeyDown(KEY_UP)) {
            yAxis = yAxis - speed;
        }else if(IsKeyDown(KEY_DOWN)) {
            yAxis = yAxis + speed;
        }

        if(yAxis <= 0) {
            yAxis = 0;
        }
        if(yAxis + height >= GetScreenHeight()) {
            yAxis = GetScreenHeight() - height;
        }
    }

    int getXAxis() {
        return xAxis;
    }

    int getYAxis() {
        return yAxis;
    }

    int getWidth() {
        return width;
    }

    int getHeight() {
        return height;
    }
};

class CPUPaddle : public Paddle {
    friend void blockCPUCollision(Ball &ball, CPUPaddle &cpuPaddle);
private:
    const int CPUwidth;
    const int CPUheight;
public:
    CPUPaddle() : CPUwidth(25), CPUheight(120){
        xAxis = 10;
        yAxis = (screenHeight/2) - (height/2);
        speed = 6;
    }
// Override:
    void Draw() {
        DrawRectangle(xAxis, yAxis, CPUwidth, CPUheight,WHITE);
    }
    // AI Algorithm for Update:
    void Update(int ball_y) {
        if(yAxis + CPUheight/2 > ball_y) {
            yAxis = yAxis - speed; // move paddle up if the centre of ball is higher than the center of CPU Paddle.
        }
        if(yAxis + CPUheight/2 < ball_y) {
            yAxis = yAxis + speed;
        }

        if(yAxis <= 0) {
            yAxis = 0;
        }
        if(yAxis + height >= GetScreenHeight()) {
            yAxis = GetScreenHeight() - height;
        }
    }


    int getXAxis() {
        return xAxis;
    }

    int getYAxis() {
        return yAxis;
    }

    int getWidth() {
        return CPUwidth;
    }

    int getHeight() {
        return CPUheight;
    }
};

void blockPlayerCollision(Ball &ball, Paddle &paddle) {
    if(CheckCollisionCircleRec(Vector2{static_cast<float>(ball.xAxis), static_cast<float>(ball.yAxis)}, ball.radius, Rectangle{static_cast<float>(paddle.xAxis), static_cast<float>(paddle.yAxis), static_cast<float>(paddle.width), static_cast<float>(paddle.height)}))
    {
        ball.speedX *= -1;
    }
}

void blockCPUCollision(Ball &ball, CPUPaddle &paddle) {
    if(CheckCollisionCircleRec(Vector2{static_cast<float>(ball.xAxis), static_cast<float>(ball.yAxis)}, ball.radius, Rectangle{static_cast<float>(paddle.xAxis), static_cast<float>(paddle.yAxis), static_cast<float>(paddle.width), static_cast<float>(paddle.height)}))
    {
        ball.speedX *= -1;
    }
}

Frame :: Frame() : screenWidth(1280), screenHeight(800){}
Frame :: ~Frame() {cout << "Destructor Called!" << endl;}
int Frame :: getScreenWidth() {
    return screenWidth;
}

int Frame :: getScreenHeight() {
    return screenHeight;
}

int main() {

    Frame *screen;
    screen = new Frame();

    const int width = screen->getScreenWidth();
    const int height = screen->getScreenHeight();

    StartupMenu menu(width, height);
    if (!menu.Show()) {
        delete screen;
        return 0;
    }

    InitWindow(width, height, "Minimal Ping Pong");
    SetTargetFPS(120); // speed of game.

    Ball *ball;
    ball = new Ball();

    line *midLine;
    midLine = new line();

    Paddle *player1;
    player1 = new Paddle();

    CPUPaddle *cpuPlayer;
    cpuPlayer = new CPUPaddle();

    while (!WindowShouldClose()) {
        BeginDrawing(); // creates new canvas for drawing elements.
        ClearBackground(*Green); // background color.

        player1->Update();
        cpuPlayer->Update(ball->getYAxis());

        player1->Draw();
        cpuPlayer->Draw();
        DrawCircle(width / 2, height / 2, 120, *DarkGreen);
        midLine->Draw();
        ball->Draw();
        ball->Update();
        // Draw scores for both players
        DrawText(TextFormat("%i", playerScore), width / 4 - 21, 20, 80, WHITE); // Draw player 1 score
        DrawText(TextFormat("%i", cpuScore), 3 * width / 4 - 21, 20, 80, WHITE); // Draw CPU score

        blockPlayerCollision(*ball, *player1);
        blockCPUCollision(*ball, *cpuPlayer);

        EndDrawing();
    }

    CloseWindow(); // closes frame window.
    // free memory from heap.
    delete cpuPlayer;
    delete player1;
    delete midLine;
    delete ball;
    delete screen;

    return 0;
}