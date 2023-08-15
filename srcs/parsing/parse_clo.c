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
    return (args);
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
        else if (clo_args[i][0] != '-' && !g_ping->args->hostname)
                g_ping->args->hostname = strdup(clo_args[i]);
        else if (clo_args[i][0] == '-')
            show_errors(concatenate_strings("ft_ping: invalid option -- %s\n", &clo_args[i][1]), EX_USAGE);
        i++;
    }
    if (!g_ping->args->hostname)
        show_errors("ft_ping: missing host operand\n", EX_USAGE);
    if (g_ping->args->options & OPT_HELP)
        show_errors("", EX_USAGE);
    return ;
}
