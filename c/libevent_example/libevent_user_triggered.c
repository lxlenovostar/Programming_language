/**
libevent
user-triggered event example
*/

#include <event2/event.h>

//  gcc -g -Wall libevent-user-triggered.c -levent -levent_core

void cb(int fd, short events, void *arg)
{
    puts("cb");
}

int main()
{
    struct event_base *base = event_base_new();
    struct event *event = event_new(base, -1, EV_READ, cb, NULL);
    event_active(event, 0, 0);
    puts("start event loop");
    event_base_loop(base, 0);
    puts("exit event loop");
    return 0;
}
