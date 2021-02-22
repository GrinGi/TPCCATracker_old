#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>

int main(int, char **argv)
{
  sched_param param;
  sched_getparam( 0, &param );
  param.sched_priority = 2;
  if ( 0 != sched_setscheduler( 0, SCHED_FIFO, &param ) ) {
    perror( "1. fail" );
  }

  const pid_t pid = fork();
  if ( pid == 0 ) {
    // I'm the child

    // make the child realtime prio, but less than the parent
    sched_param param2;
    sched_getparam( 0, &param2 );
    param2.sched_priority = 1;
    if ( 0 != sched_setscheduler( 0, SCHED_FIFO, &param2 ) ) {
      perror( "2. fail" );
    }

    // drop privileges
    if ( geteuid() != getuid() ) {
      seteuid( getuid() );
      if ( geteuid() != getuid() ) {
        return -1;
      }

      argv[0] = strdup( "./CA" );
      execv( "./CA", argv );
      perror( "./CA" );
      return -1;
    }
  }

  timeval timeout;
  for ( int i = 0; i < 600; ++i ) {
    timeout.tv_sec =  1;
    timeout.tv_usec = 0;
    select( 0, 0, 0, 0, &timeout );
    if ( pid == waitpid( pid, 0, WNOHANG ) ) {
      return 0;
    }
  }

  kill( pid, SIGKILL );

  return 0;
}
