#include "ft_ping.h"
// # include "globals.h"


// extern	t_ping	*g_ping;

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

/** @Brief Show Logs
 * Displays logs related to the ICMP packet exchange.
 *
 * @return none
 */
void show_logs()
{

	char ip_str[INET_ADDRSTRLEN];
    	char sender_ip_str[INET_ADDRSTRLEN];
    	struct ip *ip_header;

	ip_header = (struct ip *)g_ping->recv_buffer;

    	inet_ntop(AF_INET, &(ip_header->ip_src), sender_ip_str, INET_ADDRSTRLEN);
    	inet_ntop(AF_INET, &(ip_header->ip_src), ip_str, INET_ADDRSTRLEN);

	// icmp_header = (struct icmphdr *)(g_ping->recv_buffer + sizeof(struct ip));
	g_ping->rtt = (g_ping->receive_time.tv_sec - g_ping->send_time.tv_sec) * 1000.0 + (g_ping->receive_time.tv_usec - g_ping->send_time.tv_usec) / 1000.0;
	printf("\r%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%.1f ms\n",
	      g_ping->bytes_received,
	      ip_str,
	      inet_ntoa(ip_header->ip_src),
	      g_ping->sequence_number++,
	      ip_header->ip_ttl,
	      g_ping->rtt); // Replace with actual calculated time
	update_rtt_stats();
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
	printf("[COLLECT_MEMORY]\n");
	close(g_ping->sockfd);
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

	packet_loss_percent= 100.0 * (1.0 - (double)g_ping->ping_data->packets_received / g_ping->ping_data->packets_transmitted);
	printf("\n--- %s ping statistics ---\n", g_ping->args->hostname);
	printf("%d packets transmitted, %d received, %d%% packet loss, time %dms\n", g_ping->ping_data->packets_transmitted, g_ping->ping_data->packets_received, (int)packet_loss_percent, 100);
	if (g_ping->ping_data->packets_received > 0)
	{
		avg_rtt = g_ping->ping_data->rtt_total / g_ping->ping_data->packets_received;
		printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n", g_ping->ping_data->rtt_min, avg_rtt, g_ping->ping_data->rtt_max, 0.147);
	}
	return ;
}