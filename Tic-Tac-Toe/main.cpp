#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

using namespace std;

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 700;

char board[3][3] = {
    {' ', ' ', ' '},
    {' ', ' ', ' '},
    {' ', ' ', ' '}
};

char currentPlayer = 'X';

int xScore = 0;
int oScore = 0;
int drawScore = 0;

bool gameOver = false;
string result = "";


// ---------------- RESET BOARD ----------------

void resetBoard()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board[i][j] = ' ';
        }
    }

    currentPlayer = 'X';
    gameOver = false;
    result = "";
}


// ---------------- CHECK DRAW ----------------

bool isDraw()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] == ' ')
            {
                return false;
            }
        }
    }

    return true;
}


// ---------------- CHECK WINNER ----------------

bool checkWinner(char player)
{
    // Rows
    for (int i = 0; i < 3; i++)
    {
        if (board[i][0] == player &&
            board[i][1] == player &&
            board[i][2] == player)
        {
            return true;
        }
    }

    // Columns
    for (int i = 0; i < 3; i++)
    {
        if (board[0][i] == player &&
            board[1][i] == player &&
            board[2][i] == player)
        {
            return true;
        }
    }

    // Main diagonal
    if (board[0][0] == player &&
        board[1][1] == player &&
        board[2][2] == player)
    {
        return true;
    }

    // Other diagonal
    if (board[0][2] == player &&
        board[1][1] == player &&
        board[2][0] == player)
    {
        return true;
    }

    return false;
}


// ---------------- DRAW X ----------------
void drawX(sf::RenderWindow& window,
           float x,
           float y,
           float size)
{
    sf::RectangleShape line1(
        sf::Vector2f(size - 40.f, 5.f)
    );

    line1.setFillColor(sf::Color::Black);

    line1.setPosition(
        sf::Vector2f(x + 20.f, y + 20.f)
    );

    line1.setRotation(sf::degrees(45.f));

    window.draw(line1);


    sf::RectangleShape line2(
        sf::Vector2f(size - 40.f, 5.f)
    );

    line2.setFillColor(sf::Color::Black);

    line2.setPosition(
        sf::Vector2f(x + size - 20.f, y + 20.f)
    );

    line2.setRotation(sf::degrees(135.f));

    window.draw(line2);
}


// ---------------- DRAW O ----------------

void drawO(sf::RenderWindow& window,
           float x,
           float y,
           float size)
{
    sf::CircleShape circle(65.f);

    circle.setPosition(
        sf::Vector2f(
            x + (size - 130.f) / 2.f,
            y + (size - 130.f) / 2.f
        )
    );

    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineColor(sf::Color::Black);
    circle.setOutlineThickness(8.f);

    window.draw(circle);
}


// ---------------- MAIN ----------------

int main()
{
    // SFML 3 syntax
    sf::RenderWindow window(
        sf::VideoMode(
            sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)
        ),
        "Tic-Tac-Toe"
    );

    window.setFramerateLimit(60);


    // ---------------- FONT ----------------

    sf::Font font;

    if (!font.openFromFile("arial.ttf"))
    {
        cout << "Error: arial.ttf not found!" << endl;
        return 1;
    }


    // ---------------- BOARD SETTINGS ----------------

    float boardSize = 450.f;
    float cellSize = boardSize / 3.f;

    float startX = 75.f;
    float startY = 150.f;


    // ---------------- GAME LOOP ----------------

    while (window.isOpen())
    {
        // SFML 3 event system
        while (auto event = window.pollEvent())
        {
            // Window close
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }


            // Mouse click
            if (const auto* mouse =
                    event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse->button != sf::Mouse::Button::Left)
                {
                    continue;
                }

                float mouseX =
                    static_cast<float>(mouse->position.x);

                float mouseY =
                    static_cast<float>(mouse->position.y);


                // NEW GAME BUTTON
                if (mouseX >= 200.f &&
                    mouseX <= 400.f &&
                    mouseY >= 625.f &&
                    mouseY <= 675.f)
                {
                    resetBoard();
                    continue;
                }


                // Don't allow moves after game ends
                if (gameOver)
                {
                    continue;
                }


                // Check if click is inside board
                if (mouseX >= startX &&
                    mouseX <= startX + boardSize &&
                    mouseY >= startY &&
                    mouseY <= startY + boardSize)
                {
                    int col =
                        static_cast<int>(
                            (mouseX - startX) / cellSize
                        );

                    int row =
                        static_cast<int>(
                            (mouseY - startY) / cellSize
                        );


                    // Valid empty cell
                    if (board[row][col] == ' ')
                    {
                        board[row][col] = currentPlayer;


                        // CHECK WIN
                        if (checkWinner(currentPlayer))
                        {
                            gameOver = true;

                            if (currentPlayer == 'X')
                            {
                                xScore++;
                            }
                            else
                            {
                                oScore++;
                            }

                            result = "Player ";
                            result += currentPlayer;
                            result += " Wins!";
                        }


                        // CHECK DRAW
                        else if (isDraw())
                        {
                            gameOver = true;

                            drawScore++;

                            result = "It's a Draw!";
                        }


                        // CHANGE PLAYER
                        else
                        {
                            if (currentPlayer == 'X')
                            {
                                currentPlayer = 'O';
                            }
                            else
                            {
                                currentPlayer = 'X';
                            }
                        }
                    }
                }
            }
        }


        // ---------------- BACKGROUND ----------------

        window.clear(
            sf::Color(245, 245, 245)
        );


        // ---------------- TITLE ----------------

        sf::Text title(
            font,
            "TIC-TAC-TOE",
            38
        );

        title.setFillColor(
            sf::Color::Black
        );

        title.setStyle(
            sf::Text::Bold
        );

        sf::FloatRect titleBounds =
            title.getLocalBounds();

        title.setPosition(
            sf::Vector2f(
                (WINDOW_WIDTH -
                 titleBounds.size.x) / 2.f
                 - titleBounds.position.x,

                20.f
            )
        );

        window.draw(title);


        // ---------------- STATUS ----------------

        sf::Text status(
            font,
            "",
            24
        );

        status.setFillColor(
            sf::Color::Black
        );


        if (!gameOver)
        {
            string text = "Player ";
            text += currentPlayer;
            text += "'s Turn";

            status.setString(text);
        }
        else
        {
            status.setString(result);
        }


        sf::FloatRect statusBounds =
            status.getLocalBounds();

        status.setPosition(
            sf::Vector2f(
                (WINDOW_WIDTH -
                 statusBounds.size.x) / 2.f
                 - statusBounds.position.x,

                90.f
            )
        );

        window.draw(status);


        // ---------------- HORIZONTAL LINES ----------------

        sf::RectangleShape horizontalLine(
            sf::Vector2f(boardSize, 5.f)
        );

        horizontalLine.setFillColor(
            sf::Color::Black
        );


        for (int i = 1; i < 3; i++)
        {
            horizontalLine.setPosition(
                sf::Vector2f(
                    startX,
                    startY + i * cellSize
                )
            );

            window.draw(horizontalLine);
        }


        // ---------------- VERTICAL LINES ----------------

        sf::RectangleShape verticalLine(
            sf::Vector2f(5.f, boardSize)
        );

        verticalLine.setFillColor(
            sf::Color::Black
        );


        for (int i = 1; i < 3; i++)
        {
            verticalLine.setPosition(
                sf::Vector2f(
                    startX + i * cellSize,
                    startY
                )
            );

            window.draw(verticalLine);
        }


        // ---------------- DRAW X AND O ----------------

        for (int row = 0; row < 3; row++)
        {
            for (int col = 0; col < 3; col++)
            {
                float x =
                    startX + col * cellSize;

                float y =
                    startY + row * cellSize;


                if (board[row][col] == 'X')
                {
                    drawX(
                        window,
                        x,
                        y,
                        cellSize
                    );
                }
                else if (board[row][col] == 'O')
                {
                    drawO(
                        window,
                        x,
                        y,
                        cellSize
                    );
                }
            }
        }


        // ---------------- SCORE ----------------

        sf::Text score(
            font,
            "",
            20
        );

        score.setFillColor(
            sf::Color::Black
        );


        string scoreText =
            "X: " + to_string(xScore) +
            "     O: " + to_string(oScore) +
            "     Draw: " + to_string(drawScore);

        score.setString(scoreText);


        sf::FloatRect scoreBounds =
            score.getLocalBounds();

        score.setPosition(
            sf::Vector2f(
                (WINDOW_WIDTH -
                 scoreBounds.size.x) / 2.f
                 - scoreBounds.position.x,

                580.f
            )
        );

        window.draw(score);


        // ---------------- NEW GAME BUTTON ----------------

        sf::RectangleShape button(
            sf::Vector2f(200.f, 50.f)
        );

        button.setPosition(
            sf::Vector2f(200.f, 625.f)
        );

        button.setFillColor(
            sf::Color(50, 50, 50)
        );

        window.draw(button);


        // Button text
        sf::Text buttonText(
            font,
            "NEW GAME",
            20
        );

        buttonText.setFillColor(
            sf::Color::White
        );


        sf::FloatRect buttonBounds =
            buttonText.getLocalBounds();

        buttonText.setPosition(
            sf::Vector2f(
                200.f +
                (200.f -
                 buttonBounds.size.x) / 2.f
                 - buttonBounds.position.x,

                625.f +
                (50.f -
                 buttonBounds.size.y) / 2.f
                 - buttonBounds.position.y
                 - 3.f
            )
        );

        window.draw(buttonText);


        // ---------------- DISPLAY ----------------

        window.display();
    }

    return 0;
}