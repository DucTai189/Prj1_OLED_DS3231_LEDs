#include "main.h"

pthread_mutex_t lock_Blink_Led16 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_Blink_Led16 = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_Blink_Led13 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_Blink_Led13 = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_Oled = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_Oled = PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock_DS3231 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_DS3231  =  PTHREAD_COND_INITIALIZER;

DS3231_st buf;
volatile int event_display_update = 0;
void* Task_Blink_Led16(void* arg) 
{
    while (1) 
    {
        pthread_mutex_lock(&lock_Blink_Led16);
        pthread_cond_wait(&cond_Blink_Led16, &lock_Blink_Led16);  // Wait for timer to signal
        blink_led_16();
        pthread_mutex_unlock(&lock_Blink_Led16);
    }
    return NULL;
}

void* Task_Blink_Led13(void* arg) 
{
    while (1) 
    {
        pthread_mutex_lock(&lock_Blink_Led13);
        pthread_cond_wait(&cond_Blink_Led13, &lock_Blink_Led13);  // Wait for timer to signal
        blink_led_13();
        pthread_mutex_unlock(&lock_Blink_Led13);
    }
    return NULL;
}

void* Task_Oled(void* arg) 
{

    while (1) 
    {
        pthread_mutex_lock(&lock_Oled);
        while (event_display_update == 0) 
        {
            pthread_cond_wait(&cond_Oled, &lock_Oled);
        }
        event_display_update = 0;

        pthread_mutex_unlock(&lock_Oled);

        // Safe to update OLED now
        Oled_SSD_1303_Display();
    }
    return NULL;
}

void* Task_DS3231(void* arg) 
{
    while (1) 
    {
        pthread_mutex_lock(&lock_Oled);
        pthread_cond_wait(&cond_Oled, &lock_Oled);  // Wait for timer to signal
        DS3231_GetTime(&buf);
        event_display_update = 1; // Set flag to indicate OLED update needed    
        pthread_mutex_unlock(&lock_Oled);
    }
    return NULL;
}

void timer_handler(int sig, siginfo_t* si, void* uc) 
{
    int id = *(int*)si->si_value.sival_ptr;

    switch (id)
    {
        case 1:
            pthread_cond_signal(&cond_Blink_Led16);
        break;

        case 2:
            pthread_cond_signal(&cond_Blink_Led13);
        break;

        case 3:
            oled_clear();
            pthread_cond_signal(&cond_Oled);
        break;

        case 4:
            pthread_cond_signal(&cond_DS3231);
        break;
    
        default:
        break;
    }

}

void create_timer(timer_t* timerid, int signal_id, int ms_period) 
{
    struct sigevent sev;
    struct itimerspec its;

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = malloc(sizeof(int));
    *(int*)(sev.sigev_value.sival_ptr) = signal_id;
    timer_create(CLOCK_REALTIME, &sev, timerid);

    its.it_value.tv_sec = ms_period / 1000;
    its.it_value.tv_nsec = (ms_period % 1000) * 1000000;
    its.it_interval = its.it_value;

    timer_settime(*timerid, 0, &its, NULL);
}

void blink_led_16()
{

    static temp_value =0;

    gpiod_line_set_value(line_GPIO16, (temp_value));
    temp_value ^= 1;
   
}

void blink_led_13()
{
    static temp_value =0;

    gpiod_line_set_value(line_GPIO13, (temp_value));
    temp_value ^= 1;
}

void Oled_SSD_1303_Display()
{
    char buffer_Time[32];          // Buffer to hold ASCII string
    char buffer_Date[32];          // Buffer to hold ASCII string
    DS3231_st time_data;
    const char *DayOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    int written;

    memcpy(&time_data, &buf, sizeof(buf)); // Copy "Time: " to buffer
    written = snprintf(buffer_Time, sizeof(buffer_Time), "Time: %02d:%02d:%02d", 
                       time_data.hours, time_data.minutes, time_data.seconds);

    if (written < 0 || written >= sizeof(buffer_Time))
    {
        fprintf(stderr, "Error formatting time string\n");
    }
    oled_draw_string(0, 0, buffer_Time);
    
    // Check dayOfWeek is in valid range (1-7)
    if (time_data.dayOfWeek >= 1 && time_data.dayOfWeek <= 7) {
        oled_draw_string(0, 10, DayOfWeek[time_data.dayOfWeek - 1]);
    } else {
        oled_draw_string(0, 10, "Unknown");
    }

    written = snprintf(buffer_Date, sizeof(buffer_Date), "Date: %02d/%02d/20%02d", 
                       time_data.date, time_data.month, time_data.year);


    oled_draw_string(0, 20, buffer_Date);

    oled_display();

}
// Function to set real-time priority for a thread
void set_realtime_priority(pthread_t thread, int priority) 
{
    struct sched_param param;
    param.sched_priority = priority;

    if (pthread_setschedparam(thread, SCHED_FIFO, &param) != 0) 
    {
        perror("pthread_setschedparam");
    } 
    else 
    {
        printf("Set real-time priority %d\n", priority);
    }

    
}

int main() 
{

    Init_OLED();
    Init_GPIO();
    INIT_DS3231();
  //  DS3231_SetTime();
 // Setup signal
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGRTMIN, &sa, NULL);

    // Start threads
    pthread_t thread_Blink_Led16, thread_Blink_Led13, thread_Oled, thread_DS3231;
    pthread_create(&thread_Blink_Led16, NULL, Task_Blink_Led16, NULL);
    // Set real-time priority for Led 16 thread
    set_realtime_priority(thread_Blink_Led16, 50); 
    pthread_create(&thread_Blink_Led13, NULL, Task_Blink_Led13, NULL);
    // Set real-time priority for Led 12 thread
    set_realtime_priority(thread_Blink_Led13, 40); 

    // Create OLED thread
     pthread_create(&thread_Oled, NULL, Task_Oled, NULL);
    // Set real-time priority for OLED thread
    set_realtime_priority(thread_Oled, 30); 

    // Create DS3231 thread
    pthread_create(&thread_DS3231, NULL, Task_DS3231, NULL);
    // Set real-time priority for DS3231 thread
    set_realtime_priority(thread_DS3231, 20);

    // Create timers
    timer_t timer1, timer2, timer3,timer4;
    create_timer(&timer1, 1, 200);   // Task Blink Led 17 - 10ms
    create_timer(&timer2, 2, 250);   // Task Blink Led 27 - 250ms
    create_timer(&timer3, 3, 500);   // Task Display Oled - 1000ms (increased from 500ms)
    create_timer(&timer4, 4, 1000);   // Task Read DS3231 - 1000ms

    // Main loop
    // This loop is just to keep the main thread alive
    // and allow the timers to trigger the tasks.
    // In a real application, you might have other logic here.
    while (1)
    {
        pause(); // Wait for signals
    }
    return 0;
}