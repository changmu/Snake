#include "snake.h"

const char *SQUARE = "¡ö";
const char *SQUAREHEAD = "¡î";
const char *STAR = "¡ï";
const char *SPACE = "  ";
const char *LINE[] = {"¨T", "¨U", "¨T", "¨U"};
const char *CORNER[] = {"¨X", "¨[", "¨a", "¨^", "  "};
const char *TIPS[] = {"ÉÏ:<Up>", "ÏÂ:<Down>", "×ó:<Left>", "ÓÒ:<Right>",
                      "ÔÝÍ£:<Space>", "ÍË³ö:<Esc>", "Score:", "@changmu", "ver. 0.1"};

enum {
    Up = 72, Right = 77, Down = 80, Left = 75, 
    star_sign = 99, Space = 32, Esc = 27,        // keyboard button
    TRUE_ = 1
};

const int dir_sign = -32, FALSE_ = 0;
int interval_time = 300;
int mainBoard[22][22];          // on logic
int score;
Point head, tail, star, score_pos;
char buf[20];                   // save score

void game_init() {              // draw framework
    HideCursor();
    int i, j, h = 22, w = 22;
    Point pos;

    for (i = 0; i < h; ++i)
        for (j = 0; j < w; ++j) {
            pos.x = j;
            pos.y = i;

            if (i == 0) {
                if (j == 0)
                    print_at(CORNER[0], pos);
                else if (j == w - 1) 
                    print_at(CORNER[1], pos);
                else 
                    print_at(LINE[0], pos);
            } else if (i == h - 1) {
                if (j == 0)
                    print_at(CORNER[3], pos);
                else if (j == w - 1)
                    print_at(CORNER[2], pos);
                else 
                    print_at(LINE[2], pos);
            } else {
                if (j == 0) 
                    print_at(LINE[3], pos);
                else if (j == w - 1) 
                    print_at(LINE[1], pos);
            }
        }

    pos.x = 22; 
    pos.y = 1;
    print_at(TIPS[6], pos);

    ++pos.y;
    score_pos = pos;
    print_at(itoa(score = 0, buf, 10), score_pos);

    ++pos.y;
    ++pos.y;
    print_at("°ïÖú:", pos);

    for (i = 0; i < 6; ++i) {
        ++pos.y;
        ++pos.y;
        print_at(TIPS[i], pos);
    }

    pos.y = 21;
    print_at(TIPS[8], pos);
    pos.y = 20;
    print_at(TIPS[7], pos);

    head.x = 2;                 // put on head node
    head.y = 1;
    mainBoard[head.x][head.y] = Right;
    print_at(SQUAREHEAD, head);

    tail.x = 1;
    tail.y = 1;
    mainBoard[tail.x][tail.y] = Right;
    print_at(SQUARE, tail);

    /* put on star */
    srand(time(NULL));
    appear_star();
}

int game_move() {
    Sleep(interval_time);
    char ch;

    if (kbhit()) {
        ch = getch();
        if (ch == dir_sign) {
            ch = getch();
            if (ch + mainBoard[head.x][head.y] != 152)
                mainBoard[head.x][head.y] = ch;
        } else if (ch == Space) {
            getch();
        } else if (ch == Esc) {
            exit(0);
        }
    }

    if (move_head())
        return TRUE_;
    else 
        return FALSE_;
}

void game_end() {
    Point pos = {1, 10};

    print_at("Game Over! your score is: ", pos);
    printf("%d", score);

    while (getch() != Esc);
}

void adjust_interval_time() {
    interval_time *= 0.9;
}

int move_head() {
    int pre_dir = mainBoard[head.x][head.y];
    print_at(SQUARE, head);
    move_point(&head, pre_dir);
    if (check_pos(head.x, head.y) == FALSE_)
        return FALSE_;

    print_at(SQUAREHEAD, head);         // move head

    if (mainBoard[head.x][head.y] == star_sign) {
        appear_star();
        print_at(itoa(++score, buf, 10), score_pos);
        adjust_interval_time();
    } else {
        move_tail();
    }
    mainBoard[head.x][head.y] = pre_dir;

    return TRUE_;
}

void move_tail() {
    print_at(SPACE, tail);          // move tail
    int pre_dir = mainBoard[tail.x][tail.y];
    mainBoard[tail.x][tail.y] = 0;
    move_point(&tail, pre_dir);
}

int check_pos(int x, int y) {
    if (x < 1 || x > 20 || y < 1 || y > 20 ||
            mainBoard[x][y] && mainBoard[x][y] != star_sign)
        return FALSE_;
    return TRUE_;
}

void move_point(Point *pos, int dir) {
    switch (dir) {
        case Up:    --pos->y; break;
        case Right: ++pos->x; break;
        case Down:  ++pos->y; break;
        case Left:  --pos->x;
    }    
}

void print_at(const char *str, const Point pos) {
    SetCursor(pos);
    printf("%s", str);
}

void appear_star() {
    while (1) {
        star.x = rand() % 20 + 1;
        star.y = rand() % 20 + 1;
        if (mainBoard[star.x][star.y] == 0) {
            mainBoard[star.x][star.y] = star_sign;
            break;
        }
    }
    print_at(STAR, star);
}

void SetCursor(const Point p) {				// Set position of cursor		
    COORD cd = {p.x * 2, p.y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cd);
}

void HideCursor() {
    HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ConsoleCursorInfo;
    GetConsoleCursorInfo(hConsoleOutput, &ConsoleCursorInfo);
    ConsoleCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsoleOutput, &ConsoleCursorInfo);
}
