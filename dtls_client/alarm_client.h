
#ifndef ALARM_CLIENT_H
#define ALARM_CLIENT_H

#include <stddef.h>
#include "coap_client.h"

typedef struct
{
    coap_client_t coap_client;
}
alarm_client_t;

int alarm_client_init(const char *priv_key_file_name,
                     const char *pub_key_file_name,
                     const char *access_file_name);
void alarm_client_deinit(void);
int alarm_client_create(alarm_client_t *client,
                       const char *host,
                       const char *port);
void alarm_client_destroy(alarm_client_t *client);

int alarm_client_get_status(alarm_client_t *client, char *buf, size_t len);
int alarm_client_post_status(alarm_client_t *client, char *buf, size_t len);

int alarm_client_get_enabled(alarm_client_t *client, char *buf, size_t len);
int alarm_client_post_enabled(alarm_client_t *client, char *buf, size_t len);

#endif
