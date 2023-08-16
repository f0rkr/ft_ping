# include "ft_ping.h"
// #include "globals.h"

/* @brief exporting the global struct */
// extern t_ping	*g_ping;


/** @Brief Socket Creation
 * Creates a raw socket for sending and receiving ICMP packets.
 *
 * @return none
 */
void create_socket()
{
	int on;

	on = 1;
	/* TO-DO: Create raw socket */
	g_ping->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (g_ping->sockfd < 0)
		show_errors("Error: creating raw socket failed!\n", EX_OSERR);
	struct timeval timeout;
    timeout.tv_sec = 1;  // Set the timeout to 5 seconds
    timeout.tv_usec = 0;
    if (setsockopt(g_ping->sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) == -1)
        return;
    if (setsockopt(g_ping->sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        perror("setsockopt");
        return ;
    }
    int bd = 1;
    if (setsockopt(g_ping->sockfd, SOL_SOCKET, SO_BROADCAST, &bd, sizeof(bd))==-1)
    {
        return ;
    }
	return;
}

uint16_t calculate_icmp_checksum(void *data, size_t length) {
    uint32_t sum = 0;
    const uint16_t *buf = data;

    while (length > 1) {
        sum += *buf++;
        length -= 2;
    }

    // Add the remaining byte, if present
    if (length > 0) {
        sum += *(uint8_t *)buf;
    }

    // Fold the carry bits to get a 16-bit result
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    // Take the one's complement of the sum
    return (uint16_t)~sum;
}

/**
 * @brief Construct ICMP Packet
 * Constructs an ICMP packet for ping operation, including setting up ICMP header,
 * IP header, and payload data. The constructed packet is stored in the global
 * ping structure's icmp_echo_header field.
 *
 * @param None.
 * @return None.
 */
void construct_icmp_packet()
{   
	g_ping->icmp_echo_header = (t_echo_packet *)malloc(sizeof(struct s_echo_packet));
    memset((void *)g_ping->icmp_echo_header->data, 0x00, sizeof(g_ping->icmp_echo_header->data));
    
   
    g_ping->icmp_echo_header->ip_header.ip_v_ihl = (4 << 4) | (sizeof(t_ip_header) >> 2);
    g_ping->icmp_echo_header->ip_header.tos = 0;
    g_ping->icmp_echo_header->ip_header.total_length = htons(PING_PACKET_SIZE);
    g_ping->icmp_echo_header->ip_header.id = htons(getpid());
    g_ping->icmp_echo_header->ip_header.flags_offset = 0;
    g_ping->icmp_echo_header->ip_header.ttl = g_ping->ttl;
    g_ping->icmp_echo_header->ip_header.protocol = IPPROTO_ICMP;
    g_ping->icmp_echo_header->ip_header.checksum = 0;
    g_ping->icmp_echo_header->ip_header.src_ip = inet_addr(SRC_ADDRESS);
    g_ping->icmp_echo_header->ip_header.dest_ip = inet_addr(g_ping->ip_address);
    g_ping->icmp_echo_header->ip_header.checksum = calculate_icmp_checksum((void *)&g_ping->icmp_echo_header->ip_header, sizeof(g_ping->icmp_echo_header->ip_header));
    g_ping->icmp_echo_header->icmp_header.type = ICMP_ECHO;
    g_ping->icmp_echo_header->icmp_header.checksum = 0;
    g_ping->icmp_echo_header->icmp_header.code = 0;
    g_ping->icmp_echo_header->icmp_header.identifier = getpid();
    g_ping->icmp_echo_header->icmp_header.sequence_number = g_ping->sequence_number;
	
	g_ping->icmp_echo_header->icmp_header.checksum= calculate_icmp_checksum((void *)&g_ping->icmp_echo_header->icmp_header, sizeof(g_ping->icmp_echo_header->icmp_header));
    
    return ;
}
/** @Brief Send ICMP Packet
 * Constructs and sends an ICMP packet to the target host.
 *
 * @return none
 */
void send_icmp_packet()
{
    // char *buffer;


    int bytes_sent;
	construct_icmp_packet();
    // buffer = (char *)g_ping->icmp_echo_header + sizeof(g_ping->icmp_echo_header->ip_header);
	// printf("%d\n", buffer[1]);
   // printf("ip header: %ld | icmp header: %ld | data: %ld\n", sizeof(g_ping->icmp_echo_header->ip_header), sizeof(g_ping->icmp_echo_header->icmp_header), sizeof(g_ping->icmp_echo_header->data));
	gettimeofday(&g_ping->send_time, NULL);
	bytes_sent = sendto(g_ping->sockfd, (char *)g_ping->icmp_echo_header, sizeof(*g_ping->icmp_echo_header), 0, (const struct sockaddr *)g_ping->dest_addr,  sizeof(*g_ping->dest_addr));
	if (bytes_sent < 0)
		show_errors("Error: can't send icmp packet!\n", EX_OSERR);
	(void)bytes_sent;
    g_ping->ping_data->packets_transmitted++;
	return ;
}

/** @Brief Receive ICMP Packet
 * Receives and processes incoming ICMP packets.
 *
 * @return none
 */
void recv_icmp_packet()
{
	struct msghdr msg;
    struct iovec iov[1];

    iov->iov_base = g_ping->recv_buffer;
    iov->iov_len = sizeof(g_ping->recv_buffer);

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    g_ping->bytes_received = recvmsg(g_ping->sockfd, &msg, 0);
    if (g_ping->bytes_received < 0)
        g_ping->alarm = 1;
    else
        gettimeofday(&g_ping->receive_time, NULL);
        // show_errors("Error receiving ICMP packet\n", EX_OSERR);
}
