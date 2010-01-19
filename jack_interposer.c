// needed to use RTLD_NEXT from dlfcn.h
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <poll.h>
#include <jack/jack.h>
#include <stdarg.h>

#define ABORT_ON_VIOLATION 1

// is set to 'true' when entering the process-callback and to 'false' when 
// leaving it. When set to 'true', calls to non-realtime functions will 
// cause warnings/errors.
// 
// This assumes there is only 1 thread running at a time, thus introducing
// the limitation that jack_interposer is only usable on single-CPU machines
// (or machines configured to run the application under test on only 1 CPU).
bool in_rt = false;

#include "checkers.c"

JackProcessCallback real_process_callback;

int fprintf(FILE *stream, const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  return vfprintf(stream, format, ap);
}

int printf(const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  return vprintf(format, ap);
}

int pthread_cond_timedwait(pthread_cond_t * cond, pthread_mutex_t * mutex, const struct timespec* abstime)
{
  static int (*func)(pthread_cond_t*, pthread_mutex_t*, const struct timespec* abstime);

  if (in_rt)
  {
    printf("pthread_cond_timedwait() is called while in rt section\n");     
#if ABORT_ON_VIOLATION
    abort();
#endif
  }

  if(!func)
    //func = (int (*)(pthread_cond_t*, pthread_mutex_t*)) dlsym(RTLD_NEXT, "pthread_cond_wait");
    // see http://forums.novell.com/novell-product-support-forums/suse-linux-enterprise-server-sles/sles-configure-administer/385705-sles-10-2-java-hung-calling-pthread_cond_timedwait.html
    func = (int (*)()) dlvsym(RTLD_NEXT, "pthread_cond_timedwait", "GLIBC_2.3.2");
  if (func == NULL)
  {
    fprintf(stderr, "Error dlsym'ing\n");
    abort();
  }
  return(func(cond, mutex, abstime));
}


int pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex)
{
  static int (*func)(pthread_cond_t*, pthread_mutex_t*);

  if (in_rt)
  {
    printf("pthread_cond_wait() is called while in rt section\n");     
#if ABORT_ON_VIOLATION
    abort();
#endif
  }

  if(!func)
    //func = (int (*)(pthread_cond_t*, pthread_mutex_t*)) dlsym(RTLD_NEXT, "pthread_cond_wait");
    // see http://forums.novell.com/novell-product-support-forums/suse-linux-enterprise-server-sles/sles-configure-administer/385705-sles-10-2-java-hung-calling-pthread_cond_timedwait.html
    func = (int (*)(pthread_cond_t*, pthread_mutex_t*)) dlvsym(RTLD_NEXT, "pthread_cond_wait", "GLIBC_2.3.2");
  if (func == NULL)
  {
    fprintf(stderr, "Error dlsym'ing\n");
    abort();
  }
  return(func(cond, mutex));
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
  static int (*func)() = NULL;
  int result;

  if(!func)
    func = (int(*)()) dlsym(RTLD_NEXT, "jack_set_process_callback");
  if(!func)
  {
    fprintf(stderr, "Error dlsym'ing jack_set_process_callback\n");
    abort();
  }
 
  real_process_callback = process_callback;

  result = func(client, interposed_process_callback, arg);

  return result;
}
