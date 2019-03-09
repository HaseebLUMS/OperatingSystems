#ifndef PART_3
#define PART_3

enum DIRECTION {
    NORTH,
    SOUTH,
    EAST,
    WEST
};

enum LANE {
    LEFT,
    RIGHT
};

void initializeP3();
/**
 * If there is a car going from SOUTH to NORTH, from lane LEFT,
 * print 
 * SOUTH NORTH LEFT
 * Also, if two cars can simulateneously travel in the two lanes,
 * first print all the cars in the LEFT lane, followed by all the
 * cars in the right lane
 */
void *goingFromToP3(void * argument);
void startP3();
void * north_signal_f();
void * east_signal_f();
void * south_signal_f();
void * west_signal_f();
int is_left_turn(int from, int to);
char* directions(int i);
char* lanes(int i);
#endif
