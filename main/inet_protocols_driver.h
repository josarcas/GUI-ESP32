/*
 * inet_protocols_driver.h
 *
 *  Created on: 22 dic. 2021
 *      Author: JoseCarlos
 */

#ifndef MAIN_INET_PROTOCOLS_DRIVER_H_
#define MAIN_INET_PROTOCOLS_DRIVER_H_

/*INCLUDES*******************************************************************************/
#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "openssl/ssl.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

/*DEFINES********************************************************************************/
#define SMTP_PROTOCOL
#define POP_PROTOCOL
#define SNTP_PROTOCOL

#define MAX_TX_BUFFER_SIZE		512
#define MAX_RX_BUFFER_SIZE		1024*2
#define MAX_BUFFER_SIZE			1024


#ifdef POP_PROTOCOL
#define	MAX_SENDER_SIZE_POP		50
#define MAX_DATE_SIZE_POP		20
#define MAX_SUBJECT_SIZE_POP	100
#define MAX_MESSAGE_SIZE_POP	512
#endif

/*TYPEDEFS*******************************************************************************/
typedef struct hostent hostnet_t;

typedef enum{
	error,
	ok,
	timeout,
	over_flow_buffer,
	operation_error,

}inet_status_t;

/*
typedef enum{
	ip 		= IPPROTO_IP,
	icmp 	= IPPROTO_ICMP,
	tcp		= IPPROTO_TCP,
	udp		= IPPROTO_UDP

}network_protocol_t;
*/


#ifdef POP_PROTOCOL
typedef struct{
	char sender[MAX_SENDER_SIZE_POP];
	char date[MAX_DATE_SIZE_POP];
	char subject[MAX_SUBJECT_SIZE_POP];
	char message[MAX_MESSAGE_SIZE_POP];

}pop_mail_t;
#endif

/*PROTOTYPES*****************************************************************************/
#ifdef SMTP_PROTOCOL
inet_status_t connect_smtp(SSL **ssl, char *host, int port);
inet_status_t open_smtp(SSL **ssl, char *user, char *pass);
inet_status_t send_email_smtp(SSL **ssl, char *sender, char *recipient, char *subject,
		char *data);
inet_status_t close_smtp(SSL ** ssl);
#endif

#ifdef POP_PROTOCOL
inet_status_t connect_pop(char *host, char *port, uint32_t timeout);
inet_status_t open_pop(char *user, char *pass, uint32_t timeout);
//inet_status_t check_inbox_pop(uint32_t timeout);
inet_status_t status_pop(uint16_t *n_messages, uint16_t *size);
inet_status_t read_email_pop(uint8_t id, pop_mail_t *mail, uint32_t timeout);
inet_status_t delete_message_pop(uint8_t id, uint32_t timeout);
inet_status_t close_pop(uint32_t timeout);
#endif

#endif /* MAIN_INET_PROTOCOLS_DRIVER_H_ */
