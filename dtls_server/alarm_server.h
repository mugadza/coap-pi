#ifndef ALARM_SERVER_H
#define ALARM_SERVER_H

#include "coap_server.h"

typedef struct
{
    coap_server_t coap_server;
}
alarm_server_t;

void setup_raspberry_pi();
int alarm_server_init(const char* pPrivateKeyFilename, const char* pPublicKeyFilename, const char* pAccessFilename);
void alarm_server_deinit(void);
int alarm_server_create(alarm_server_t *pServer, const char *pHost, const char *pPort);
void alarm_server_destroy(alarm_server_t* pServer);
int alarm_server_run(alarm_server_t* pServer);

#endif
