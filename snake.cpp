
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <vector>
#include <ctime>

using namespace std;

#include <fstream>

enum eGameState { MAIN_MENU, PLAYING, GAME_OVER, SCORE_BOARD };
eGameState currentState;

bool gameOver;
const int width = 50;
const int height = 30;
int x, y, fruitX, fruitY, score;
vector<int> tailX, tailY;
int nTail;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

#include <algorithm>

const char* SCORE_FILE = "scores.txt";

void SaveScore(int score) {
    ofstream outfile(SCORE_FILE, ios::app);
    if (outfile.is_open()) {
        outfile << score << endl;
        outfile.close();
    }
}

vector<int> LoadScores() {
    vector<int> scores;
    ifstream infile(SCORE_FILE);
    int score;
    if (infile.is_open()) {
        while (infile >> score) {
            scores.push_back(score);
        }
        infile.close();
        sort(scores.rbegin(), scores.rend()); // Sort descending
    }
    return scores;
}

void HideCursor();

void Setup() {
    HideCursor();
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;
    tailX.clear();
    tailY.clear();
    srand(time(0)); // Seed for random number generation
}

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void HideCursor() {
    CONSOLE_CURSOR_INFO cursor_info = {1, 0};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void Draw() {
    gotoxy(0, 0);
    SetColor(8); // Grey for walls
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0) {
                SetColor(8);
                cout << "#";
            }
            if (i == y && j == x) {
                SetColor(10); // Bright Green for snake head
                cout << "O";
            } else if (i == fruitY && j == fruitX) {
                SetColor(12); // Red for fruit
                cout << "F";
            } else {
                bool print = false;
                for (int k = 0; k < tailX.size(); k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        SetColor(2); // Green for snake tail
                        cout << "o";
                        print = true;
                    }
                }
                if (!print)
                    cout << " ";
            }

            if (j == width - 1) {
                SetColor(8);
                cout << "#";
            }
        }
        cout << endl;
    }

    SetColor(8);
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;
    SetColor(7); // Reset to default white
    cout << "Score:" << score << endl;
}

void Input() {
    if (_kbhit()) {
        switch (_getch()) {
        case 'a':
            if (dir != RIGHT) dir = LEFT;
            break;
        case 'd':
            if (dir != LEFT) dir = RIGHT;
            break;
        case 'w':
            if (dir != DOWN) dir = UP;
            break;
        case 's':
            if (dir != UP) dir = DOWN;
            break;
        case 'p': // Pause
            gotoxy(width / 2 - 4, height / 2);
            SetColor(14);
            cout << "PAUSED";
            _getch();
            break;
        case 'x':
            gameOver = true;
            break;
        }
    }
}

void Logic() {
    vector<int> prevTailX = tailX;
    vector<int> prevTailY = tailY;

    if (nTail > 0) {
        if (tailX.empty()) {
            tailX.push_back(x);
            tailY.push_back(y);
        } else {
            tailX.insert(tailX.begin(), x);
            tailY.insert(tailY.begin(), y);
            if(tailX.size() > nTail) {
                tailX.pop_back();
                tailY.pop_back();
            }
        }
    }

    switch (dir) {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }

    if (x >= width || x < 0 || y >= height || y < 0) {
        gameOver = true;
    }

    for (int i = 0; i < tailX.size(); i++) {
        if (tailX[i] == x && tailY[i] == y) {
            gameOver = true;
        }
    }

    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
    }
}

void ShowScoreBoard() {
    system("cls");
    SetColor(14); // Yellow
    gotoxy(width / 2 - 5, 3);
    cout << "SCORE BOARD" << endl;

    vector<int> scores = LoadScores();
    if (scores.empty()) {
        SetColor(7);
        gotoxy(width / 2 - 10, 5);
        cout << "No scores recorded yet." << endl;
    } else {
        SetColor(11); // Light Cyan
        gotoxy(width / 2 - 8, 5);
        cout << "Highest Score: " << scores[0] << endl;

        SetColor(7);
        gotoxy(width / 2 - 8, 7);
        cout << "Recent Scores:" << endl;
        int count = 0;
        for (int s : scores) {
            if (count < 5) { // Show top 5
                gotoxy(width / 2 - 8, 8 + count);
                cout << count + 1 << ". " << s << endl;
                count++;
            }
        }
    }

    SetColor(7);
    gotoxy(width / 2 - 12, height - 2);
    cout << "(b) Back to menu (q) Quit" << endl;
    char choice = _getch();
    if (choice == 'q' || choice == 'Q') {
        currentState = SCORE_BOARD; // Exit signal
    } else {
        currentState = MAIN_MENU;
    }
}

void ShowMainMenu() {
    system("cls");
    SetColor(11); // Light Cyan
    gotoxy(width / 2 - 4, 5);
    cout << "SNAKE" << endl;

    SetColor(7);
    gotoxy(width / 2 - 8, 8);
    cout << "1. Start Game" << endl;
    gotoxy(width / 2 - 8, 9);
    cout << "2. View Scores" << endl;
    gotoxy(width / 2 - 8, 10);
    cout << "3. Exit" << endl;

    char choice = _getch();
    switch (choice) {
    case '1':
        currentState = PLAYING;
        break;
    case '2':
        ShowScoreBoard();
        break;
    case '3':
        currentState = SCORE_BOARD; // Exit state
        break;
    default:
        break;
    }
}

void ShowGameOverScreen() {
    SaveScore(score);
    system("cls");
    SetColor(12); // Red
    gotoxy(width / 2 - 5, 5);
    cout << "GAME OVER" << endl;

    SetColor(7);
    gotoxy(width / 2 - 8, 8);
    cout << "Your Score: " << score << endl;

    vector<int> scores = LoadScores();
    if (!scores.empty()) {
        SetColor(14); // Yellow
        gotoxy(width / 2 - 8, 9);
        cout << "Highest Score: " << scores[0] << endl;
    }

    SetColor(7);
    gotoxy(width / 2 - 12, 12);
    cout << "Press any key to continue..." << endl;
    _getch();
    currentState = MAIN_MENU;
}

int main() {
    HideCursor();
    currentState = MAIN_MENU;

    while (currentState != SCORE_BOARD) { // SCORE_BOARD will be the exit state
        switch (currentState) {
        case MAIN_MENU:
            ShowMainMenu();
            break;
        case PLAYING:
            Setup();
            while (!gameOver) {
                Draw();
                Input();
                Logic();

                int speed = 150 - (score / 20) * 10;
                if (speed < 50) speed = 50;
                Sleep(speed);
            }
            currentState = GAME_OVER;
            break;
        case GAME_OVER:
            ShowGameOverScreen();
            break;
        case SCORE_BOARD:
            // This state is now used as the exit condition for the main loop
            break;
        }
    }

    return 0;
}
