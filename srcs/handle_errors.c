#include "ft_ping.h"

void	ft_bzero(void *s, size_t n)
{
	unsigned char	*str;
	size_t			i;

	str = (unsigned char *)s;
	i = 0;
	if (n == 0)
		return ;
	while (i < n)
	{
		str[i] = '\0';
		i++;
	}
}

/**
 * @brief Concatenate Strings with Format
 * Concatenates a format string with provided arguments and returns the result.
 * Allocates memory for the concatenated string. Remember to free the allocated
 * memory after use.
 *
 * @param format Format string with optional format specifiers.
 * @param ...    Arguments corresponding to the format specifiers in the format string.
 * @return       Pointer to the concatenated string. NULL if memory allocation fails.
 */
char *concatenate_strings(const char *format, ...) {
    // Calculate the length of the formatted string
    va_list args;
    va_start(args, format);
    size_t length = vsnprintf(NULL, 0, format, args) + 1; // +1 for the null terminator
    va_end(args);

    // Allocate memory for the concatenated string
    char *result = (char *)malloc(length);
    if (result == NULL) {
        return NULL; // Memory allocation failed
    }

    // Format the string with the provided arguments
    va_start(args, format);
    vsnprintf(result, length, format, args);
    va_end(args);

    return result;
}

/** @brief Help function
 * Printing the usage of ft_ping
 * to the Stdout
 *
 * @param void
 * @return none
 */
 void   show_usage( void )
{
     printf("Try './ft_ping -?' or 'ping2 --help' for more information.\n");
     return ;
}

void 		formated_received_log()
{

}

void print_hex_dump(const void *data, size_t size) {
    const unsigned char *buf = (const unsigned char *)data;
	printf(" ");
    for (size_t i = 0; i < size; ++i) {
        printf("%02x", buf[i]);
		if (i % 2)
			printf(" ");
    }
    printf("\n");
}

void print_ip_header(const t_ip_header *ip_header) {
	char src_ip_str[INET_ADDRSTRLEN];
    char dest_ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip_header->src_ip, src_ip_str, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &ip_header->dest_ip, dest_ip_str, INET_ADDRSTRLEN);

    printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data\n");
    printf("%2u %2u  %02X %04x %04x%4u %04x  %02x  %02u %04x %4s  %4s\n",
           ip_header->ip_v_ihl >> 4, ip_header->ip_v_ihl & 0xF,
           ip_header->tos, ntohs(ip_header->total_length),
           ntohs(ip_header->id), ntohs(ip_header->flags_offset) >> 13,
           ntohs(ip_header->flags_offset) & 0x1FFF, ip_header->ttl,
           ip_header->protocol, ntohs(ip_header->checksum),
           src_ip_str,dest_ip_str);
}

void show_verbose(t_icmp_packet *icmp_header, t_ip_header *ip_header) {
	// print_hex_dump(icmp_header, icmp_size);
    // Print IP Header
    printf("IP Hdr Dump:\n");
    print_hex_dump(ip_header, sizeof(*ip_header));
	print_ip_header(ip_header);
    // Print ICMP Header
    printf("ICMP: type %d, code %d, size %d, id 0x%x, seq 0x%.4x\n",
           icmp_header->type, icmp_header->code, ntohs(ip_header->total_length),
           icmp_header->identifier, icmp_header->sequence_number);
}

char	*perform_reverse_dns(t_ip_header *ip_header)
{
	struct sockaddr_in sa;
    char *host;

	host = (char *)malloc(sizeof(char) * 1024);
	if (host == NULL)
		show_errors("ERROR: can't allocate memory!\n", EX_OSERR);

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = ip_header->src_ip;
   	getnameinfo((struct sockaddr *)&sa, sizeof(sa), host, 1024, NULL, 0, 0);
    
	return (host);
}

/** @Brief Show Logs
 * Displays logs related to the ICMP packet exchange.
 *
 * @return none
 */
void show_logs()
{
	char sender_ip_str[INET_ADDRSTRLEN];
	char *host_address;
    t_icmp_packet *icmp_header;
    t_ip_header *ip_header;
 
    ip_header = (t_ip_header *)g_ping->recv_buffer;
    inet_ntop(AF_INET, &(ip_header->src_ip), sender_ip_str, INET_ADDRSTRLEN);
	host_address = strdup(sender_ip_str);
	if (!(g_ping->args->options & OPT_NUMERIC))
		host_address = perform_reverse_dns(ip_header);
    icmp_header = (t_icmp_packet *)((char *)ip_header + sizeof(*ip_header));
	if (icmp_header->type != ICMP_ECHOREPLY)
	{
		switch (icmp_header->type) {
			case ICMP_DEST_UNREACH:
				printf("%ld bytes from %s (%s): ", sizeof(*icmp_header) + sizeof(*ip_header) + sizeof(icmp_header), host_address, sender_ip_str);
				printf("Destination Net Unreachable\n");
				break;
			// Add more cases for other ICMP types as needed
			case ICMP_ECHO:
				if (g_ping->args->options & OPT_VERBOSE)
					show_verbose(icmp_header, ip_header);
				g_ping->ping_data->packets_transmitted--;
				break;
			default:
		}
	} 
	else if (icmp_header->type == ICMP_ECHOREPLY) 
	{
		g_ping->rtt = (g_ping->receive_time.tv_sec - g_ping->send_time.tv_sec) * 1000.0 + (g_ping->receive_time.tv_usec - g_ping->send_time.tv_usec) / 1000.0;
		if (!(g_ping->args->options & OPT_QUIET))
		{
			printf("%ld bytes from %s: icmp_seq=%d ", sizeof(*icmp_header),sender_ip_str, icmp_header->sequence_number);
			printf("ttl=%d ", ip_header->ttl);
			printf("time=%.3f ms\n", g_ping->rtt);
		}
		if (icmp_header->type == ICMP_ECHOREPLY && icmp_header->code == 0) {
        	g_ping->ping_data->packets_received++;
   		}
	}
	free(host_address);
	host_address = NULL;
	return ;
}

/** @brief Collect Memory function
 * Free all memory allocation and initialize
 * the pointer values
 *
 * @param void
 * @return none
 */
void    collect_memory()
{
	if (g_ping->routine_loop == 0)
	{
		close(g_ping->sockfd);
		if (g_ping->icmp_echo_header)
		{
			free(g_ping->icmp_echo_header);
			g_ping->icmp_echo_header = NULL;
		}
		if (g_ping->dest_addr)
		{
			free(g_ping->dest_addr);
			g_ping->dest_addr = NULL;
		}
		if (g_ping->args->hostname)
		{
			free(g_ping->args->hostname);
			g_ping->args->hostname = NULL;
		}
		if(g_ping->args)
		{
			free(g_ping->args);
			g_ping->args = NULL;
		}
		if (g_ping->ip_address)
		{
			free(g_ping->ip_address);
			g_ping->ip_address = NULL;
		}
		if (g_ping->ping_data)
		{
			free(g_ping->ping_data);
			g_ping->ping_data = NULL;
		}
		if (g_ping)
		{
			free(g_ping);
			g_ping = NULL;
		}
	}
	return ;
}

/** @brief Print function
 * Printing the error type and exit
 * to the Stdout
 *
 * @param errorstr
 * @param error_number
 * @return none
 */
void    show_errors(char *error_string, int error_number)
{
	collect_memory();
	// TO-DO: print the error string
	dprintf(2, "%s", error_string);
	// free(error_string);
	// error_string = NULL;
	if (error_number == EX_USAGE)
		show_usage();
	if (error_number == EX_HELP)
		show_help();
	exit(error_number);
}


/** @Brief Show Statistics
 * Displays statistics about the ICMP packet exchange session.
 *
 * @return none
 */
void show_statistics()
{
	double packet_loss_percent;
	double avg_rtt;

    gettimeofday(&g_ping->ping_data->end_time, NULL);
    //int total_time_ms = (g_ping->ping_data->end_time.tv_sec - g_ping->ping_data->start_time.tv_sec) * 1000 +
//                        (g_ping->ping_data->end_time.tv_usec - g_ping->ping_data->start_time.tv_usec) / 1000;
	packet_loss_percent= 100.0 * (1.0 - (double)g_ping->ping_data->packets_received / g_ping->ping_data->packets_transmitted);
	printf("--- %s ping statistics ---\n", g_ping->args->hostname);
	//printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n", g_ping->ping_data->packets_transmitted, g_ping->ping_data->packets_received, (int)packet_loss_percent, total_time_ms);
	printf("%d packets transmitted, %d packets received, %d%% packet loss\n", g_ping->ping_data->packets_transmitted, g_ping->ping_data->packets_received, (int)packet_loss_percent);
	if (g_ping->ping_data->packets_received > 0)
	{
		avg_rtt = g_ping->ping_data->rtt_total / g_ping->ping_data->packets_received;
		printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", g_ping->ping_data->rtt_min, avg_rtt, g_ping->ping_data->rtt_max, 0.147);
	}
	return ;
}