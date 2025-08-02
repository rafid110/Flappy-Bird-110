#include "raylib.h"
#include <stdio.h>

#define MAX_OBJECTS 5
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 500
#define BIRD_FRAMES 4
Texture2D bird[BIRD_FRAMES];


//Game Object 
typedef struct GameObject {
    Texture2D texture;
    Vector2 position;
    float width;
    bool passed;
} GameObject;




int main(){
    
    
    //Variables
    Texture2D Smoke;
    Sound click;
    Sound fail;
    
    
    //Window Setup
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy Bird 110"); 
    SetTargetFPS(60);
    InitAudioDevice();
    
    
    // Load textures
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
    float frameTime = 0.1f; // seconds per frame
    float frameCounter = 0;



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
    const float gravity = 0.4f;
    const float jumpStrength = -5.8f;



    // Initialize objects with fixed spacing
    GameObject objects[MAX_OBJECTS];
    int objectSpacing = 300;
    int startX = 800;
    float objectSpeed = 3.5f;
    
    
    
    // Set manually controlled positions for each object
    objects[0].texture = textures[0]; // tree
    objects[0].position = (Vector2){ 600, SCREEN_HEIGHT - textures[0].height - 10 };
    objects[0].width = textures[0].width;
    
    objects[1].texture = textures[1]; // hippo
    objects[1].position = (Vector2){ 850, SCREEN_HEIGHT - textures[1].height - 10 };
    objects[1].width = textures[1].width;
    
    objects[2].texture = textures[2]; // cloud
    objects[2].position = (Vector2){ 1100, 80 }; // floating high
    objects[2].width = textures[2].width;
       
    objects[3].texture = textures[3]; // croc
    objects[3].position = (Vector2){ 1350, SCREEN_HEIGHT - textures[3].height - 90 };
    objects[3].width = textures[3].width;

    objects[4].texture = textures[4]; // sun
    objects[4].position = (Vector2){ 1500, 25 }; // floating high
    objects[4].width = textures[4].width;

    for (int i = 0; i < MAX_OBJECTS; i++) {
    objects[i].passed = false;
    }


    int score = 0;
    int highScore = 0;
    bool gameOver = false;



    //Colors 
    Color LightBlue = {208, 226, 237, 1};
    Color Green = {50, 123, 66, 255};
    
    
    //Gamo Loop
    while(WindowShouldClose() == false)
    {
        // 1. Event Handling
        if (!gameOver) {
            // Bird physics
            if (IsKeyPressed(KEY_SPACE)) {
                birdVelocity = jumpStrength;
            }
            frameCounter += GetFrameTime();
            if (frameCounter >= frameTime) {
                currentFrame = (currentFrame + 1) % BIRD_FRAMES;
                frameCounter = 0.0f;
            }


            birdVelocity += gravity;
            birdY += birdVelocity;

            // Move objects
            for (int i = 0; i < MAX_OBJECTS; i++) {
                objects[i].position.x -= objectSpeed;

                // Reset object when it goes off screen
                if (objects[i].position.x + objects[i].width < 0) {
                    // Find farthest object's X
                    float farthestX = 0;
                    for (int j = 0; j < MAX_OBJECTS; j++) {
                        if (j != i && objects[j].position.x > farthestX) {
                            farthestX = objects[j].position.x;
                    }
                }

                // Reposition this object after the farthest one, keeping the same Y
                objects[i].position.x = farthestX + objectSpacing;
                objects[i].passed = false;
            }
    

                // Score logic
                if (!objects[i].passed && birdX > objects[i].position.x + objects[i].width) {
                    score++;
                    objects[i].passed = true;
                }

                // Collision detection
                Rectangle birdRect = { birdX, birdY, bird[currentFrame].width, bird[currentFrame].height };
                Rectangle objRect = { objects[i].position.x, objects[i].position.y,
                                      objects[i].texture.width, objects[i].texture.height };
                if (CheckCollisionRecs(birdRect, objRect)) {
                    gameOver = true;
                    PlaySound (fail);
                }
            }

            // Screen border collision
            if (birdY < 0 || birdY + birdTextures[currentFrame].height > SCREEN_HEIGHT - 13) {
                gameOver = true;
                PlaySound (fail);
            }
        } else {
            if (IsKeyPressed(KEY_ENTER)) {
                // Restart game
                birdY = 8;
                birdVelocity = 0;
                score = 0;
                gameOver = false;

                // Reinitialize objects
                for (int i = 0; i < MAX_OBJECTS; i++) {
                    objects[i].position.x = ( startX ) + i * objectSpacing;
                    objects[i].passed = false;
                }
            }
        }
    
        // Update high score
        if (score > highScore) highScore = score;
        
        if (IsKeyPressed(KEY_SPACE)){
            PlaySound (click);
        }
        
         
        // 2. Drawing Objects
        
        
        
        BeginDrawing();
        ClearBackground(LightBlue);
        
        
        
        //Rectangles for background:
        DrawTexture(Smoke, 0, -80, WHITE);
        DrawRectangle(0, SCREEN_HEIGHT - 11, 800, 16, DARKGREEN);
        
        
        
        //Contents
        
        // Animate bird
        
        for (int i = 0; i < BIRD_FRAMES; i++) {
            char birdFile[11];
            sprintf(birdFile, "bird_%d.png", i);
            bird[i] = LoadTexture(birdFile);
        }

        // Drawing the bird
        Vector2 birdPos = {birdX, birdY}; // Declare birdPos
        DrawTexture(birdTextures[currentFrame], birdPos.x, birdPos.y, WHITE);
        
        DrawText(TextFormat("Flappy Bird"), 10, 7, 24, BLACK);
        DrawText(TextFormat("SCORE: %d", score), 10, 40, 20, DARKBLUE);
        DrawText(TextFormat("HIGH SCORE: %d", highScore), 10, 70, 20, MAROON);

       

        // Draw bird
        DrawTexture(birdTextures[currentFrame], birdX, birdY, WHITE); // Corrected line

        // Draw objects
        for (int i = 0; i < MAX_OBJECTS; i++) {
            DrawTexture(objects[i].texture, objects[i].position.x, objects[i].position.y, WHITE);
        }
        
        
        if (gameOver) {
            DrawText("GAME OVER!", SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 40, 30, RED);
            DrawText("Press [ENTER] to Restart", SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2, 20, DARKGRAY);
        }
        
        
        
        EndDrawing();    
    }
    for (int i = 0; i < BIRD_FRAMES; i++) {
        UnloadTexture(bird[i]);
    }


    for (int i = 0; i < MAX_OBJECTS; i++) {
        UnloadTexture(textures[i]);
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
    