#pragma once

class Timer {
public:
    //Initializes variables
    Timer();

    //The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    //Gets the timer's time
    Uint32 get_ticks();

    //Checks the status of the timer
    bool is_started();
    bool is_paused();

private:
    //The clock time when the timer started
    Uint32 start_ticks;

    //The ticks stored when the timer was paused
    Uint32 paused_ticks;

    //The timer status
    bool paused;
    bool started;
};
