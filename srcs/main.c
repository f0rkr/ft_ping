#include "ft_ping.h"

int g_loop;

/** @Brief Signal Handler
 * Handle the SIGKILL signal by setting the loop variable
 * to false. By doing so the program will quit the loop and
 * gives a report.
 * 
 * @param sig Interrupt Signal code
 * @return none
 */
void    interrupt_handler(int sig)
{
    if (sig == SIGINT)
        g_loop = 0;
    return ;
}

/** @Bief Ping Fuction
 * Send ICMP packets to the target host
 * 
 * @param icmp_sock Raw ICMP socket
 * @return none
 */
void    icmp_echo(struct sockaddr_in *addr, char *hostname, int icmp_sock)
{
    int     ttl;
    
    ttl = icmp_sock;
    g_loop=1;
    printf("PING %s (%s): 56 data bytes\n", hostname, inet_ntoa((struct in_addr)addr->sin_addr));
    while (g_loop) {
        printf("64 bytes from %s: icmp_seq=%d ttl=114 time=%f ms\n", inet_ntoa((struct in_addr)addr->sin_addr), ttl, (float)43.234+ttl);
        usleep(1000*100);
        ttl++;
    }     
    printf("\n--- %s ping statistics ---\n", hostname);
    return ;
}

/** @Brief Main function
 * Parsing arguments and execute the main program
 * 
 * @param argc Number of arguments
 * @param argv CL arguments
 * @6return
 */
int     main(int argc, char **argv) {
    t_args *args;

    struct addrinfo *res;

    if (argc <= 1 || argc > 4) {
        printf("Error: wrong number of arguments\n");
        return (1);
    }
    // Parsing data 
    args = parse_clo(argc, argv);
    if (args == NULL)
        return (1);
    // printf("hostname: %s, verbos: %d, help: %d\n", args->hostname, args->verbose, args->help);
    if (getaddrinfo(args->hostname, NULL, NULL, &res) != 0) {
        printf("ft_ping: cannot resolve %s: Unknown host\n", args->hostname);
        return (1);
    }
    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr; 
   
    signal(SIGINT, (void *)&interrupt_handler);

    // TO-DO: send icmp echo packets to targeted host
    icmp_echo(addr, args->hostname, 0);

    return (0);
}