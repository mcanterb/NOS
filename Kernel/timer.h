
typedef void (*TimerCallback)(int param);

typedef struct 
{
    unsigned int ticks;
    TimerCallback schedCallback;
    int param;
} Timer;


void InitTimer(int frequency);
void sleep(unsigned int ticks);
int NewTimer(unsigned int ticks, TimerCallback schedCallback, int param);
void DestroyTimer(int timer);
void ResetTimer(int timer, unsigned int ticks);