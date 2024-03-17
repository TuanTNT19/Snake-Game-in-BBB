
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <string.h>

//Linux Libraries
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define SNAKE_ARRAY_SIZE 310
#define SSD1306_DEV_FILE    "/dev/my_device_ssd"
#define EXIT_BUTTON 27 //ESC
#define PAUSE_BUTTON 112 //P
#define UP_ARROW  'A'
#define LEFT_ARROW  'D'
#define RIGHT_ARROW  'C'
#define DOWN_ARROW 'B'
#define ENTER_KEY 10

int fd;

int fx[26] = {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125};
char user_cmd[50];

int kbhit(void)
{

    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF){
        ungetc(ch, stdin);
        return 1;
  }

    return 0;
}

char getch()
{
    char c;
    system("stty raw");
    c= getchar();
    system("stty sane");
    return(c);
}

void clrscr()
{
    system("clear");
    return;
}

char waitForAnyKey(void)
{
    int pressed;

    while(!kbhit());

    pressed = getch();
    return((char)pressed);
}

int getGameSpeed()
{
    int speed = 1;
    clrscr();

    while(speed < 1 || speed > 9){
        return(speed);
    }
    return 1;
}

int checkKeysPressed(int direction)
{
int pressed;

if(kbhit()) //If a key has been pressed
{
    pressed=getch();
    if (direction != pressed)
    {
        if(pressed == DOWN_ARROW && direction != UP_ARROW)
            direction = pressed;
        else if (pressed == UP_ARROW && direction != DOWN_ARROW)
            direction = pressed;
        else if (pressed == LEFT_ARROW && direction != RIGHT_ARROW)
            direction = pressed;
        else if (pressed == RIGHT_ARROW && direction != LEFT_ARROW)
            direction = pressed;
    }
}
return(direction);
}

void set_cursor_to_LCD(int x, int y)
{
    char str[20];
    sprintf(str, "cursor %d %d", x, y);
    int w = write(fd, str, strlen(str));   
    if ( w == -1)
    {
        printf("Can not set cursor to LCD\n");
    }
}

void Display_to_LCD(char *str){
    int w = write(fd, str, strlen(str));   
    if ( w == -1)
    {
        printf("Can not write to LCD\n");
    }
}

int collisionSnake (int x, int y, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int detect)
{
    int i;
    for (i = detect; i < snakeLength; i++) //Checks if the snake collided with itself
    {
        if ( x == snakeXY[0][i] && y == snakeXY[1][i])
        return(1);
    }
    return(0);
}

int generateFood(int foodXY[], int width, int height, int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{
    do
    {

        srand ( time(NULL) );
        int temp =  rand() % 27;
        foodXY[0] = fx[temp];
        srand ( time(NULL) );
        foodXY[1] = rand() % 7;
    } while (collisionSnake(foodXY[0], foodXY[1], snakeXY, snakeLength, 0)); //This should prevent the "Food" from being created on top of the snake. - However the food has a chance to be created ontop of the snake, in which case the snake should eat it...

    set_cursor_to_LCD(foodXY[0] ,foodXY[1]);
    Display_to_LCD("$");

    return(0);
}

void prepairSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{
    int i;
    int snakeX = snakeXY[0][0];
    int snakeY = snakeXY[1][0];

    for(i = 1; i <= snakeLength; i++)
    {
        snakeXY[0][i] = snakeX - 5*i;
        snakeXY[1][i] = snakeY;
    }

    return;
}

void moveSnakeArray(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction)
{
    
int i;
for( i = snakeLength-1; i >= 1; i-- )
{
    snakeXY[0][i] = snakeXY[0][i-1];
    snakeXY[1][i] = snakeXY[1][i-1];
}

/*
because we don't actually know the new snakes head x y,
we have to check the direction and add or take from it depending on the direction.
*/
switch(direction)
{
    case DOWN_ARROW:
        snakeXY[1][0]++;
        break;
    case RIGHT_ARROW:
        snakeXY[0][0]+=5;
        break;
    case UP_ARROW:
        snakeXY[1][0]--;
        break;
    case LEFT_ARROW:
        snakeXY[0][0]-=5;
        break;
}

return;
}

void move(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength, int direction)
{
    int x;
    int y;

//Remove the tail ( HAS TO BE DONE BEFORE THE ARRAY IS MOVED!!!!! )
    x = snakeXY[0][snakeLength-1];
    y = snakeXY[1][snakeLength-1];

    set_cursor_to_LCD(x,y);
    Display_to_LCD(" ");

//Changes the head of the snake to a body part
    set_cursor_to_LCD(snakeXY[0][0],snakeXY[1][0]);
    Display_to_LCD("*");

    moveSnakeArray(snakeXY, snakeLength, direction);

    set_cursor_to_LCD(snakeXY[0][0],snakeXY[1][0]);
    Display_to_LCD("*");

    set_cursor_to_LCD(1,1);

    return;
}


//This function checks if the snakes head his on top of the food, if it is then it'll generate some more food...
int eatFood(int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[])
{
    if (snakeXY[0][0] == foodXY[0] && snakeXY[1][0] == foodXY[1])
    {
        foodXY[0] = 0;
        foodXY[1] = 0; //This should prevent a nasty bug (loops) need to check if the bug still exists...

        return(1);
    }

    return 0;
}


int collisionDetection(int snakeXY[][SNAKE_ARRAY_SIZE], int consoleWidth, int consoleHeight, int snakeLength)
{
    // Kiểm tra va chạm với tường
    if (snakeXY[0][0] < 0 || snakeXY[1][0] < 0 || snakeXY[0][0] >= consoleWidth || snakeXY[1][0] >= consoleHeight)
    {
        return 1; // Có va chạm với tường
    }
    
    // Kiểm tra va chạm với chính mình
    for (int i = 1; i < snakeLength; i++) // Bắt đầu từ 1 để bỏ qua đầu con rắn
    {
        if (snakeXY[0][0] == snakeXY[0][i] && snakeXY[1][0] == snakeXY[1][i])
        {
            return 1; // Có va chạm với chính mình
        }
    }
    
    return 0; // Không có va chạm
}


void loadSnake(int snakeXY[][SNAKE_ARRAY_SIZE], int snakeLength)
{
    int i;

    for (i = 0; i < snakeLength; i++)
    {
        set_cursor_to_LCD(snakeXY[0][i], snakeXY[1][i]);
        Display_to_LCD("*");
    }

    return;
}


void refreshInfoBar(int score, int speed)
{
    char str[10];
    set_cursor_to_LCD(0, 7);
    sprintf(str, "score:%d", score);
    Display_to_LCD(str);

    set_cursor_to_LCD(70, 7);
    sprintf(str, "speed:%d", speed);
    Display_to_LCD(str);

    return;
}

void clearLCD()
{
    write(fd, "clear", 5);
}

void gameOverScreen(void)
{
    set_cursor_to_LCD(25,3);
    Display_to_LCD("GAME OVER!");
    set_cursor_to_LCD(10, 4);
    Display_to_LCD("PRESS TO CONTINUE");
    waitForAnyKey();
    clrscr(); //clear the console
    return;
}

void startGame(int snakeXY[][SNAKE_ARRAY_SIZE], int foodXY[], int consoleWidth, int consoleHeight, int snakeLength, int direction, int score, int speed) {
    int gameOver = 0;
    clock_t endWait;

    //CLOCKS_PER_SEC-(n-1)*(CLOCKS_PER_SEC/10)
    int waitMili = CLOCKS_PER_SEC-(speed)*(CLOCKS_PER_SEC/10);  // Sets the correct wait time according to the selected speed
    int tempScore = 10*speed;
    int oldDirection;
    int canChangeDirection = 1;
    //int seconds = 1;

    endWait = clock() + waitMili;

    do {
        if(canChangeDirection) {
            oldDirection = direction;
            direction = checkKeysPressed(direction);
        }

        if(oldDirection != direction) // Temp fix to prevent the snake from colliding with itself
            canChangeDirection = 0;

        if(clock() >= endWait) { // It moves according to how fast the computer running it is...
            move(snakeXY, snakeLength, direction);
            canChangeDirection = 1;

            if(eatFood(snakeXY, foodXY)) {
                generateFood(foodXY, consoleWidth, consoleHeight, snakeXY, snakeLength); // Generate More Food
                snakeLength++;
                score += 10;

                if(score >= 10*speed+tempScore) {
                    speed++;
                    tempScore = score;

                    if(speed <= 9) // This needs to be fixed
                        waitMili = waitMili - (CLOCKS_PER_SEC/10);
                    else {
                        if(waitMili >= 40) // Maximum Speed (the game has to be beatable)
                            waitMili = waitMili - (CLOCKS_PER_SEC/200);
                    }
                }

                refreshInfoBar(score, speed);
            }

            endWait = clock() + waitMili; // TEMP FIX, NEED TO FIND A WAY TO RESET CLOCK().. Na, seems to work fine this way...
        }

        gameOver = collisionDetection(snakeXY, consoleWidth, consoleHeight, snakeLength);

        if(snakeLength >= SNAKE_ARRAY_SIZE-5) { // Just to make sure it doesn't get longer then the array size & crash
            gameOver = 2; // You Win! <- doesn't seem to work - NEED TO FIX/TEST THIS
            score += 1500; // When you win you get an extra 1500 points!!!
        }

    } while (!gameOver);

    switch(gameOver) {
        case 1:
            gameOverScreen();
            break;
    }

    return;
}

void loadGame(void)
{
    int snakeXY[2][SNAKE_ARRAY_SIZE]; // Two Dimensional Array, the first array is for the X coordinates and the second array for the Y coordinates

    int snakeLength = 2; // Starting Length

    int direction = LEFT_ARROW; // DO NOT CHANGE THIS TO RIGHT ARROW, THE GAME WILL INSTANTLY BE OVER IF YOU DO!!! <- Unless the prepareSnakeArray function is changed to take into account the direction....

    int foodXY[] = {20,5}; // Stores the location of the food

    int score = 0;
    //int level = 1;

    //Window Width * Height - at some point find a way to get the actual dimensions of the console... <- Also somethings that display don't take this dimensions into account.. need to fix this...
    int consoleWidth = 128;
    int consoleHeight = 7;

    int speed = getGameSpeed();

    //The starting location of the snake
    snakeXY[0][0] = 20;
    snakeXY[1][0] = 0;

    //loadEnvironment(consoleWidth, consoleHeight); // borders
    prepairSnakeArray(snakeXY, snakeLength);
    loadSnake(snakeXY, snakeLength);
    generateFood(foodXY, consoleWidth, consoleHeight, snakeXY, snakeLength);
    refreshInfoBar(score, speed); // Bottom info bar. Score, Level etc
    startGame(snakeXY, foodXY, consoleWidth, consoleHeight, snakeLength, direction, score, speed);

    return;
}

int main()
{
    int c;

    fd = open(SSD1306_DEV_FILE, O_WRONLY);

    if (fd == -1)
    {
        printf("Open /dev/my_ssd1306 failed. Please check the dev folder\n");
        exit(EXIT_FAILURE);
    }

    system("clear");  
    write(fd, "clear", 5);
   
    do {
        loadGame();
        write(fd, "clear", 5);
        set_cursor_to_LCD(25, 2);
        Display_to_LCD("PLAY AGAIN ?");
        set_cursor_to_LCD(5, 3);
        Display_to_LCD("1. YES   Other. NO");
        scanf("%d", &c);
        if (c == 1)
        {
            write(fd, "clear", 5);
        }
    } while (c == 1);
   
    write(fd, "clear", 5);
    set_cursor_to_LCD(30, 3);
    Display_to_LCD(" END GAME !");
   
    return 0;
}
