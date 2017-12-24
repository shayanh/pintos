#include "devices/mydevice.h"

#include "devices/timer.h"

struct mydevice devices[MAX_DEVICES];

void mydevice_init(int id) {
    if (id >= MAX_DEVICES) {
        return ;
    }
    lock_init(&devices[id].lock);
    lock_init(&devices[id].protection);
}

void mydevice_do_io(int id, int64_t ticks) {
    if (id >= MAX_DEVICES) {
        return ;
    }
    lock_acquire(&devices[id].protection);
    devices[id].queue_time += ticks;
    lock_release(&devices[id].protection);

    lock_acquire(&devices[id].lock);
    timer_sleep(ticks);

    lock_acquire(&devices[id].protection);
    devices[id].queue_time += ticks;
    lock_release(&devices[id].protection);

    lock_release(&devices[id].lock);
}

int64_t mydevice_get_queue_time(int id) {
    return devices[id].queue_time;
}