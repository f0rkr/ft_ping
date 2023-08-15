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
     printf("usage: ft_ping [-v?] [-v Verbose] [-? Help]\n");
     return ;
}

void 		formated_received_log()
{

}
/** @Brief Show Logs
 * Displays logs related to the ICMP packet exchange.
 *
 * @return none
 */
void show_logs()
{
	char ip_str[INET_ADDRSTRLEN];
   	char sender_ip_str[INET_ADDRSTRLEN];
    // struct icmphdr *icmp_header;
    struct ip *ip_header;

	ip_header = (struct ip *)g_ping->recv_buffer;

	inet_ntop(AF_INET, &(ip_header->ip_src), sender_ip_str, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &(ip_header->ip_src), ip_str, INET_ADDRSTRLEN);

	// icmp_header = (struct icmphdr *)(g_ping->recv_buffer + sizeof(struct ip));
	// if (icmp_header->type == ICMP_ECHOREPLY && icmp_header->code == 0) {
    //     g_ping->ping_data->packets_received++;
    // }
	g_ping->rtt = (g_ping->receive_time.tv_sec - g_ping->send_time.tv_sec) * 1000.0 + (g_ping->receive_time.tv_usec - g_ping->send_time.tv_usec) / 1000.0;
	printf("%d bytes from %s: icmp_seq=%d ", g_ping->bytes_received,ip_str, g_ping->sequence_number++);
	// if (g_ping->args->verbose)
	// 	printf("ident=%d ",g_ping->icmp_echo_header->icmp_header.identifier);
	printf("ttl=%d ", ip_header->ip_ttl);
	printf("time=%.3f ms\n", g_ping->rtt);
	g_ping->ping_data->packets_received++;
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
	dprintf(2, error_string);

	// free(error_string);
	// error_string = NULL;
	if (error_number == EX_USAGE)
		show_usage();
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
		printf("routine_trip min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", g_ping->ping_data->rtt_min, avg_rtt, g_ping->ping_data->rtt_max, 0.147);
	}
	return ;
}