// --------------------------------------------------------
//    file: snakegame.cpp
//  author: TB
//   class: COP 2001
// purpose: using SFML libraries to create snake game
// --------------------------------------------------------

#include <cstdlib>
#include <SFML/Graphics.hpp>

// --------------------------------------------------------
// global constants
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 1200;
const sf::Color WINDOW_COLOR = sf::Color::White;
const float FRAME_RATE = (1.0/30.0) * 4000.0;

const sf::Color BORDER_COLOR = sf::Color::Blue;
const int BORDER_THICKNESS = 30;

const sf::Color SNAKE_COLOR = sf::Color::Green;
const int SNAKE_THICKNESS = 60;

const sf::Color FOOD_COLOR = sf::Color::Red;
const int FOOD_RADIUS = 10;


// --------------------------------------------------------
// structures for game objects

// structure for the window borders
struct Border{
    sf::RectangleShape topBorder;
    sf::RectangleShape bottomBorder;
    sf::RectangleShape leftBorder;
    sf::RectangleShape rightBorder;
};

// structure for the food
struct Food {
    int radius;
    int xPosition;
    int yPosition;
    sf::Color color;
};


// --------------------------------------------------------
// enum for directions
enum direction {
    Exit = -1,
    None = 0,
    Left = 1,
    Up = 2,
    Right = 3,
    Down = 4,
    Start
};


// --------------------------------------------------------
// function definitions

void objectsSetup(Border &borders,
                  sf::RectangleShape snakeArray[100],
                  int &snakeLength, Food &food);
direction userInput();
void update(direction &input, Border &borders,
            sf::RectangleShape snakeArray[100],
            int &snakeLength, Food &food, bool &started,
            float &xChange, float &yChange, bool &gameOver);
void draw(sf::RenderWindow &gameWindow, Border &borders,
          sf::RectangleShape snakeArray[100],
          int &snakeLength, Food &food);
void foodEaten(sf::RectangleShape snakeArray[100], Food &food,
               int &snakeLength);
void collisionCheck(Border &borders,
                    sf::RectangleShape snakeArray[100],
                    int &snakeLength, bool &gameOver);


// --------------------------------------------------------
int main() {


    // defining gameWindow as and AnimatedWindow
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH,WINDOW_HEIGHT),"Snake Game");
    window.clear(WINDOW_COLOR);



    // initializing the objects in the game
    Border borders;
    sf::RectangleShape snakeArray[100];
    int snakeLength = 3;
    float xChange = 0.0;
    float yChange = 0.0;
    Food food;


    // setting up the objects in the game
    objectsSetup(borders, snakeArray, snakeLength, food);


    // timing variables for the main game loop
    sf::Clock clock;
    sf::Time startTime = clock.getElapsedTime();
    sf::Time stopTime = startTime;
    float delta = 0.0;


    // variables to control the games start and finish
    bool started = false;
    bool gameOver = false;
    while (!gameOver){


        // calculate frame time
        stopTime = clock.getElapsedTime();
        delta += (stopTime.asMilliseconds() - startTime.asMilliseconds());
        startTime = stopTime;


        // event processing
        sf::Event event;
        while(window.pollEvent(event)){
            // closing the window
            if (event.type == sf::Event::Closed)
                gameOver = true;
        } // end while(gameWindow.pollEvent(event))


        // processing user input
        direction input = userInput();
        if (input == Exit){
            gameOver = true;
        }


        // if it has been at least a full frame time
        if (delta >= FRAME_RATE) {

            // update the game properties
            update(input, borders, snakeArray, snakeLength, food,
                   started, xChange, yChange, gameOver);

            // subtract the frame-rate from the current frame-time
            // for each full frame covered by this update
            while (delta >= FRAME_RATE)
                delta -= FRAME_RATE;
        }


        // draw the window and game elements
        draw(window, borders, snakeArray, snakeLength, food);

    } // end while(!gameover)


    // game has ended, close the graphics window
    window.close();


    return 0;
} // end main

// --------------------------------------------------------
// function definitions

/**
 * the original set up of the game properties
 * @param borders - the borders of the game window
 * @param snakeArray - an array that holds the snake's body parts,
 *                     with the first index [0] being the snake's head
 * @param snakeLength - the current length of the snake
 * @param food - the food that the snake will eat
 */
void objectsSetup(Border &borders,
                  sf::RectangleShape snakeArray[100],
                  int &snakeLength, Food &food){

    // properties of the top border
    borders.topBorder.setSize(sf::Vector2f(WINDOW_WIDTH,
                              BORDER_THICKNESS));
    borders.topBorder.setPosition(0,0);
    borders.topBorder.setFillColor(BORDER_COLOR);


    // properties of the bottom border
    borders.bottomBorder.setSize(sf::Vector2f(WINDOW_WIDTH,
                                 BORDER_THICKNESS));
    borders.bottomBorder.setPosition(0,WINDOW_HEIGHT - BORDER_THICKNESS);
    borders.bottomBorder.setFillColor(BORDER_COLOR);


    // properties of the left border
    borders.leftBorder.setSize(sf::Vector2f(BORDER_THICKNESS,
                                            WINDOW_HEIGHT));
    borders.leftBorder.setPosition(0,0);
    borders.leftBorder.setFillColor(BORDER_COLOR);


    // properties of the right border
    borders.rightBorder.setSize(sf::Vector2f(BORDER_THICKNESS,
                                WINDOW_HEIGHT));
    borders.rightBorder.setPosition(WINDOW_WIDTH - BORDER_THICKNESS,0);
    borders.rightBorder.setFillColor(BORDER_COLOR);


    // initial properties of the snake head
    snakeArray[0].setPosition(420, 420);
    snakeArray[0].setSize(sf::Vector2f(SNAKE_THICKNESS,
                                       SNAKE_THICKNESS));
    snakeArray[0].setFillColor(sf::Color::Yellow);


    // initial properties of second and third part of snake
    for(int x = 1; x < snakeLength; x++){
        snakeArray[x].setPosition(420, 420 + (x*60));
        snakeArray[x].setSize(sf::Vector2f(SNAKE_THICKNESS,
                              SNAKE_THICKNESS));
        snakeArray[x].setFillColor(SNAKE_COLOR);
    }


    // initial properties of the food
    sf::CircleShape snakeFood;
    srand(time(0));
    food.xPosition = rand() % 870;
    food.yPosition = rand() % 870;

    // if the food is on, or very close to, one of the borders,
    // pick a new x or y spot
    if(food.xPosition <= BORDER_THICKNESS + 30
       || food.xPosition >= WINDOW_WIDTH - BORDER_THICKNESS - 30){
        food.xPosition = rand() % 870;
    }
    if(food.yPosition <= BORDER_THICKNESS + 30
       || food.yPosition >= WINDOW_HEIGHT - BORDER_THICKNESS - 30){
        food.yPosition = rand() % 870;
    }

    food.radius = FOOD_RADIUS;
    snakeFood.setRadius(FOOD_RADIUS);
    snakeFood.setPosition(food.xPosition, food.yPosition);
    snakeFood.setFillColor(FOOD_COLOR);


} // end of objectsSetup


/**
 * convert keyboard input into integer values
 * for left=1/up=2/right=3/down=4
 * @return int - user input (default no-input=0, quit=-1)
 */
direction userInput() {

    direction input = None;  // no input

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        input = Left;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        input = Up;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        input = Right;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        input = Down;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        input = Left;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        input = Up;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        input = Right;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        input = Down;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
        input = Start;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
        input = Exit;
    }

    return input;
} // end userInput


/**
 * updating the positions of the objects, and chekcing for collisions
 * @param input - the keyboard input from the user
 * @param borders - the borders of the game window
 * @param snakeArray - an array that holds the snake's body parts,
 *                     with the first index [0] being the snake's head
 * @param snakeLength - the current length of the snake
 * @param food - the food that the snake will eat
 * @param started - if the game has started yet
 * @param xChange - the change in x position based on the input
 * @param yChange - the change in y position based on the input
 * @param gameOver - if the game is over
 */
void update(direction &input, Border &borders,
            sf::RectangleShape snakeArray[100],
            int &snakeLength, Food &food, bool &started,
            float &xChange, float &yChange, bool &gameOver){

    // adjust positions for user input
    switch(input){
        case Up: // Up
            xChange = 0;
            yChange = -SNAKE_THICKNESS;
            started = true;
            break;
        case Down: // Down
            xChange = 0;
            yChange = SNAKE_THICKNESS;
            started = true;
            break;
        case Left: // Left
            xChange = -SNAKE_THICKNESS;
            yChange = 0;
            started = true;
            break;
        case Right: // Right
            xChange = SNAKE_THICKNESS;
            yChange = 0;
            started = true;
            break;
        case Exit: // Exit game
            gameOver = true;
    } // end switch


    if(started){
        // temporary storage variable for the previous body part
        float oldBodyX, oldBodyY;

        // updating the snake body's locations tail to head
        for(int i = snakeLength - 1; i > 0; i--){
            oldBodyX = snakeArray[i-1].getPosition().x;
            oldBodyY = snakeArray[i-1].getPosition().y;
            snakeArray[i].setPosition(oldBodyX, oldBodyY);
        } // end for loop

        // moving the snake head's position
        snakeArray[0].move(xChange, yChange);
    } // end if(started)


    // check if food is eaten
    foodEaten(snakeArray, food, snakeLength);


    // check for collisions
    collisionCheck(borders, snakeArray, snakeLength, gameOver);


} // end update


/**
 * drawing the game objects
 * @param gameWindow - the graphics window
 * @param borders - the borders of the game window
 * @param snakeArray - an array that holds the snake's body parts,
 *                     with the first index [0] being the snake's head
 * @param snakeLength - the current length of the snake
 * @param food - the food that the snake will eat
 */
void draw(sf::RenderWindow &gameWindow, Border &borders,
          sf::RectangleShape snakeArray[100],
          int &snakeLength, Food &food){


    // clearing window and setting it to the window's color
    gameWindow.clear(WINDOW_COLOR);


    // drawing the top border
    sf::RectangleShape topBorderDraw;
    topBorderDraw.setSize(sf::Vector2f(WINDOW_WIDTH,
                                       BORDER_THICKNESS));
    topBorderDraw.setPosition(0,0);
    topBorderDraw.setFillColor(BORDER_COLOR);
    gameWindow.draw(topBorderDraw);


    // drawing the bottom border
    sf::RectangleShape bottomBorderDraw;
    bottomBorderDraw.setSize(sf::Vector2f(WINDOW_WIDTH,
                                          BORDER_THICKNESS));
    bottomBorderDraw.setPosition(0,WINDOW_HEIGHT - BORDER_THICKNESS);
    bottomBorderDraw.setFillColor(BORDER_COLOR);
    gameWindow.draw(bottomBorderDraw);


    // drawing the left border
    sf::RectangleShape leftBorderDraw;
    leftBorderDraw.setSize(sf::Vector2f(BORDER_THICKNESS,
                                        WINDOW_HEIGHT));
    leftBorderDraw.setPosition(0,0);
    leftBorderDraw.setFillColor(BORDER_COLOR);
    gameWindow.draw(leftBorderDraw);


    // drawing the right border
    sf::RectangleShape rightBorderDraw;
    rightBorderDraw.setSize(sf::Vector2f(BORDER_THICKNESS,
                                         WINDOW_HEIGHT));
    rightBorderDraw.setPosition(WINDOW_WIDTH - BORDER_THICKNESS,0);
    rightBorderDraw.setFillColor(BORDER_COLOR);
    gameWindow.draw(rightBorderDraw);


    // drawing the snake
    gameWindow.draw(snakeArray[0]);
    for (int d = 1; d < snakeLength; d++){
        snakeArray[d].setSize(sf::Vector2f(SNAKE_THICKNESS,
                                           SNAKE_THICKNESS));
        snakeArray[d].setFillColor(SNAKE_COLOR);
        gameWindow.draw(snakeArray[d]);
    }


    // drawing the food
    sf::CircleShape snakeFoodDraw;
    snakeFoodDraw.setRadius(FOOD_RADIUS);
    snakeFoodDraw.setPosition(food.xPosition, food.yPosition);
    snakeFoodDraw.setFillColor(FOOD_COLOR);
    gameWindow.draw(snakeFoodDraw);


    // display game window
    gameWindow.display();

} // end render


/**
 * checking if the snake has 'eaten' the food
 * @param snakeArray - an array that holds the snake's body parts,
 *                     with the first index [0] being the snake's head
 * @param food - the food that the snake will eat
 * @param snakeLength - the current length of the snake
 */
void foodEaten(sf::RectangleShape snakeArray[100], Food &food,
               int &snakeLength){


    bool foodIsEaten = false;


    // sides of snake's head
    float snakeTop;
    float snakeBottom;
    float snakeLeft;
    float snakeRight;

    snakeTop = snakeArray[0].getPosition().y;
    snakeBottom = snakeArray[0].getPosition().y + SNAKE_THICKNESS;
    snakeLeft = snakeArray[0].getPosition().x;
    snakeRight = snakeArray[0].getPosition().x + SNAKE_THICKNESS;


    // checking if the snake's head has 'eaten' the food
    if((food.yPosition > snakeTop) && (food.yPosition < snakeBottom)
       && (food.xPosition > snakeLeft) && (food.xPosition < snakeRight)){
        foodIsEaten = true;
    } // end if


    // if the snake has 'eaten' the food, move the food to a new
    // spot and make the snake one body part longer
    if(foodIsEaten){

        srand(time(0));
        food.xPosition = rand() % 870;
        food.yPosition = rand() % 870;

        // if the food is on, or very close to, one of the borders,
        // pick a new x or y spot
        if(food.xPosition <= BORDER_THICKNESS + 30
           || food.xPosition >= WINDOW_WIDTH - BORDER_THICKNESS - 30){
            food.xPosition = rand() % 870;
        }
        if(food.yPosition <= BORDER_THICKNESS + 30
           || food.yPosition >= WINDOW_HEIGHT - BORDER_THICKNESS - 30){
            food.yPosition = rand() % 870;
        }

        snakeLength++;

    } // end if(foodIsEaten)

} // end foodEaten


/**
 * checking if the snake's head had collided with the borders or
 * any other part of the snake
 * @param borders - the borders of the game window
 * @param snakeArray - an array that holds the snake's body parts,
 *                     with the first index [0] being the snake's head
 * @param snakeLength - the current length of the snake
 * @param gameOver - if the game is over
 */
void collisionCheck(Border &borders,
                    sf::RectangleShape snakeArray[100],
                    int &snakeLength, bool &gameOver){


    // the sides of the borders
    float topBorder;
    float bottomBorder;
    float leftBorder;
    float rightBorder;

    topBorder = BORDER_THICKNESS;
    bottomBorder = WINDOW_HEIGHT - BORDER_THICKNESS;
    leftBorder = BORDER_THICKNESS;
    rightBorder = WINDOW_WIDTH - BORDER_THICKNESS;


    // the sides of snake's head
    float snakeTop;
    float snakeBottom;
    float snakeLeft;
    float snakeRight;

    snakeTop = snakeArray[0].getPosition().y;
    snakeBottom = snakeArray[0].getPosition().y + SNAKE_THICKNESS;
    snakeLeft = snakeArray[0].getPosition().x;
    snakeRight = snakeArray[0].getPosition().x + SNAKE_THICKNESS;


    // check collision with walls
    if((snakeTop < BORDER_THICKNESS)
       || (snakeBottom > WINDOW_HEIGHT - BORDER_THICKNESS)
       || (snakeLeft < BORDER_THICKNESS)
       || (snakeRight > WINDOW_WIDTH - BORDER_THICKNESS)){

        gameOver = true;
    } // end if


    // check collision with rest of snake
    for(int c = 1; c < snakeLength; c++){

        if(snakeArray[c].getPosition()
           == snakeArray[0].getPosition()){
            gameOver = true;
        } // end if

    } // end for


} // end collisionCheck
