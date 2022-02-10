/*
 * inet_protocols_driver.c
 *
 *  Created on: 22 dic. 2021
 *      Author: JoseCarlos
 */

/*INCLUDES*******************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "inet_protocols_driver.h"

/*GLOBAL VARIABLES***********************************************************************/
static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'};

static int mod_table[] = {0, 2, 1};

/*PROTOTYPES*****************************************************************************/
static char *base64_encode(char *data,size_t input_length,
		size_t *output_length);

/*FUNCTIONS******************************************************************************/
static char *base64_encode(char *data,size_t input_length,
		size_t *output_length)
{

    *output_length = 4 * ((input_length + 2) / 3);

    char *encoded_data = (char *)pvPortMalloc(*output_length);
    if (encoded_data == NULL) return NULL;

    for (int i = 0, j = 0; i < input_length;)
    {

        uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
        uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[input_length % 3]; i++)
        encoded_data[*output_length - 1 - i] = '=';

    return encoded_data;
}

#ifdef SMTP_PROTOCOL
inet_status_t connect_smtp(SSL **ssl, char *host, int port)
{
    int ret;
    SSL_CTX *ctx;
    int sockfd;
    struct sockaddr_in sock_addr;
    struct hostent *hp;
    struct ip4_addr *ip4_addr;

    hp = gethostbyname(host);
    if (!hp)
    	return error;

    ip4_addr = (struct ip4_addr *)hp->h_addr;
    ctx = SSL_CTX_new(TLSv1_1_client_method());

    if (!ctx)
    	return error;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    	return error;

    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = 0;
    sock_addr.sin_port = htons(port);
    ret = bind(sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret)
    	return error;

    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = ip4_addr->addr;
    sock_addr.sin_port = htons(port);
    ret = connect(sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret)
    	return error;

    (*ssl) = SSL_new(ctx);
    if (!(*ssl))
    	return error;

    SSL_set_fd((*ssl), sockfd);

    ret = SSL_connect((*ssl));
    if (!ret)
    	return error;

    char buffer[MAX_BUFFER_SIZE];

    memset(buffer, 0, MAX_BUFFER_SIZE);

    ret = SSL_read((*ssl), buffer, MAX_BUFFER_SIZE-1);
    if(ret<0)
    	return error;
    printf(buffer);


    sprintf(buffer, "HELO %s\r\n", host);

    ret = SSL_write((*ssl), buffer, strlen(buffer));
    if (ret <= 0)
    	return error;

    ret = SSL_read((*ssl), buffer, MAX_BUFFER_SIZE-1);
    if(ret<0)
    	return error;
    buffer[ret] = 0;
    printf(buffer);


    return ok;

}

inet_status_t open_smtp(SSL **ssl, char *user, char *pass)
{
	char buffer[MAX_BUFFER_SIZE];
	int len;

	memset(buffer, '\0', MAX_BUFFER_SIZE);
	strcpy(buffer, "auth login\r\n");
	len = SSL_write((*ssl), buffer, strlen(buffer));
	if(len<0)return operation_error;
	len = SSL_read((*ssl), buffer, MAX_BUFFER_SIZE-1);
	if(len<0)return operation_error;
    buffer[len] = 0;
    printf(buffer);

	size_t base64_len;
	char *base64_user = base64_encode(user, strlen(user), &base64_len);
	len = SSL_write((*ssl), base64_user, base64_len);
	vPortFree(base64_user);
	if(len<0)return operation_error;
	sprintf(buffer, "\r\n");
	len = SSL_write((*ssl), buffer, strlen(buffer));
	if(len<0)return operation_error;
	len = SSL_read((*ssl), buffer, MAX_BUFFER_SIZE-1);
	if(len<0)return operation_error;
    buffer[len] = 0;
    printf(buffer);

	char *base64_pass = base64_encode(pass, strlen(pass), &base64_len);
	len = SSL_write((*ssl), base64_pass, base64_len);
	vPortFree(base64_pass);
	if(len<0)return operation_error;
	sprintf(buffer, "\r\n");
	len = SSL_write((*ssl), buffer, strlen(buffer));
	if(len<0)return operation_error;
	len = SSL_read((*ssl), buffer, MAX_BUFFER_SIZE-1);
	if(len<0)return operation_error;
	buffer[len] = 0;
	printf(buffer);

	return ok;
}

inet_status_t send_email_smtp(SSL **ssl, char *sender, char *recipient, char *subject,
		char *data)
{
	int len;
	char buffer[MAX_BUFFER_SIZE];

	memset(buffer, '\0', MAX_BUFFER_SIZE);
	sprintf(buffer, "mail FROM:<%s>\r\n", sender);
	len = SSL_write((*ssl), buffer, strlen(buffer));
	if(len<0)return operation_error;
	len = SSL_read((*ssl), buffer, MAX_BUFFER_SIZE-1);
	if(len<0)return operation_error;
	buffer[len] = 0;
	printf(buffer);


	sprintf(buffer, "rcpt TO:<%s>\r\n", recipient);
	len = SSL_write((*ssl), buffer, strlen(buffer));
	if(len<0)return operation_error;
	len = SSL_read((*ssl), buffer, MAX_BUFFER_SIZE-1);
	if(len<0)return operation_error;
	buffer[len] = 0;
	printf(buffer);

	sprintf(buffer, "data\r\n");
	len = SSL_write((*ssl), buffer, strlen(buffer));
	if(len<0)return operation_error;
	len = SSL_read((*ssl), buffer, MAX_BUFFER_SIZE-1);
	if(len<0)return operation_error;
	buffer[len] = 0;
	printf(buffer);
	vTaskDelay(10);

	sprintf(buffer, "Subject: %s\r\n", subject);
	len = SSL_write((*ssl), buffer, strlen(buffer));
	if(len<0)
		return operation_error;
	sprintf(buffer, "%s\r\n", data);
	len = SSL_write((*ssl), buffer, strlen(buffer));
	if(len<0)return operation_error;

	strcpy(buffer, ".\r\n");
	len = SSL_write((*ssl), buffer, strlen(buffer));
	if(len<0)return operation_error;
	len = SSL_read((*ssl), buffer, MAX_BUFFER_SIZE-1);
	if(len<0)return operation_error;
	buffer[len] = 0;
	printf(buffer);


	return ok;
}
inet_status_t close_smtp(SSL **ssl)
{
	int len;
	char buffer[MAX_BUFFER_SIZE];

	memset(buffer, '\0', MAX_BUFFER_SIZE);
	strcpy(buffer, "quit\r\n");
	len = SSL_write((*ssl), buffer, strlen(buffer));
	if(len<0)return operation_error;
	len = SSL_read((*ssl), buffer, MAX_BUFFER_SIZE-1);
	if(len<0)return operation_error;
	buffer[len] = 0;
	printf(buffer);

	SSL_CTX_free((*ssl)->ctx);
	SSL_shutdown((*ssl));
	SSL_free((*ssl));
	(*ssl) = NULL;


	return ok;
}
#endif
