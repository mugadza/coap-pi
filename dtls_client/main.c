#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "alarm_client.h"

#define PUB_KEY_FILE_NAME   "../libs/coap_dtls/raw_keys/client_pub_key.txt"
#define PRIV_KEY_FILE_NAME  "../libs/coap_dtls/raw_keys/client_priv_key.txt"
#define ACCESS_FILE_NAME    "../libs/coap_dtls/raw_keys/client_access.txt"
#define BUF_LEN             256

int main(int argc, char **argv)
{
    alarm_client_t client = {0};
    char buf[BUF_LEN] = {0};
    int result = 0;

    if (argc != 3)
    {
        fprintf(stderr, "usage: client host port\n");
        fprintf(stderr, "    host: IP address or host name to connect to\n");
        fprintf(stderr, "    port: port number to connect to\n");
        return EXIT_FAILURE;
    }
    result = alarm_client_init(PRIV_KEY_FILE_NAME, PUB_KEY_FILE_NAME, ACCESS_FILE_NAME);
    if (result < 0)
    {
        return EXIT_FAILURE;
    }
    result = alarm_client_create(&client, argv[1], argv[2]);
    if (result < 0)
    {
        alarm_client_deinit();
        return EXIT_FAILURE;
    }

    //memcpy(buf, argv[3], len);
    memcpy(buf, "1", 2);
    //ret = alarm_client_get_status(&client, buf, sizeof(buf));
    result = alarm_client_post_status(&client, buf, sizeof(buf));
    if (result < 0)
    {
        alarm_client_destroy(&client);
        alarm_client_deinit();
        return EXIT_FAILURE;
    }
    printf("alarm: '%s'\n", buf);
    
    alarm_client_destroy(&client);
    alarm_client_deinit();
    return EXIT_SUCCESS;
}
