#include <vector>
#include <map>
#include <chrono>

#include <termbox.h>

using namespace std;
using namespace chrono;

const float    BASE_SPEED  = 30.0f;
const float    BOOST_SPEED = 60.0f;
const int      FOOD_VALUE  = 10;

const uint16_t BACKGROUND  = TB_DEFAULT;

enum direction {
    UP,
    LEFT,
    DOWN,
    RIGHT,
};

struct point {
    float x;
    float y;
};

struct body_part {
    point pos;
    time_point<steady_clock> time;
};

struct snake {
    point pos;

    direction d;

    map<direction, char> controls;

    uint16_t head_color;
    uint16_t body_color;

    vector<body_part> body;
};

snake player1;
snake player2;

bool process_input() {
    tb_event event;

    int val = tb_peek_event(&event, 1);

    if(val < 0) {
        return false;
    }

    if(val > 0) {
        if(event.type == TB_EVENT_KEY) {
            char c = event.ch;
            uint16_t key = event.key;

            if(key == TB_KEY_ESC || key == TB_KEY_CTRL_C) {
                return false;
            }

            for(direction d : {UP, DOWN, LEFT, RIGHT}) {
                for(snake * s : {&player1, &player2})
                    if(s->controls[d] == c)
                        s->d = d;
            }

        } else if(event.type == TB_EVENT_RESIZE) {
            return false;
        }
    }

    return true;
}

time_point<steady_clock> last_update;
bool update() {
    tb_clear();

    time_point<steady_clock> this_update = steady_clock::now();
    float dt = duration_cast<milliseconds>(this_update - last_update).count() / 1000.0f;
    last_update = this_update;

    for(snake * s : {&player1, &player2}) {
        point new_pos = {s->pos.x, s->pos.y};
        switch(s->d) {
            case UP:    new_pos.y -= 1 * dt * BASE_SPEED; break;
            case DOWN:  new_pos.y += 1 * dt * BASE_SPEED; break;
            case LEFT:  new_pos.x -= 2 * dt * BASE_SPEED; break;
            case RIGHT: new_pos.x += 2 * dt * BASE_SPEED; break;
        }
        

        point p = {s->pos.x, s->pos.y};
        while( (int)p.x != (int)new_pos.x || (int)p.y != (int)new_pos.y ) {
            switch(s->d) {
                case UP:    p.y -= 1; break;
                case DOWN:  p.y += 1; break;
                case LEFT:  p.x -= 1; break;
                case RIGHT: p.x += 1; break;
            }
            s->body.push_back( {p, this_update} );
        }
        s->pos=new_pos;

        for(body_part bp : s->body)
            tb_change_cell(bp.pos.x, bp.pos.y, '#', s->body_color, BACKGROUND);
        tb_change_cell(s->pos.x, s->pos.y, '#', s->head_color, BACKGROUND);
    }

    tb_present();

    return true;
}

int main() {
    player1.controls[UP]    = 'w';
    player1.controls[LEFT]  = 'a';
    player1.controls[DOWN]  = 's';
    player1.controls[RIGHT] = 'd';

    player1.d = RIGHT;

    player2.controls[UP]    = 'i';
    player2.controls[LEFT]  = 'j';
    player2.controls[DOWN]  = 'k';
    player2.controls[RIGHT] = 'l';

    player2.d = LEFT;

    tb_init();

    float width  = (float)tb_width();
    float height = (float)tb_height();

    point p_p1 = { width / 3,     height / 2};
    point p_p2 = { 3 * width / 4, height / 2 };

    player1.pos = p_p1;
    player2.pos = p_p2;

    player1.head_color = TB_RED;
    player2.head_color = TB_BLUE;

    player1.body_color = TB_YELLOW;
    player2.head_color = TB_MAGENTA;

    last_update = steady_clock::now();
    while(process_input() && update()) {
        // keep running until error or exit
    }

    tb_shutdown();

    return 0;
}