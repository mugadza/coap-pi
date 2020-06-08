#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <coap/coap.h>

#define LED_PIN   0
#define PIR_PIN   7

static unsigned char* pStatus = "OFF";

static void helloHandler(coap_context_t* pContext, coap_resource_t* pResource,
						 const coap_endpoint_t* pLocalInterface, coap_address_t* pPeer,
						 coap_pdu_t* pRequest, coap_pdu_t* pToken, coap_pdu_t* pResponse)
{
	unsigned char buffer[3];
	const char* pResponseData = "{\"alarm\": {\"id\": \"DEV-10-147\", \"location\": \"Rondebosch, Cape Town\", \"status\": \"ON\"}}";			
	pResponse->hdr->code = COAP_RESPONSE_CODE(205);

	coap_add_option(pResponse, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buffer, COAP_MEDIATYPE_TEXT_PLAIN), buffer);
	coap_add_data(pResponse, strlen(pResponseData), (unsigned char*)pResponseData);
	
	if ( pRequest != NULL )
	{
		printf("request %s\n", pResource->uri.s);
	}
	else
	{
		printf("request - NONE\n");
	}
}


static void statusHandler(coap_context_t* pContext, coap_resource_t* pResource,
						 const coap_endpoint_t* pLocalInterface, coap_address_t* pPeer,
						 coap_pdu_t* pRequest, coap_pdu_t* pToken, coap_pdu_t* pResponse)
{
	unsigned char buffer[3];
	char pResponseData[256] = {0};
	sprintf(pResponseData, "{\"alarm\": {\"id\": \"DEV-10-147\", \"status\": %s}}", pStatus);			
	pResponse->hdr->code = COAP_RESPONSE_CODE(205);

	coap_add_option(pResponse, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buffer, COAP_MEDIATYPE_TEXT_PLAIN), buffer);
	coap_add_data(pResponse, strlen(pResponseData), (unsigned char*)pResponseData);
	
	if ( pRequest != NULL )
	{
		printf("request %s\n", pResource->uri.s);
	}
	else
	{
		printf("request - NONE\n");
	}
}

void motionSensorHandler(void)
{
	printf("-------- motion detected ---------\n");
	digitalWrite(LED_PIN, HIGH);
	pStatus = "ON";	
}

int main(int argc, char* argv[])
{
	coap_context_t* pContext;
	coap_address_t serverAddress;
	coap_resource_t* pHelloResource;
	coap_resource_t* pStatusResource;
	
	// Initialize CoAP server socket
	coap_address_init(&serverAddress);
	serverAddress.addr.sin.sin_family = AF_INET;
	serverAddress.addr.sin.sin_addr.s_addr = INADDR_ANY;
	serverAddress.addr.sin.sin_port = htons(5683);
	
	// create the context for the server
	pContext = coap_new_context(&serverAddress);
	if(!pContext)
	{
		exit(EXIT_FAILURE);
	}
	
	// Initialize coap hello resources for the server and register its handler
	pHelloResource = coap_resource_init((unsigned char*)"hello", 5, 0);
	coap_register_handler(pHelloResource, COAP_REQUEST_GET, helloHandler);
	coap_add_resource(pContext, pHelloResource);

	// Initialize coap hello resources for the server and register its handler
	pStatusResource = coap_resource_init((unsigned char*)"status", 6, 0);
	coap_register_handler(pStatusResource, COAP_REQUEST_GET, statusHandler);
	coap_add_resource(pContext, pStatusResource);
	
	// Setup and ISR for the sensor
	if(wiringPiSetup() < -1)
	{
		printf("wiringPi config failed!");
		exit(1);	
	}

	pinMode(LED_PIN, OUTPUT);
	//pinMode(PIR_PIN, INPUT);

	digitalWrite(LED_PIN,LOW);
	
	if(wiringPiISR(PIR_PIN, INT_EDGE_RISING, &motionSensorHandler) < -1)
	{
		printf("Unable to seup ISR on PIR pin!\n");
		exit(1);	
	}
	
	printf("---------sever running-------\n");
	while(1)
	{
		fd_set readFDS;
		FD_ZERO(&readFDS);
		FD_SET(pContext->sockfd, &readFDS);
		
		int result = select(FD_SETSIZE, &readFDS, 0, 0, NULL);
		
		if(result < 0)
		{
			exit(EXIT_FAILURE);
		}
		else if( result > 0 && ( FD_ISSET(pContext->sockfd, &readFDS) ) )
		{
			coap_read(pContext);
		}
		else
		{
			printf("\nselect() returned with no read.");
		}
	}
	
	return 0;
}


/*
	
	while(1)
	{
		if(digitalRead(PIR_PIN) == 0)
		{
			digitalWrite(LED_PIN, LOW);
			printf("|       no motion detected      |\n");
		}
		else
		{

		}
		delay(1000);
	}
	* 
	* */
	
//"{\"alarm\": {\"id\": \"DEV-10-147\", \"location\": \"Rondebosch, Cape Town\", \"status\": \"ON\"}}";
	
