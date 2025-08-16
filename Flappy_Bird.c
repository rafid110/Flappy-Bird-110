#include "raylib.h"
#include <stdio.h>

#define MAX_OBJECTS 5
#define SCREEN_WIDTH 850
#define SCREEN_HEIGHT 550
#define BIRD_FRAMES 4

Texture2D bird[BIRD_FRAMES];

// Game Object 
typedef struct GameObject {
    Texture2D texture;
    Vector2 position;
    float width;
    bool passed;
} GameObject;


int main() {
    // Variables
    Texture2D Smoke;
    Sound click;
    Sound fail;

    // Window Setup
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy Bird 110"); 
    SetTargetFPS(30);
    InitAudioDevice();
    
    Music bgm = LoadMusicStream("background.mp3");  // Load music
    bgm.looping = true; // To make sure it loops
    PlayMusicStream(bgm); // Starts playing

    // Load textures and sounds
    Smoke = LoadTexture("Smoke.png");
    click = LoadSound("click.wav");
    fail = LoadSound("fail.wav");

    Texture2D birdTextures[BIRD_FRAMES];
    for (int i = 0; i < BIRD_FRAMES; i++) {
        char birdFile[12];
        sprintf(birdFile, "bird_%d.png", i);
        birdTextures[i] = LoadTexture(birdFile);
    }

    int currentFrame = 0;
    float frameTime = 0.1f; 
    float frameCounter = 0;
    
    float baseSpeed = 1.0f;   // starting speed
    float gameSpeed = baseSpeed;
    float speedIncrease = 0.015f; // how fast the game accelerates


    Texture2D textures[MAX_OBJECTS] = {
        LoadTexture("tree.png"),
        LoadTexture("hippo.png"),
        LoadTexture("cloud.png"),
        LoadTexture("croc.png"),
        LoadTexture("sun.png")
    };

    float birdX = 130;
    float birdY = 8;
    float birdVelocity = 0;
    const float gravity = 35.0f;
    const float jumpStrength = -300.0f;

    GameObject objects[MAX_OBJECTS];
    int objectSpacing = 300;
    int startX = 800;
    float objectSpeed = 180.0f;

    // Set manually controlled positions
    objects[0].texture = textures[0];
    objects[0].position = (Vector2){ 600, SCREEN_HEIGHT - textures[0].height - 10 };
    objects[0].width = textures[0].width;

    objects[1].texture = textures[1];
    objects[1].position = (Vector2){ 850, SCREEN_HEIGHT - textures[1].height - 10 };
    objects[1].width = textures[1].width;

    objects[2].texture = textures[2];
    objects[2].position = (Vector2){ 1100, 100 };
    objects[2].width = textures[2].width;

    objects[3].texture = textures[3];
    objects[3].position = (Vector2){ 1350, SCREEN_HEIGHT - textures[3].height - 90 };
    objects[3].width = textures[3].width;

    objects[4].texture = textures[4];
    objects[4].position = (Vector2){ 1500, 25 };
    objects[4].width = textures[4].width;

    for (int i = 0; i < MAX_OBJECTS; i++) {
        objects[i].passed = false;
    }

    int score = 0;
    int highScore = 0;
    bool gameOver = false;
    bool paused = false; // Pause state

    // Colors
    Color LightBlue = {208, 226, 237, 255};

    // Game Loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        UpdateMusicStream(bgm);  // Keep streaming the music

        // Pause Toggle
        if (IsKeyPressed(KEY_P)) paused = !paused;

        if (!gameOver && !paused) {
            // Bird physics
            if (IsKeyPressed(KEY_SPACE)) {
                birdVelocity = jumpStrength * dt;
                PlaySound(click);
            }
            
            gameSpeed += speedIncrease * GetFrameTime();  // gradually faster

            frameCounter += dt;
            if (frameCounter >= frameTime) {
                currentFrame = (currentFrame + 1) % BIRD_FRAMES;
                frameCounter = 0.0f;
            }

            birdVelocity += gravity * dt;
            birdY += birdVelocity;

            // Move objects
            for (int i = 0; i < MAX_OBJECTS; i++) {
                objects[i].position.x -= gameSpeed * objectSpeed * dt;

                // Reset object when off screen
                if (objects[i].position.x + objects[i].width < 0) {
                    float farthestX = 0;
                    for (int j = 0; j < MAX_OBJECTS; j++) {
                        if (j != i && objects[j].position.x > farthestX) {
                            farthestX = objects[j].position.x;
                        }
                    }
                    objects[i].position.x = farthestX + objectSpacing;
                    objects[i].passed = false;
                }
                
                if (gameSpeed > 2.0f) gameSpeed = 2.0f;

                // Score
                if (!objects[i].passed && birdX > objects[i].position.x + objects[i].width) {
                    score++;
                    objects[i].passed = true;
                }

                // Collision
                Rectangle birdHitbox = {
                    birdX + 10,
                    birdY + 8,
                    birdTextures[currentFrame].width - 20,
                    birdTextures[currentFrame].height - 16
                };

                Rectangle objRect = { objects[i].position.x, objects[i].position.y,
                                      objects[i].texture.width, objects[i].texture.height };
                if (CheckCollisionRecs(birdHitbox, objRect)) {
                    gameOver = true;
                    PlaySound(fail);
                }
            }

            // Border collision
            if (birdY < 0 || birdY + birdTextures[currentFrame].height > SCREEN_HEIGHT - 16) {
                gameOver = true;
                PlaySound(fail);
            }
        } 
        else if (gameOver) {
            if (IsKeyPressed(KEY_ENTER)) {
                // Restart game
                birdY = 8;
                birdVelocity = 0;
                score = 0;
                gameOver = false;
                gameSpeed = baseSpeed;

                
                // restart music
                SeekMusicStream(bgm, 0.0f);
                PlayMusicStream(bgm);

                for (int i = 0; i < MAX_OBJECTS; i++) {
                    objects[i].position.x = startX + i * objectSpacing;
                    objects[i].passed = false;
                }
            }
        }

        // High Score Update
        if (score > highScore) highScore = score;

        // Drawing
        BeginDrawing();
        ClearBackground(LightBlue);

        // Background
        DrawTexture(Smoke, 0, -80, WHITE);
        DrawRectangle(0, SCREEN_HEIGHT - 11, 1000, 16, DARKGREEN);

        // UI
        DrawText("Flappy Bird", 10, 7, 24, BLACK);
        DrawText(TextFormat("SCORE: %d", score), 10, 40, 20, DARKBLUE);
        DrawText(TextFormat("HIGH SCORE: %d", highScore), 10, 70, 20, MAROON);
        DrawText(TextFormat("FPS: %i", GetFPS()), 10, 98, 20, DARKGRAY);
        DrawText("Press [P] to Pause/Resume", 10, 124, 20, GRAY);

        // Draw bird
        DrawTexture(birdTextures[currentFrame], birdX, birdY, WHITE);

        // Draw objects
        for (int i = 0; i < MAX_OBJECTS; i++) {
            DrawTexture(objects[i].texture, objects[i].position.x, objects[i].position.y, WHITE);
        }

        // Game Over Screen
        if (gameOver) {
            DrawText("GAME OVER!", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 40, 30, RED);
            DrawText("Press [ENTER] to Restart", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2, 20, DARKGRAY);
        }

        // Pause Screen
        if (paused && !gameOver) {
            DrawText("PAUSED", SCREEN_WIDTH/2 - 50, SCREEN_HEIGHT/2 - 20, 30, DARKBLUE);
            DrawText("Press [P] to Resume", SCREEN_WIDTH/2 - 110, SCREEN_HEIGHT/2 + 20, 20, GRAY);
        }

        EndDrawing();
    }

    // Cleanup
    for (int i = 0; i < BIRD_FRAMES; i++) {
        UnloadTexture(birdTextures[i]);
    }
    for (int i = 0; i < MAX_OBJECTS; i++) {
        UnloadTexture(textures[i]);
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}