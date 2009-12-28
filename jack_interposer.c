// needed to use RTLD_NEXT from dlfcn.h
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <poll.h>
#include <jack/jack.h>

#define ABORT_ON_VIOLATION 1

bool in_rt = false;

JackProcessCallback real_process_callback;

/* this doesn't quite seem to work - not sure why yet
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
  static int (*func)();

  if (in_rt)
  {
    printf("pthread_cond_wait() is called while in rt section\n");
#if ABORT_ON_VIOLATION
    abort();
#endif
  }
  if(!func)
    func = (int (*)()) dlsym(RTLD_NEXT, "pthread_cond_wait");
  return(func(cond, mutex));
}
*/

int pthread_join(pthread_t thread, void **value_ptr)
{
  static int (*func)();

  if (in_rt)
  {
    printf("pthread_join() is called while in rt section\n");
#if ABORT_ON_VIOLATION
    abort();
#endif
  }
  if(!func)
    func = (int (*)()) dlsym(RTLD_NEXT, "pthread_join");
  return(func(thread, value_ptr));
}

int select(int nfds, fd_set *readfds, fd_set *writefds,
                  fd_set *exceptfds, struct timeval *timeout)
{
  static int (*func)();

  if (in_rt)
  {
    printf("select() is called while in rt section\n");
#if ABORT_ON_VIOLATION
    abort();
#endif
  }
  if(!func)
    func = (int (*)()) dlsym(RTLD_NEXT, "select");
  return(func(nfds, readfds, writefds, exceptfds, timeout));
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
  static int (*func)();

  if (in_rt)
  {
    printf("poll() is called while in rt section\n");
#if ABORT_ON_VIOLATION
    abort();
#endif
  }
  if(!func)
    func = (int (*)()) dlsym(RTLD_NEXT, "poll");
  return(func(fds, nfds, timeout));
}

pid_t wait(int* status)
{
  static pid_t (*func)();

  if (in_rt)
  {
    printf("wait() is called while in rt section\n");
#if ABORT_ON_VIOLATION
    abort();
#endif
  }
  if(!func)
    func = (pid_t (*)()) dlsym(RTLD_NEXT, "wait");
  return(func(status));
}

unsigned int sleep(unsigned int seconds)
{
  static unsigned int (*func)();

  if (in_rt)
  {
    printf("sleep(%d) is called while in rt section\n", seconds);     
#if ABORT_ON_VIOLATION
    abort();
#endif
  }
  if(!func)
    func = (unsigned int (*)()) dlsym(RTLD_NEXT, "sleep");
  return(func(seconds));
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
  static int (*func)();

  if (in_rt)
  {
    printf("pthread_mutex_lock() is called while in rt section\n");     
#if ABORT_ON_VIOLATION
    abort();
#endif
  }
  if(!func)
    func = (int (*)()) dlsym(RTLD_NEXT, "pthread_mutex_lock");
  return(func(mutex));
}

void free(void* ptr)
{
  static void * (*func)();

  if (in_rt)
  {
    printf("free() is called while in rt section\n");     
#if ABORT_ON_VIOLATION
    abort();
#endif
  }
  if(!func)
    func = (void *(*)()) dlsym(RTLD_NEXT, "free");
  func(ptr);
}

void * malloc(size_t size)
{
  static void * (*func)();

  if (in_rt)
  {
    printf("malloc(%d) is called while in rt section\n", size);     
#if ABORT_ON_VIOLATION
    abort();
#endif
  }
  if(!func)
    func = (void *(*)()) dlsym(RTLD_NEXT, "malloc");
  return(func(size));
}

int interposed_process_callback(jack_nframes_t nframes, void* arg)
{
  int result;

  in_rt = true;

  result = real_process_callback(nframes, arg);

  in_rt = false;

  return result;
}

int jack_set_process_callback(jack_client_t* client,
	JackProcessCallback process_callback, void* arg)
{
  static int (*func)();
  int result;

  if(!func)
    func = (int(*)()) dlsym(RTLD_NEXT, "jack_set_process_callback");

  real_process_callback = process_callback;

  result = func(client, interposed_process_callback, arg);

  return result;
}
