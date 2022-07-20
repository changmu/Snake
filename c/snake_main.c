#include "snake.h"

int main() {
    game_init();
    while (game_move());
    game_end();
    return 0;
}
