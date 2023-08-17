#include "ft_ping.h"

/** @Brief Init Args
 * Initialize args struct
 *
 * @param none
 * @return t_args*
 */
t_args  *init_args( void )
{
    t_args *args;

    args = (t_args *) malloc(sizeof(t_args) * 1);
    if (args == NULL)
        return (args);
    args->invalid_option = 0;
    args->hostname = NULL;
    args->options = 0;
    args->ttl = DEFAULT_TTL;
    args->timeout = 1;
    args->packet_size = PING_PACKET_SIZE;
    args->count = 0;
    args->count_valid = 0;
    return (args);
}

void    check_args_validity()
{
    if (g_ping->args->options & OPT_COUNT)
    {
        if (g_ping->args->count > 0)
            g_ping->args->count_valid = 1;
    }
}

int     read_value(const char *string, int option)
{
    long value;
    char *endptr;
    
    value = strtol(string, &endptr, 10);
    if (*endptr != '\0')
        show_errors(concatenate_strings("ft_ping: invalid value (`%s' near `%s')\n", string,string), EXIT_FAILURE);
    if (errno == EINVAL)
        show_errors(concatenate_strings("Invalid conversion: %s\n", string), EXIT_FAILURE);
    if (options == OPT_TTL)
    {
        if ((errno == ERANGE && string[0] != '-') || value >= 256)
            show_errors(concatenate_strings("ping2: option value too big: %s\n", string), EXIT_FAILURE);
        else if (errno == ERANGE || value <= 0)
            show_errors(concatenate_strings("ping2: option value too small: %s\n", string), EXIT_FAILURE);
    }
    else if (options == OPT_TIMEOUT)
    {
        if (errno == ERANGE || (value < 0 && value > INT32_MAX))
            show_errors(concatenate_strings("ping2: option value too big: %s\n", string), EXIT_FAILURE);
        else if (value == 0)
            show_errors(concatenate_strings("ping2: option value too small: %s\n", string), EXIT_FAILURE);
    }
    else if (options == OPT_PACKETSIZE)
    {
        if (errno == ERANGE || (value >= 65400 && value < 0))
            show_errors(concatenate_strings("ping2: option value too big: %s\n", value), EXIT_FAILURE);
    }
    return (value);
}

/**
 * @brief Parsing command line
 * parse command line argumments 
 * and store valid arguments in t_args struct
 */
void    parse_clo(int len, char *clo_args[]) {
    int i;
    
    i = 1;
    g_ping->args = init_args();
    if (g_ping->args == NULL)
        show_errors("ft_ping: can't allocate memory\n", EX_OSERR);
    // TO-DO: Parsing only valid options and break if any invalid_option appears
    while (i < len)
    {
        if (strcmp(clo_args[i], "-?") == 0)
            g_ping->args->options |= OPT_HELP;
        else if (strcmp(clo_args[i], "-v") == 0)
            g_ping->args->options |= OPT_VERBOSE;
        else if (strcmp(clo_args[i], "-c") == 0)
        {
            g_ping->args->options |= OPT_COUNT;
            if (!clo_args[i + 1])
                show_errors("ft_ping: option requires an argument -- 'c'\n", EX_USAGE);
            g_ping->args->count = read_value(clo_args[i + 1]);
            i++;
        }
        else if (strcmp(clo_args[i], "-n") == 0)
            g_ping->args->options |= OPT_NUMERIC;
        else if (strcmp(clo_args[i], "--ttl") == 0)
        {
            g_ping->args->options |= OPT_TTL;
            if (!clo_args[i + 1])
                show_errors("ft_ping: option '--ttl' requires an argument\n", EX_USAGE);
            g_ping->args->ttl = read_value(clo_args[i + 1]);
            i++;
        }
        else if (strcmp(clo_args[i], "-w") == 0)
        {
            g_ping->args->options |= OPT_TIMEOUT;
            if (!clo_args[i + 1])
                show_errors("ft_ping: option requires an argument -- 'w'\n", EX_USAGE);
            g_ping->args->timeout = read_value(clo_args[i + 1]);
            i++;
        }
        else if (strcmp(clo_args[i], "-s") == 0)
        {
            g_ping->args->options |= OPT_PACKETSIZE;
            if (!clo_args[i + 1])
                show_errors("ft_ping: option requires an argument -- 's'\n", EX_USAGE);
            g_ping->args->packet_size = read_value(clo_args[i + 1], OPT_PACKETSIZE);
            i++;
        }
        else if (clo_args[i][0] != '-' && !g_ping->args->hostname)
            g_ping->args->hostname = strdup(clo_args[i]);
        else if (clo_args[i][0] == '-')
            show_errors(concatenate_strings("ft_ping: invalid option -- %s\n", &clo_args[i][1]), EX_USAGE);
        i++;
    }
    if (g_ping->args->options & OPT_HELP)
        show_errors("", EX_HELP);
    if (!g_ping->args->hostname)
        show_errors("ft_ping: missing host operand\n", EX_USAGE);
}
