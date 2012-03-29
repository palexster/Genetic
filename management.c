#include "management.h"

void my_handler(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("Received SIGUSR1!\n");
    }
}

//signal(SIGUSR1, my_handler);
