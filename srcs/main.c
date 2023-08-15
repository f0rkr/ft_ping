#include "ft_ping.h"
//#include "globals.h"

t_ping   *g_ping;


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
    {
        g_ping->routine_loop = 0;
        show_statistics();
        collect_memory();
        exit(0);
        // Calculate total time in milliseconds
    }
    if (sig == SIGALRM)
        g_ping->alarm = 0;
    return ;
}


/**
 * @brief Setup Destination Address
 * Resolves the IP address of the provided hostname using getaddrinfo().
 * Assigns the resolved address to the destination address in the global ping structure.
 * Also extracts and stores the IPv4 address as a string in the ping structure.
 *
 * @param None.
 * @return None.
 */
void    setup_destination_address()
{
    struct addrinfo *res;
    char ip_str[INET_ADDRSTRLEN];

    if (getaddrinfo(g_ping->args->hostname, NULL, NULL, &res) != 0)
        show_errors(concatenate_strings("ft_ping: cannot resolve %s: Unknown host\n", g_ping->args->hostname), EX_NOHOST);
    g_ping->dest_addr = (struct sockaddr_in *)res->ai_addr;
    /* TO-DO: Extract the ipv4 address */
    inet_ntop(AF_INET, &(g_ping->dest_addr->sin_addr), ip_str, INET_ADDRSTRLEN);
    g_ping->ip_address = strdup(ip_str);
	freeaddrinfo(res);
    return ;
}

/** @Bief Ping Fuction
 * Sends ICMP packets to the target host and manages the packet exchange loop.
 * 
 * @param addr       Target host's socket address
 * @param hostname   Hostname of the target
 * @param icmp_sock  Raw ICMP socket
 * @return none
*/
void    icmp_echo()
{
    create_socket();
    setup_destination_address();
    if (g_ping->args->options & OPT_VERBOSE)
        printf("PING %s (%s): %ld data bytes, id 0x%x = %d\n", g_ping->args->hostname, g_ping->ip_address, sizeof(*g_ping->icmp_echo_header), getpid(), getpid());
    else
        printf("PING %s (%s): %ld data bytes\n", g_ping->args->hostname, g_ping->ip_address, sizeof(*g_ping->icmp_echo_header));
    gettimeofday(&g_ping->ping_data->start_time, NULL);
    while (g_ping->routine_loop) {
        send_icmp_packet();
		recv_icmp_packet();
        if (!g_ping->alarm)
            show_logs();
        g_ping->alarm = 0;
        if (g_ping->icmp_echo_header)
        {
            free(g_ping->icmp_echo_header);
            g_ping->icmp_echo_header = NULL;
        }
        update_rtt_stats();
        usleep(8900*100);
    }
    show_statistics();
	collect_memory();
    return ;
}

/**
 * @brief Initialize Ping Structure
 * Allocates memory for the global ping structure and initializes its fields.
 * The structure holds information related to ping operations, including IP address,
 * arguments, loop control, ICMP header, and destination address.
 *
 * @param None.
 * @return None.
 */
void    init_ping_struct()
{
    g_ping = (t_ping *)malloc(sizeof(t_ping));
    if (g_ping == NULL)
        show_errors("ERROR: can't allocate memory!", EX_OSERR);
    g_ping->ping_data = (t_ping_data *)malloc(sizeof(t_ping_data));
    if (g_ping->ping_data == NULL)
        show_errors("ERROR: can't allocate memory!", EX_OSERR);
    g_ping->ping_data->packets_received = 0;
    g_ping->ping_data->packets_transmitted = 0;
    g_ping->ping_data->rtt_min = 0;
    g_ping->ping_data->rtt_max = 0;
    g_ping->ping_data->rtt_total = 0;
    g_ping->ip_address = NULL;
    g_ping->args = NULL;
    g_ping->routine_loop = 1;
    g_ping->icmp_echo_header = NULL;
    g_ping->dest_addr = NULL;
    g_ping->sequence_number = 1;
    g_ping->ttl = 114;
    g_ping->rtt = 0;
    g_ping->alarm = 0;
    g_ping->bytes_received = 0;
    memset((void *)g_ping->recv_buffer, 0x00, sizeof(g_ping->recv_buffer));
    return ;
}

/** @Brief Main function
 * Parsing arguments and execute the main program
 * 
 * @param argc Number of arguments
 * @param argv CL arguments
 * @return
 */
int     main(int argc, char **argv) {
    if (argc <= 1)
    {
        printf("ft_ping: missing host operand\n");
        return(1);
    }
    init_ping_struct();
    /* TO-DO: Parsing the command line arguments */
    parse_clo(argc, argv);
    if (getuid() != 0)
        show_errors("ft_ping: Lacking privilige for icmp socket.\n", EX_NOPERM);
    //printf("hostname: %s, verbos: %d, help: %d\n", args->hostname, args->verbose, args->help);
    /* TO-DO: Handling signales */
    signal(SIGINT, (void *)&interrupt_handler);
    signal(SIGALRM, (void *)&interrupt_handler);
    /* TO-DO: sends icmp echo packets to targeted host */
    icmp_echo();
    return (0);
}
