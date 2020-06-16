
#ifndef TIME_CLIENT_H
#define TIME_CLIENT_H

#include <stddef.h>
#include "coap_client.h"

typedef struct
{
    coap_client_t coap_client;
}
time_client_t;

int time_client_init(const char *priv_key_file_name,
                     const char *pub_key_file_name,
                     const char *access_file_name);
void time_client_deinit(void);
int time_client_create(time_client_t *client,
                       const char *host,
                       const char *port);
void time_client_destroy(time_client_t *client);
int time_client_get_time(time_client_t *client, char *buf, size_t len);

int alarm_client_post_status(time_client_t *client, char *buf, size_t len);

#endif
