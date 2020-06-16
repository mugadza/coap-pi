#include <stdlib.h>
#include <stdio.h>
#include "alarm_server.h"

#define PUB_KEY_FILE_NAME   "../libs/coap_dtls/raw_keys/server_pub_key.txt"
#define PRIV_KEY_FILE_NAME  "../libs/coap_dtls/raw_keys/server_priv_key.txt"
#define ACCESS_FILE_NAME    "../libs/coap_dtls/raw_keys/server_access.txt"

int main(int argc, char **argv)
{
    alarm_server_t server = {0};
    int result = 0;

    if (argc != 3)
    {
        fprintf(stderr, "usage: server host port\n");
        fprintf(stderr, "    host: IP address or host name to listen on (0.0.0.0 to listen on all interfaces)\n");
        fprintf(stderr, "    port: port number to listen on\n");
        return EXIT_FAILURE;
    }
    result = alarm_server_init(PRIV_KEY_FILE_NAME, PUB_KEY_FILE_NAME, ACCESS_FILE_NAME);
    if (result < 0)
    {
        return EXIT_FAILURE;
    }
    result = alarm_server_create(&server,
                             argv[1],
                             argv[2]);
    if (result < 0)
    {
        alarm_server_deinit();
        return EXIT_FAILURE;
    }
    result = alarm_server_run(&server);
    alarm_server_destroy(&server);
    alarm_server_deinit();
    if (result < 0)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
