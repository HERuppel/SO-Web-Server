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

// URL Params used in server.c for GET METHOD
extern char* req_params; 

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

    char* ano = NULL;
    char* mes = NULL;
    
    strcpy(params, req_params);
    if (strcmp(params, "")) {
      ano = xmalloc(sizeof(params));
      mes = xmalloc(sizeof(params));
      
      strcpy(ano, params);
      ano = strstr(ano, "ano");

      if (ano != NULL) {
        ano = strchr(ano, '=') + 1;
        strtok(ano, "&");
      }
      strcpy(mes, params);
      mes = strstr(mes, "mes");
      if (mes != NULL) {
          mes = strchr(mes, '=') + 1;
          strtok(mes, "&");
      }
    }

    rval = dup2(fd, STDOUT_FILENO);
    if (rval == -1)
        system_error("dup2");

    rval = dup2(fd, STDERR_FILENO);
    if (rval == -1)
      system_error("dup2");

    if (mes != NULL && ano != NULL) {
      char* argv[] = {"/usr/bin/cal", mes, ano, NULL};
      execv(argv[0], argv);
    } else if (mes != NULL && ano == NULL) {
      char* argv[] = {"/usr/bin/cal", "-m", mes, NULL};
      execv(argv[0], argv);
    } else if (mes == NULL && ano != NULL) {
      char* argv[] = {"/usr/bin/cal", ano, NULL};
      execv(argv[0], argv);
    } else {
      char* argv[] = {"/usr/bin/cal", NULL};
      execv(argv[0], argv);
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