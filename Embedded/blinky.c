
#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdio.h>

#define LED_PIN 25 // for testing
#define NUM_CHANNELS 10 // number of electrodes we are reading from
#define SAMPLING_RATE 10 // how often we want to sample in Hz
// currently unused : #define MAX_SIZE 100 // maximum number of samples we can have that we don't deal with
volatile long curData[NUM_CHANNELS];

// The pins that will communicate with MUX
#define S0 21
#define S1 20
#define S2 19
#define S3 18
#define E 17
#define dataPin 0

int findSampleRate();


repeating_timer_callback(struct repeating_timer *t) {
    // This is where we will collect data

    // For testing sake
    gpio_put(LED_PIN, !gpio_get(LED_PIN));

    for (int i = 0; i < NUM_CHANNELS; i++) {
        changeChannel(i);
        curData[i] = gpio_get(dataPin);
    }

    // TODO write the data somewhere
}

/*
'   Queue implementation based on code from geeksforgeeks.org
    https://www.geeksforgeeks.org/queue-in-c/
*/
/*
    May bring this back if we cannot deal with data quick enough but for now gonna try to deal with data as it comes in.
void initalizeQueue();
bool isEmpty();
void enqueue();

typedef struct {
    int items[MAX_SIZE][NUM_CHANNELS];
    int front;
    int rear;
} Queue;

void initalizeQueue(Queue* q) {
    q->front = -1;
    q->rear = 0;
}

bool isEmpty(Queue* q) { return (q->front == q-> rear - 1);}
bool isFull(Queue* q) { return (q->rear == MAX_SIZE); }

void enqueue(Queue* q, int[NUM_CHANNELS] value) {
    if (isFull(q)) {
        printf("Queue is full\n");
        return;
    }
    q->items[q->rear] = value;
    q->rear++;
}
*/

/**
 * Writes pins S0 S1 S2 S3 and E to select the approprate channel we want
 * returns true if sucsessfully changed cannel
 * returns false if sent a channel we cannot select
 * 
 * from CD74HC4067M96 datasheet:
 * selected channel:
 * none: 1 on E
 * 
 * All others: 0 on E and:
 * S0   S1  S2  S3      Channel
 * 0    0   0   0       0
 * 1    0   0   0       1
 * 0    1   0   0       2
 * 1    1   0   0       3
 * 0    0   1   0       4
 * 1    0   1   0       5
 * 0    1   1   0       6
 * 1    1   1   0       7
 * 0    0   0   1       8
 * 1    0   0   1       9
 * 0    1   0   1       10
 * 1    1   0   1       11
 * 0    0   1   1       12
 * 1    0   1   1       13
 * 0    1   1   1       14
 * 1    1   1   1       14
 */
bool changeChannel(int c) {
    if (c < 0 || c > 15) {
        gpio_put(E,1);
        return false;
    }
    
    switch (c) {
        case 0:
            gpio_put(S0,0);
            gpio_put(S1,0);
            gpio_put(S2,0);
            gpio_put(S3,0);
            break;
        case 1:
            gpio_put(S0,1);
            gpio_put(S1,0);
            gpio_put(S2,0);
            gpio_put(S3,0);
            break;
        case 2:
            gpio_put(S0,0);
            gpio_put(S1,1);
            gpio_put(S2,0);
            gpio_put(S3,0);
            break;
        case 3:
            gpio_put(S0,1);
            gpio_put(S1,1);
            gpio_put(S2,0);
            gpio_put(S3,0);
            break;
        case 4:
            gpio_put(S0,0);
            gpio_put(S1,0);
            gpio_put(S2,1);
            gpio_put(S3,0);
            break;
        case 5:
            gpio_put(S0,1);
            gpio_put(S1,0);
            gpio_put(S2,1);
            gpio_put(S3,0);
            break;
        case 6:
            gpio_put(S0,0);
            gpio_put(S1,1);
            gpio_put(S2,1);
            gpio_put(S3,0);
            break;
        case 7:
            gpio_put(S0,1);
            gpio_put(S1,1);
            gpio_put(S2,1);
            gpio_put(S3,0);
            break;
        case 8:
            gpio_put(S0,0);
            gpio_put(S1,0);
            gpio_put(S2,0);
            gpio_put(S3,1);
            break;
        case 9:
            gpio_put(S0,1);
            gpio_put(S1,0);
            gpio_put(S2,0);
            gpio_put(S3,1);
            break;
        case 10:
            gpio_put(S0,0);
            gpio_put(S1,1);
            gpio_put(S2,0);
            gpio_put(S3,1);
            break;
        case 11:
            gpio_put(S0,1);
            gpio_put(S1,1);
            gpio_put(S2,0);
            gpio_put(S3,1);
            break;
        case 12:
            gpio_put(S0,0);
            gpio_put(S1,0);
            gpio_put(S2,1);
            gpio_put(S3,1);
            break;
        case 13:
            gpio_put(S0,1);
            gpio_put(S1,0);
            gpio_put(S2,1);
            gpio_put(S3,1);
            break;
        case 14:
            gpio_put(S0,0);
            gpio_put(S1,1);
            gpio_put(S2,1);
            gpio_put(S3,1);
            break;
        case 15:
            gpio_put(S0,1);
            gpio_put(S1,1);
            gpio_put(S2,1);
            gpio_put(S3,1);
            break;
        
        
    }
    gpio_put(E,0);
    return true;
}

// Finds the time between each sample in ms
int findSampleRate() {
    return 1000 / SAMPLING_RATE;;
}

// Main (runs on core 0)
int main() {
    // Initialize the pins
    gpio_init(LED_PIN);
    gpio_init(S0);
    gpio_init(S1);
    gpio_init(S2);
    gpio_init(S3);
    gpio_init(E);
    gpio_init(dataPin);

    // Configure the pins as input or output
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(S0, GPIO_OUT);
    gpio_set_dir(S1, GPIO_OUT);
    gpio_set_dir(S2, GPIO_OUT);
    gpio_set_dir(S3, GPIO_OUT);
    gpio_set_dir(E, GPIO_OUT);
    gpio_set_dir(dataPin, GPIO_IN);
    gpio_put(E,1); // tels the MUX we are not currently selecting a channel

    stdio_init_all();

    int sampleRate = -1 * findSampleRate();
    struct repeating_timer timer;
    add_repeating_timer_ms(sampleRate,repeating_timer_callback, NULL, &timer); // Starts the sampling timer

    // TODO: Change this to only loop for as long as we wanna collect data
    while(1){
        // Doing low priority stuff
    }
}

