/*
 *	./a.out <node> [<service>]
 *
 *	NOTE: inet_pton and inet_ntop
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>		//include def for struct addrinfo

#include "telnet.h"

#define BUF_SIZE 1024

//here we should use unsigned as signed char can cause problems
unsigned char buffer[BUF_SIZE] = {0};

int
main (int argc, char** argv)
{
	printf ("main ok\n");
    if (argc != 3)
    {
	fprintf (stderr, "Usage: ./a.out <node> [<service>]\n");
	exit (EXIT_SUCCESS);
    }
    //1. name resolution: 
    struct addrinfo hints;
    struct addrinfo* res;
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_INET; // only support IPv4
    hints.ai_socktype = SOCK_STREAM; //tcp stream

    int status;
    if ((status = getaddrinfo (argv[1], argv[2], &hints, &res)) != 0)
    {
	perror ("getaddrinfo error: ");
	exit (EXIT_FAILURE);
    }
    //1.1 go through the lists.
    int client_sfd;
    int conn_retval;
    struct addrinfo* rp;
    for(rp = res; rp != NULL; rp = rp->ai_next) 
    {

    //2. create a socket and connect to remote address.
	client_sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
	if (client_sfd == -1)
	{
	    close (client_sfd);
	    perror ("socket errror: ");
	    continue;
	}
	conn_retval = connect (client_sfd, rp->ai_addr, rp->ai_addrlen);
	if (conn_retval == -1)
	{
	    close (client_sfd);
	    perror ("connect errror: ");
	    continue;
	}
	break;
    }
	printf("connect ok\n");
    if ((client_sfd == -1)||(conn_retval == -1))
    {
	fprintf (stderr, "socket and connect all failed\n");
	exit (EXIT_FAILURE);
    }
    freeaddrinfo (res);
    //3. event loop
    printf("event loop\n");
    int read_size = 0;
    unsigned char tmp_buffer[TELNET_MAX_CMD_LEN];
    int tmp_size;
    telnet_nvt state;
	
    telnet_nvt_set_sock_fd (&state, client_sfd);

    //for write
    int n = TELNET_MAX_CMD_LEN;
    char* lineptr = (char*)malloc (n+1);

    telnet_toggle_log_cmd (TELNET_LOG_CMD_ON);
    for (;;)
    {
	//4. read data
	read_size = 0;
	tmp_size = 0;
	memset (buffer, 0, sizeof (buffer));
	memset (tmp_buffer, 0, sizeof (tmp_buffer));
	while ((read_size = read (client_sfd, buffer, sizeof(buffer)))!=-1)
	{
		//printf("read buffer=%s\n", buffer);
	    //5. data processing
	    telnet_parse_data (&state, 
			       tmp_buffer, &tmp_size,
			       buffer, read_size);

	    if (read_size < sizeof(buffer))
		break;
	}
	//4. write data
	lineptr = fgetln(stdin, &tmp_size);
	//printf("input cmd=%.*s\n", tmp_size, lineptr);
	//if ((tmp_size = getline (&lineptr, &n, stdin))!=-1)
	if (lineptr != NULL)
	{
	    lineptr[tmp_size] = TELNET_NVT_ASCII_CR;
	    lineptr[tmp_size+1] = TELNET_NVT_ASCII_LF;
	    lineptr[tmp_size+2] = NULL;
		//printf("lineptr to send=%.*s\n", tmp_size, lineptr);
	    write (client_sfd, lineptr, tmp_size+2);
	}
    }
    //5. close connection
    close (client_sfd);
}
