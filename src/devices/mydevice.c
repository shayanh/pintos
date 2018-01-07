#include "devices/mydevice.h"

#include "devices/timer.h"
#include <stdio.h>

struct mydevice devices[MAX_DEVICES];

void mydevice_init(int id) {
    if (id < 0 || id >= MAX_DEVICES) {
        return ;
    }
    lock_init(&devices[id].lock);
    lock_init(&devices[id].protection);
	devices[id].queue_time = 0;
}

void mydevice_do_io(int id, int64_t ticks) {
    if (id < 0 || id >= MAX_DEVICES) {
        return ;
    } 
	lock_acquire(&devices[id].protection);
    devices[id].queue_time += ticks;
    lock_release(&devices[id].protection);

    lock_acquire(&devices[id].lock);
	printf("device %d, io for %lld ticks\n", id, ticks);
    timer_sleep(ticks);
    lock_release(&devices[id].lock);

    lock_acquire(&devices[id].protection);
    devices[id].queue_time -= ticks;
    lock_release(&devices[id].protection);

}

int64_t mydevice_get_queue_time(int id) {
    if (id < 0 || id >= MAX_DEVICES) {
        return ;
    }
    return devices[id].queue_time;
}
