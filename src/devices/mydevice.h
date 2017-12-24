#ifndef DEVICES_MYDEVICE_H
#define DEVICES_MYDEVICE_H

#include "threads/synch.h"

#define MAX_DEVICES 3

struct mydevice {
    struct lock lock;
    struct lock protection;
    int64_t queue_time;
};

void mydevice_init(int id);
void mydevice_do_io(int id, int64_t ticks);
int64_t mydevice_get_queue_time(int id);

#endif /* devices/mydevice.h */