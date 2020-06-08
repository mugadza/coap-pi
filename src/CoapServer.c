#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <coap/coap.h>
#include <coap/option.h>

#define LED_PIN   0
#define PIR_PIN   7

static int printOptions(coap_pdu_t* pRequest, coap_opt_iterator_t* pOptOterator)
{
	int status = 1;
	coap_opt_t* pOptions;
	while (pOptions = coap_option_next(pOptIterator))
	{
		if ( pOptIterator->type != COAP_OPTION_URI_QUERY )
		{
			continue;
		}
		
		char xBuffer[128] = {0};
		strncpy(xBuffer, COAP_OPT_VALUE(pOptions), COAP_OPT_LENGTH(pOptions));
		printf("option len %d\n      %s\n", COAP_OPT_LENGTH(pOptions), xBuffer);
		status = 1;
		break;
	}
	
	return status;
}

static void helloHandler(coap_context_t* pContext, coap_resource_t* pResource,
						 const coap_endpoint_t* pLocalInterface, coap_address_t* pPeer,
						 coap_pdu_t* pRequest, coap_pdu_t* pToken, coap_pdu_t* pResponse)
{
	static int count = 0;
	unsigned char buffer[3];
	const char* pResponseData = "{\"alarm\": {\"id\": \"DEV-10-147\", \"location\": \"Rondebosch, Cape Town\", \"status:\": \"ON\"}}";
	char responseBuffer[256] = {0};
	
	pResponse->hdr->code = COAP_RESPONSE_CODE(205);
	coap_add_option(pResponse, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buffer, COAP_MEDIATYPE_TEXT_PLAIN), buffer);
	
	sprintf(responseBuffer, pResponseData, count);
	coap_add_data(pResponse, strlen(responseBuffer), (unsigned char*)responseBuffer);
	count++;
	
	if ( pRequest != NULL )
	{
		coap_opt_iterator_t optIterator;
		printf("request %s\n", pResource->uri.s);
		coap_option_iterator_init(pRequest, &optIterator, COAP_OPT_ALL);
		
		while(printOptions(pRequest, &optIterator));
	}
	else
	{
		printf("request - NONE\n");
	}
}


int main(int argc, char* argv[])
{
	coap_context_t* pContext;
	coap_address_t serverAddress;
	coap_resource_t* pHelloResource;
	
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
	
	// Initialize coap resources for the server and register its handler
	pHelloResource = coap_resource_init((unsigned char*)"hello", 5, 0);
	coap_register_handler(pHelloResource, COAP_REQUEST_GET, helloHandler);
	coap_add_resource(pContext, pHelloResource);
	
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
			printf("---------dispatching read-------\n");
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
	if(wiringPiSetup() == -1)
	{
		printf("wiringPi config failed!");
		exit(1);	
	}

	pinMode(LED_PIN, OUTPUT);
	pinMode(PIR_PIN, INPUT);

	digitalWrite(LED_PIN,LOW);

	while(1)
	{
		if(digitalRead(PIR_PIN) == 0)
		{
			digitalWrite(LED_PIN, LOW);
			printf("|       no motion detected      |\n");
		}
		else
		{
			printf("|       motion detected         |\n");
			digitalWrite(LED_PIN, HIGH);
		}
		delay(1000);
	}
	* 
	* */
	
