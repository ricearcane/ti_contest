#ifndef SEM_H_INCLUDED
#define SEM_H_INCLUDED



#endif // SEM_H_INCLUDED



/***
* Wait on the specified semaphore.
* @param s - the semaphore to wait on
**/
void wait(int s);

/***
* Signal on the specified semaphore, allowing blocked processes to run
* @param s - the semaphore to signal
**/
void clear(int s);

/***
* Acquire a new semaphore. This is provided as a wrapper for _locknew
*
*@return - the semaphore acquired (0-7) or -1 if no semaphores are available
**/
int get_semaphore();

/***
* Release the specified semaphore for later reuse.
* @param s - the semaphore to release
**/
void release_semaphore(int s);
