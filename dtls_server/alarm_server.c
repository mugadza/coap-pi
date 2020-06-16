#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "alarm_server.h"
#include "coap_msg.h"
#include "coap_mem.h"
#include "coap_log.h"
#include "raw_keys.h"

#define ALARM_SERVER_URI_PATH_BUF_LEN  32
#define ALARM_SERVER_PAYLOAD_BUF_LEN   256 
#define ALARM_SERVER_SMALL_BUF_NUM     128 
#define ALARM_SERVER_SMALL_BUF_LEN     256 
#define ALARM_SERVER_MEDIUM_BUF_NUM    128 
#define ALARM_SERVER_MEDIUM_BUF_LEN    1024
#define ALARM_SERVER_LARGE_BUF_NUM     32                  
#define ALARM_SERVER_LARGE_BUF_LEN     8192

volatile char* pStatus = "OFF";
volatile int enabled = 0;

void update_status_state(int data, char* pResponseData)
{
    if ( data < 0 )
	{
		sprintf(pResponseData, "%s", "{\"alarm\": {\"id\": \"DEV-10-147\", \"error\": POST FAILED,\
				\"message\": \"unkown setting for status field\"}}");
        return;
	}
    
	else if ( data == 0 )
	{
		printf("-------- Manually switched off the alarm ---------\n");
//		digitalWrite(LED_PIN, LOW);
		pStatus = "OFF";	
	}
	else
	{
		printf("-------- Manually switched on the alarm ---------\n");
//		digitalWrite(LED_PIN, HIGH);
		pStatus = "ON";	
	}
    
    sprintf(pResponseData, "{\"alarm\": {\"id\": \"DEV-10-147\", \"status\": %s}}", pStatus);
}

void update_enabled(int data, char* pResponseData)
{
    if ( data != 0 && data != 1 )
	{
		sprintf(pResponseData, "%s", "{\"alarm\": {\"id\": \"DEV-10-147\", \"error\": POST FAILED,\
				\"message\": \"unkown setting for enabled field\"}}");
        return;
	}
    enabled = data;	
	
    sprintf(pResponseData, "{\"alarm\": {\"id\": \"DEV-10-147\", \"enabled\": %s}}", (enabled == 1) ? "true" : "false");
}

int set_response_uri_and_payload(char* payload_buf, int uri_path_len, char* resource_path,coap_msg_t *pResponse)
{
    int result = 0;
    
    result = coap_msg_add_op(pResponse, COAP_MSG_URI_PATH, uri_path_len, resource_path);
    if (result < 0)
    {
        coap_log_error("Failed to set URI path in response message. URI: %s", resource_path);
        return result;
    }
    result = coap_msg_set_payload(pResponse, payload_buf, strlen(payload_buf));
    if (result < 0)
    {
        coap_log_error("Failed to set payload in response message. URI: %s", resource_path);
        return result;
    }
    return result;
}

static int alarm_server_handle_status(coap_server_trans_t *pTransaction, coap_msg_t *pRequest, coap_msg_t *pResponse)
{
    unsigned code_detail = 0;
    char pResponseData[ALARM_SERVER_PAYLOAD_BUF_LEN] = {0};
    int result = 0;

    code_detail = coap_msg_get_code_detail(pRequest);
    if (code_detail == COAP_MSG_GET)
    {
        coap_log_info("Received request method: GET /status");
        
        sprintf(pResponseData, "{\"alarm\": {\"id\": \"DEV-10-147\", \"status\": %s}}", pStatus);
    }
    else if (code_detail == COAP_MSG_POST)
    {
        coap_log_info("Received request method: POST /status");
        
        const char* pPayload = coap_msg_get_payload(pRequest);
        size_t payload_size = coap_msg_get_payload_len(pRequest);
        if ((pPayload == NULL) || (payload_size == 0))
        {
            coap_log_warn("Received request message without payload");
            return coap_msg_set_code(pResponse, COAP_MSG_CLIENT_ERR, COAP_MSG_BAD_REQ);
        }
        
        char data[1] = {0};
        
        memcpy(data, pPayload, payload_size);
        memset(data + payload_size, 0, sizeof(data) - payload_size);
        coap_log_info("Received request payload: '%s'", data);

        update_status_state(atoi(data), pResponseData);
    }
    else
    {
        coap_log_warn("Received request message with unsupported code detail: %d", pResponseData);
        return coap_msg_set_code(pResponse, COAP_MSG_SERVER_ERR, COAP_MSG_NOT_IMPL);
    }
        
    coap_msg_set_code(pResponse, COAP_MSG_SUCCESS, COAP_MSG_CONTENT);        
    result = set_response_uri_and_payload(pResponseData, 6, "status", pResponse);
    
    if (result < 0)
    {
        coap_log_error("Failed to set payload and URI in response message");
        return result;
    }
    coap_log_info("Sent response with payload: '%s'", pResponseData);
    return 0;
}

static int alarm_server_handle_enabled(coap_server_trans_t *pTransaction, coap_msg_t *pRequest, coap_msg_t *pResponse)
{
    unsigned code_detail = 0;
    char pResponseData[ALARM_SERVER_PAYLOAD_BUF_LEN] = {0};
    int result = 0;

    code_detail = coap_msg_get_code_detail(pRequest);
    if (code_detail == COAP_MSG_GET)
    {
        coap_log_info("Received request method: GET /enabled");
        
        sprintf(pResponseData, "{\"alarm\": {\"id\": \"DEV-10-147\", \"enabled\": %s}}", (enabled == 1) ? "true" : "false");
    }
    else if (code_detail == COAP_MSG_POST)
    {
        coap_log_info("Received request method: POST /enabled");
        
        const char* pPayload = coap_msg_get_payload(pRequest);
        size_t payload_size = coap_msg_get_payload_len(pRequest);
        if ((pPayload == NULL) || (payload_size == 0))
        {
            coap_log_warn("Received request message without payload");
            return coap_msg_set_code(pResponse, COAP_MSG_CLIENT_ERR, COAP_MSG_BAD_REQ);
        }
        
        char data[1] = {0};
        
        memcpy(data, pPayload, payload_size);
        memset(data + payload_size, 0, sizeof(data) - payload_size);
        coap_log_info("Received request payload: '%s'", data);

        update_enabled(atoi(data), pResponseData);
    }
    else
    {
        coap_log_warn("Received request message with unsupported code detail: %d", pResponseData);
        return coap_msg_set_code(pResponse, COAP_MSG_SERVER_ERR, COAP_MSG_NOT_IMPL);
    }
        
    coap_msg_set_code(pResponse, COAP_MSG_SUCCESS, COAP_MSG_CONTENT);        
    result = set_response_uri_and_payload(pResponseData, 7, "enabled", pResponse);
    
    if (result < 0)
    {
        coap_log_error("Failed to set payload and URI in response message");
        return result;
    }
    coap_log_info("Sent response with payload: '%s'", pResponseData);
    return 0;
}


static int alarm_server_handle(coap_server_trans_t* pTransaction, coap_msg_t* pRequest, coap_msg_t* pResponse)
{
    char uri_path_buf[ALARM_SERVER_URI_PATH_BUF_LEN] = {0};

    if (coap_msg_get_ver(pRequest) != COAP_MSG_VER)
    {
        coap_log_error("Received request message with invalid version: %d", coap_msg_get_ver(pRequest));
        return -EBADMSG;
    }
    coap_msg_uri_path_to_str(pRequest, uri_path_buf, sizeof(uri_path_buf));
    
    coap_log_info("Received request URI path: '%s'", uri_path_buf);
    if (strcmp(uri_path_buf, "/status") == 0)
    {
        return alarm_server_handle_status(pTransaction, pRequest, pResponse);
    }
    else if (strcmp(uri_path_buf, "/enabled") == 0)
    {
        return alarm_server_handle_enabled(pTransaction, pRequest, pResponse);
    }
    else
    {
        coap_log_warn("URI path not recognised");
        return coap_msg_set_code(pResponse, COAP_MSG_CLIENT_ERR, COAP_MSG_NOT_FOUND);
    }
}

int alarm_server_init(const char* pPrivateKeyFilename, const char* pPublicKeyFilename, const char* pAccessFilename)
{
    int result = 0;
    coap_log_set_level(COAP_LOG_INFO);
    result = coap_mem_all_create(ALARM_SERVER_SMALL_BUF_NUM, ALARM_SERVER_SMALL_BUF_LEN,
                                 ALARM_SERVER_MEDIUM_BUF_NUM, ALARM_SERVER_MEDIUM_BUF_LEN,
                                 ALARM_SERVER_LARGE_BUF_NUM, ALARM_SERVER_LARGE_BUF_LEN);
    if (result < 0)
    {
        coap_log_error("%s", strerror(-result));
        return -1;
    }
    
    result = raw_keys_load(pPrivateKeyFilename, pPublicKeyFilename, pAccessFilename);
    
    if (result < 0)
    {
        coap_log_error("Unable to load raw public keys");
        coap_mem_all_destroy();
        return -1;
    }
    
    return 0;
}

void alarm_server_deinit(void)
{
    coap_mem_all_destroy();
}

int alarm_server_create(alarm_server_t* pServer, const char *pHost, const char *pPort)
{
    int result = 0;
    memset(pServer, 0, sizeof(alarm_server_t));

    result = coap_server_create(&pServer->coap_server,
                             alarm_server_handle, pHost, pPort,
                             raw_keys_get_ecdsa_priv_key(),
                             raw_keys_get_ecdsa_pub_key_x(),
                             raw_keys_get_ecdsa_pub_key_y(),
                             raw_keys_get_ecdsa_access_x(),
                             raw_keys_get_ecdsa_access_y(),
                             raw_keys_get_ecdsa_access_num(),
                             RAW_KEYS_ECDSA_KEY_LEN);

    if (result < 0)
    {
        if (result != -1)
        {
            coap_log_error("%s", strerror(-result));
        }
        memset(pServer, 0, sizeof(alarm_server_t));
    }
    return result;
}

void alarm_server_destroy(alarm_server_t* pServer)
{
    coap_server_destroy(&pServer->coap_server);
    memset(pServer, 0, sizeof(alarm_server_t));
}

int alarm_server_run(alarm_server_t* pServer)
{
    int result = 0;

    result = coap_server_run(&pServer->coap_server);
    if ( (result < 0) && (result != -1) )
    {
        coap_log_error("%s", strerror(-result));
    }
    return result;
}
