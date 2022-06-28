#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    // thread_func_args->mutex
    // thread_func_args->wait_to_obtain_ms
    // thread_func_args->wait_to_release_ms

    // usleep(sleep_before_lock_ms*20*1000),

    // pthread_join()

    // pthread_exit()
    // --> pthread_cancel()

    


    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    struct thread_data thd =
    {
        .mutex = mutex,
        .wait_to_obtain_ms = wait_to_obtain_ms,
        .wait_to_release_ms = wait_to_release_ms,
    };

    int rc = pthread_create(thread, NULL, threadfunc, &thd);
    if (rc != 0)
    {
        printf("pthread_create() failed: %d\n", rc);
        return false;
    }

    return true;
}

