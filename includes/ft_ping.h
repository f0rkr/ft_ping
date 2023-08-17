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

# define PING_PACKET_SIZE   56
# define DEFAULT_TTL        64


# define PING_PACKET_SIZE   56
# define DEFAULT_TTL        64

# define EX_HELP        0

#define OPT_HELP     (1 << 0)
#define OPT_VERBOSE  (1 << 1)
#define OPT_COUNT    (1 << 2)
#define OPT_TTL      (1 << 3)
#define OPT_NUMERIC  (1 << 4)
#define OPT_TIMEOUT  (1 << 5)
#define OPT_QUIET    (1 << 6)



/**
 * @brief arguments struct
 * Store command line arguments
 */
typedef struct s_args {
    char	*hostname;
    int     options;
    int     ttl;
    time_t     timeout;
    int     quite;
    int     count;
    int     count_valid;
    int     invalid_option;
} t_args;
/**
 * @brief IP Header struct
*/
typedef struct s_ip_header {
	uint8_t ip_v_ihl;
	uint8_t tos;
	uint16_t total_length;
	uint16_t id;
	uint16_t flags_offset;
	uint8_t  ttl;
	uint8_t	 protocol;
	uint16_t checksum;
	uint32_t	src_ip;
	uint32_t	dest_ip;
}		t_ip_header;

typedef struct s_ping_data {
    int packets_transmitted;
    int packets_received;
    double rtt_min;
    double rtt_max;
    double rtt_total;
    struct timeval start_time;
    struct timeval end_time;
}               t_ping_data;

typedef struct s_id_ts
{
    u_int32_t its_otime;
    u_int32_t its_rtime;
    u_int32_t its_ttime;
} t_id_ts;

typedef struct s_timeval
{
    time_t          tv_sec;
    suseconds_t     tv_usec;
}               t_timeval;

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
    uint8_t         type; /* ICMP Type (e.g., 8 for Echo Request) */
    uint8_t         code; /* ICMP code (e.g., 0 for Echo Request) */
    uint16_t        checksum;  /* ICMP Checksum */
    uint16_t        identifier; /* Identifier for this ping process */
    uint16_t        sequence_number; /* Sequence number for this ping */
    char            data[PING_PACKET_SIZE];
}   t_icmp_packet;

/** @brief ICMP echo packet struct
 * Construct the ICMP echo packet
 * 
 * @param icmp  ICMP header packet struct
 * @param data  32bit data left to complete the ICMP echo packet
*/
typedef struct s_echo_packet
{
    t_ip_header     ip_header;
    t_icmp_packet   icmp_header;
}               t_echo_packet;

typedef struct s_ping
{
    int                 sockfd;
    int                 ttl;
    double              rtt;
    int                 bytes_received;
    int                 sequence_number;
    char                *ip_address;
    char                recv_buffer[PING_PACKET_SIZE*100];
    t_args              *args;
    uint8_t             routine_loop;
    t_echo_packet       *icmp_echo_header;
    struct sockaddr_in  *dest_addr;
    t_ping_data         *ping_data;
    struct timeval      send_time;
    int                 alarm;
    struct timeval      receive_time;
}               t_ping;

/* Global Variable */

extern t_ping   *g_ping;

/** @brief arguments parser
 * Parse all argument and store
 * valid data in t_args struct
 */
void    parse_clo(int len, char *clo_args[]);
void    create_socket();
void    send_icmp_packet();
void    recv_icmp_packet();
void    show_logs();
void    show_errors(char *errorstr, int error_number);
void    show_statistics();
char    *concatenate_strings(const char *format, ...);
void    ft_bzero(void *s, size_t n);
void    update_rtt_stats();
void    collect_memory();
void    construct_icmp_packet();
void    show_help();
#endif
