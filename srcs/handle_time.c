#include "ft_ping.h"

void update_rtt_stats() {
    if (g_ping->ping_data->packets_received == 1) {
        g_ping->ping_data->rtt_min = g_ping->rtt;
        g_ping->ping_data->rtt_max = g_ping->rtt;
    } else {
        if (g_ping->rtt < g_ping->ping_data->rtt_min) {
            g_ping->ping_data->rtt_min = g_ping->rtt;
        }
        if (g_ping->rtt > g_ping->ping_data->rtt_max) {
            g_ping->ping_data->rtt_max = g_ping->rtt;
        }
    }
    g_ping->ping_data->rtt_total += g_ping->rtt;
}

void    show_help()
{
    printf("usage: ft_ping [-v?] [-v Verbose] [-? Help]\n");
    return ;
}