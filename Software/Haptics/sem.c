#include "sem.h"
#include <catalina_cog.h>

/***
* Wait on the specified semaphore, halting execution until it becomes available.
* @param s - the semaphore to wait on
**/
void wait(int s){
    while(_lockset(s) == 1); //set lock once available.
}

/***
* Signal on the specified semaphore, allowing blocked processes to run
* @param s - the semaphore to signal
**/
void clear(int s){
    _lockclr(s); //clear lock
}

/***
* Acquire a new semaphore. This is provided as a wrapper for _locknew
*
*@return - the semaphore acquired (0-7) or -1 if no semaphores are available
**/
int get_semaphore(){
    return _locknew();
}

/***
* Release the specified semaphore for later reuse.
* @param s - the semaphore to release
**/
void release_semaphore(int s){
    _lockret(s);
}
