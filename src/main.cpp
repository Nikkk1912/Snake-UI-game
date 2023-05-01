#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <deque>

using namespace std;

Color dark_blue = {31, 29, 52, 255};
Color light_yellow = {248, 232, 199, 255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;

double lastUpdateTime = 0;

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{   
    for(unsigned int i = 0; i < deque.size(); i++)
    {
        if(Vector2Equals(deque[i], element))
        {
            return true;
        }
    
    }
    return false;
};

bool EventTrigger(double interval)
{
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

class Food {

    public:
        Vector2 position;
        Vector2 positionBonus;
        Texture2D texture;


        Food(deque<Vector2> snakeBody)
        {
            Image pic = LoadImage("textures/food.png");
            texture = LoadTextureFromImage(pic);
            UnloadImage(pic);
            position = GenerateRandPos(snakeBody);
            positionBonus = GenerateRandPos(snakeBody);
        } //constructor #1

        ~Food()
        {
            UnloadTexture(texture);
        }//destructor #1

        void Draw()
        {
            DrawTexture(texture,offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
            
        }

        void DrawBonus()
        {
            DrawRectangle(offset + positionBonus.x * cellSize, offset + positionBonus.y * cellSize, cellSize, cellSize, light_yellow);
        }

        Vector2 GenerateRandomCell()
        {
            float x = GetRandomValue(1, cellCount - 1);
            float y = GetRandomValue(1, cellCount - 1);
            return Vector2{x, y};
        }

        Vector2 GenerateRandPos(deque<Vector2> snakeBody)
        {
            
            Vector2 position = GenerateRandomCell();
            while(ElementInDeque(position, snakeBody))
            {
                position = GenerateRandomCell();
            }

            return position;
        }

        friend ostream& operator<<(ostream& os, const Food& food) 
        {
            TraceLog(LOG_INFO, "Food position: (%f, %f)", food.position.x, food.position.y);
            TraceLog(LOG_INFO, "Bonus food position: (%f, %f)", food.positionBonus.x, food.positionBonus.y);
            return os;
        }//operator<< overload
};


class Snake{

    public:
        deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
        Vector2 direction = {1, 0};
        bool addSegment = false;


        Snake() {} //#constructor #2

        ~Snake() {} //#destructor #2

        void Draw()
        {
            for(unsigned i =0; i <body.size(); i++)
            {
                float x = body[i].x;
                float y = body[i].y;
                Rectangle segment = Rectangle{offset + x*cellSize, offset+ y*cellSize, (float)cellSize, (float)cellSize};
                DrawRectangleRounded(segment, 0.5 , 6, light_yellow);
            }
        }

        void Update()
        {
            body.push_front(Vector2Add(body[0], direction));
            if (addSegment == true)
            {
                addSegment = false;
            }
            else
            {
                body.pop_back();
            }
            
        }

        void Reset()
        {
            body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
            direction = {1,0};
        }

        
};


class Game{
    public:
        Snake snake = Snake();
        Food food = Food(snake.body);
        bool running = true;
        int score = 0;

    Game() {} //constructor #3

    ~Game() {} //destructor #3

    void Draw()
    {
        food.Draw();
        food.DrawBonus();
        snake.Draw();
    }

    void Update()
    {
        if(running)
        {
        snake.Update();
        CheckCollisionWithFood();
        CheckCollisionWithBonus();
        CheckCollisionWithEdges();
        CheckCollisionWithTale();
        }
    }

    void CheckCollisionWithFood()
    {
        if(Vector2Equals(snake.body[0], food.position))
        {
            food.position = food.GenerateRandPos(snake.body);
            snake.addSegment = true;
            score++;
            cout << food;
        }
    }

    void CheckCollisionWithBonus()
    {
        if(Vector2Equals(snake.body[0], food.positionBonus))
        {
            food.positionBonus = food.GenerateRandPos(snake.body);
            snake.addSegment = true;
            score++;
            cout << food;
        }
    }

    void CheckCollisionWithEdges()
    {
        if(snake.body[0].x == cellCount || snake.body[0].x == -1)
        {
            GameOver();
        }
        if(snake.body[0].y == cellCount || snake.body[0].y == -1)
        {
            GameOver();
        }
    }

    void CheckCollisionWithTale()
    {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody))
        {
            GameOver();
        }
    }

    void GameOver()
    {
        snake.Reset();   
        food.position = food.GenerateRandPos(snake.body);
        running = false;
        score = 0;
    }

};

class Name
{   
    public:
    string windowName;
    string nameInGame;

    friend istream& operator>>(istream& is, Name name) 
        {
            cout << "Enter window name:";
            is >> name.windowName;
            return is;
        }
};//Operator >> overload

int main(){


    InitWindow(2*offset + cellSize * cellCount, 2*offset + cellSize * cellCount, "Snake");
    SetTargetFPS(60);

    Texture2D textureExample;
    Image pic = LoadImage("textures/food.png");
    textureExample = LoadTextureFromImage(pic);

    Game game = Game();

    while (WindowShouldClose()==false)
    {
        BeginDrawing();

        if(EventTrigger(0.17))
        {
            game.Update();
        }


        if(IsKeyPressed(KEY_W) && game.snake.direction.y != 1)
        {
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_S) && game.snake.direction.y != -1)
        {
            game.snake.direction = {0, 1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_A) && game.snake.direction.x != 1)
        {
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_D) && game.snake.direction.x != -1)
        {
            game.snake.direction = {1, 0};
            game.running = true;
        }
        

        
        ClearBackground(dark_blue);
        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize*cellCount+10, (float)cellSize*cellCount+10}, 3, light_yellow);
        DrawText("Snake", offset - 5 , 20, 40, light_yellow);
        DrawText(TextFormat("%i", game.score), offset -5, offset + cellSize*cellCount+10, 40, light_yellow);
        DrawTexture(textureExample, offset + 100, offset + cellSize*cellCount+15, WHITE);
        DrawText(" - food", offset + 130 , offset + cellSize*cellCount+15, 30, light_yellow);
        DrawRectangle(offset + 320, offset + cellSize*cellCount+15, cellSize, cellSize, light_yellow);
        DrawText(" - bonus food", offset + 350 , offset + cellSize*cellCount+15, 30, light_yellow);
        game.Draw();


        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}