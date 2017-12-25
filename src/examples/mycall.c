#include <stdio.h>
#include <syscall.h>

int main(int argc, char *argv[]){
    sch_io(1);
    do_io(1, 10);
    return EXIT_SUCCESS;
}