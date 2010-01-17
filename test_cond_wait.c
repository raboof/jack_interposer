#include <jack/jack.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

bool been_in_process = false;

int test_process_callback(jack_nframes_t nframes, void* arg)
{
  pthread_cond_wait(&cond, &mutex);
  been_in_process = true;
  return 1;
}

int main()
{
  jack_client_t * client = jack_client_open("testclient", JackNullOption, NULL);
  jack_set_process_callback(client, test_process_callback, NULL);
  jack_activate(client);
  while(!been_in_process)
  {
    pthread_cond_signal(&cond);
  }
  jack_deactivate(client);
  printf("Been in process: %d\n", been_in_process);
  return 0;
}
