#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "alarm_client.h"

#define PUB_KEY_FILE_NAME   "../libs/coap_dtls/raw_keys/client_pub_key.txt"
#define PRIV_KEY_FILE_NAME  "../libs/coap_dtls/raw_keys/client_priv_key.txt"
#define ACCESS_FILE_NAME    "../libs/coap_dtls/raw_keys/client_access.txt"
#define PAYLOAD_LEN             128

int main(int argc, char **argv)
{
    alarm_client_t client = {0};
    char payload[PAYLOAD_LEN] = {0};
    int result = 0;

    if ( argc < 5 && argc > 6 )
    {
        fprintf(stderr, "usage: CoAP DTLS client ----- %d\n", argc);
        fprintf(stderr, "usage: CoAP DTLS client\n");
        fprintf(stderr, "    host: IP address or host name\n");
        fprintf(stderr, "    port: port number.\n");
        fprintf(stderr, "  method: server only suppport GET/POST only for available resouces.\n");
        fprintf(stderr, "     uri: the resource uri. currently supported are status and enabled.\n");
        fprintf(stderr, " payload: for POST method, the payload can be 1 or 0 to set the appropriate resource.\n");
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

    if (strcmp(argv[3], "POST") == 0)
    {
        memcpy(payload, argv[5], strlen(argv[5]));
        if (strcmp(argv[4], "status") == 0)
        {
            result = alarm_client_post_status(&client, payload, sizeof(payload));
        }        
        else if (strcmp(argv[4], "enabled") == 0)
        {
            result = alarm_client_post_enabled(&client, payload, sizeof(payload));
        }        
        else
        {
            fprintf(stderr, "Unknown resource uri, server only supports status/enabled: '%s'", argv[4]);
            alarm_client_deinit();
            return EXIT_FAILURE;
        }
    }
    else if (strcmp(argv[3], "GET") == 0)
    {
        if (strcmp(argv[4], "status") == 0)
        {
            result = alarm_client_get_status(&client, payload, sizeof(payload));
        }        
        else if (strcmp(argv[4], "enabled") == 0)
        {
            result = alarm_client_get_enabled(&client, payload, sizeof(payload));
        }        
        else
        {
            fprintf(stderr, "Unknown resource uri, server only supports status/enabled: '%s'", argv[4]);
            alarm_client_deinit();
            return EXIT_FAILURE;
        }    
    }
    else
    {
        fprintf(stderr, "The given method type in not supported, server only supports GET/POST: '%s'", argv[3]);
        alarm_client_deinit();
        return EXIT_FAILURE;
    }
        
    if (result < 0)
    {
        alarm_client_destroy(&client);
        alarm_client_deinit();
        return EXIT_FAILURE;
    }
    printf("alarm: '%s'\n", payload);
    
    alarm_client_destroy(&client);
    alarm_client_deinit();
    return EXIT_SUCCESS;
}
