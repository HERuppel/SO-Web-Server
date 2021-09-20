#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "server.h"

/* HTML source for the start of the page we generate.  */

static char* page_start =
  "<html>\n"
  " <body>\n"
  "  <pre>\n";

/* HTML source for the end of the page we generate.  */

static char* page_end =
  "  </pre>\n"
  " </body>\n"
  "</html>\n";

// URL Params for GET METHOD
extern char* req_params; 
char params_instruction[] = "O parametro e: interface=eth0\n";

void module_generate (int fd)
{
  pid_t child_pid;
  int rval;

  char* params = xmalloc(sizeof(req_params));
  /* Write the start of the page.  */
  write (fd, page_start, strlen (page_start));
  /* Fork a child process.  */
  child_pid = fork ();
  if (child_pid == 0) {
    char *interface = NULL;

    strcpy(params, req_params);
    if (strcmp(params, "")) {
      interface = xmalloc(sizeof(params));

      strcpy(interface, params);
      interface = strstr(interface, "interface");

      if (interface != NULL) {
        interface = strchr(interface, '=') + 1;
        strtok(interface, "&");
      }
    }

    rval = dup2(fd, STDOUT_FILENO);
    if (rval == -1)
      system_error("dup2");

    rval = dup2(fd, STDERR_FILENO);
    if (rval == -1)
      system_error("dup2");

    if (interface == NULL) {
        char* argv[] = {"/sbin/ifconfig", NULL};
        execv(argv[0], argv);
    } else {
      if (strcmp(interface, "eth0") == 0) {
        char* argv[] = {"/sbin/ifconfig", "eth0", NULL};
        execv(argv[0], argv);
      }
      else {
        write(fd, params_instruction, strlen(params_instruction));
      }
    }
    
    system_error("execv");
  }
  else if (child_pid > 0) {
    /* This is the parent process.  Wait for the child process to
       finish.  */
    rval = waitpid (child_pid, NULL, 0);
    if (rval == -1)
      system_error ("waitpid");
  }
  else 
    /* The call to fork failed.  */
    system_error ("fork");
  /* Write the end of the page.  */
  write (fd, page_end, strlen (page_end));
}