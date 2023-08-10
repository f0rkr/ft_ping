#ifndef FT_PING_H
# define FT_PING_H

# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <signal.h>
# include <netinet/ip_icmp.h>
# include <netdb.h>
# include <sys/time.h>
# include <sysexits.h>
# include <errno.h>
# include <stdarg.h>

# define PING_PACKET_SIZE 64

/**
 * @brief arguments struct
 * Store command line arguments
 */
typedef struct s_args {
    char	*hostname;
    int		verbose;
    int		sweepincrsize;
    int     help;
    int     invalid_option;
} t_args;

typedef struct  s_ping_data {
    int packets_transmitted;
    int packets_received;
    double rtt_min;
    double rtt_max;
    double rtt_total;
}               t_ping_data;

/** @brief ICMP packet struct
 * Constructing ICMP packet header
 *
 * @param type ICMP packet starts with a type and code field. These fields indicate the purpose
 * of the ICMP packet.
 * @param code
 * @param checksum This filed is used to ensure the integrity of the ICMP packet. Before sending
 * the packet, we need to calculate the checksum based on the contents of the packet
 * @param identifier This field uniquely identify the ICMP Packet. it's usually set to the process
 * ID of the sender
 * @param sequence_number This field also uniquely identify the ICMP packet. it's incremented for
 * each sent packet
 */
typedef struct s_icmp_packet
{
    uint8_t     type; /* ICMP Type (e.g., 8 for Echo Request) */
    uint8_t     code; /* ICMP code (e.g., 0 for Echo Request) */
    uint16_t    checksum;  /* ICMP Checksum */
    uint16_t    identifier; /* Identifier for this ping process */
    uint16_t    sequence_number; /* Sequence number for this ping */
}   t_icmp_packet;

/** @brief ICMP echo packet struct
 * Construct the ICMP echo packet
 * 
 * @param icmp  ICMP header packet struct
 * @param data  32bit data left to complete the ICMP echo packet
*/
typedef struct s_echo_packet
{
    t_icmp_packet   icmp_header;
    char            data[PING_PACKET_SIZE - sizeof(t_icmp_packet)];
}               t_echo_packet;

typedef struct s_ping
{
    int                 sockfd;
    int                 ttl;
    double                 rtt;
    int                 bytes_received;
    int                 sequence_number;
    char                *ip_address;
    char                recv_buffer[PING_PACKET_SIZE];
    t_args              *args;
    uint8_t             routine_loop;
    t_echo_packet       *icmp_echo_header;
    struct sockaddr_in  *dest_addr;
    t_ping_data         *ping_data;
    struct timeval      send_time;
    struct timeval      receive_time;
}               t_ping;


extern t_ping   *g_ping;
/** @brief arguments parser
 * Parse all argument and store
 * valid data in t_args struct
 */
t_args  *parse_clo(int len, char *clo_args[]);
void    create_socket();
void    send_icmp_packet();
void    recv_icmp_packet();
void    show_logs();
void    show_errors(char *errorstr, int error_number);
void    show_statistics();
char    *concatenate_strings(const char *format, ...);
void    ft_bzero(void *s, size_t n);
void    update_rtt_stats();
#endif
