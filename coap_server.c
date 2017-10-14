#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include <coap.h>

struct coap_resource_t *time_resource = NULL;
static coap_async_state_t *async = NULL;

#define INDEX "RFC's We Love CoAP Demo Server\n\n"

static void
handle_index_get(coap_context_t * ctx, struct coap_resource_t *resource, const coap_endpoint_t * local_interface,
		 coap_address_t * peer, coap_pdu_t * request, str * token, coap_pdu_t * response)
{
	unsigned char buf[3];

	response->hdr->code = COAP_RESPONSE_CODE(205);
	coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);
	coap_add_option(response, COAP_OPTION_MAXAGE, coap_encode_var_bytes(buf, 0x2ffff), buf);
	coap_add_data(response, strlen(INDEX), (unsigned char *)INDEX);
}

static void handle_hello_get(coap_context_t * ctx, struct coap_resource_t *resource,
                             const coap_endpoint_t * local_interface, coap_address_t * peer, coap_pdu_t * request,
                             str * token, coap_pdu_t * response)
{
	unsigned char buf[3];
	const char *response_data = "Hello World!";

    response->hdr->code = COAP_RESPONSE_CODE(205);
	coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);
	coap_add_data(response, strlen(response_data), (unsigned char *)response_data);
}

static void send_async_response(coap_context_t * ctx, const coap_endpoint_t * local_if)
{
	coap_pdu_t *response;
	unsigned char buf[3];
	const char *response_data = "Async Hello World!";
	size_t size = sizeof(coap_hdr_t) + 20;

	response = coap_pdu_init(async->flags & COAP_MESSAGE_CON, COAP_RESPONSE_CODE(205), 0, size);
	response->hdr->id = coap_new_message_id(ctx);
	if (async->tokenlen)
		coap_add_token(response, async->tokenlen, async->token);
	coap_add_option(response, COAP_OPTION_CONTENT_TYPE, coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);
	coap_add_data(response, strlen(response_data), (unsigned char *)response_data);

	coap_send(ctx, local_if, &async->peer, response);

	coap_delete_pdu(response);
	coap_async_state_t *tmp;
	coap_remove_async(ctx, async->id, &tmp);
	coap_free_async(async);
	async = NULL;
}

static void handle_async_get(coap_context_t * ctx, struct coap_resource_t *resource,
                             const coap_endpoint_t * local_interface, coap_address_t * peer, coap_pdu_t * request,
                             str * token, coap_pdu_t * response)
{
	async = coap_register_async(ctx, peer, request, COAP_ASYNC_SEPARATE | COAP_ASYNC_CONFIRM, (void *)"no data");
}

static void
handle_time_get(coap_context_t * ctx, struct coap_resource_t *resource, const coap_endpoint_t * local_interface,
		coap_address_t * peer, coap_pdu_t * request, str * token, coap_pdu_t * response)
{
	unsigned char buf[26];

    response->hdr->code = COAP_RESPONSE_CODE(205);

	if (coap_find_observer(resource, peer, token)) {
		coap_add_option(response, COAP_OPTION_OBSERVE, coap_encode_var_bytes(buf, ctx->observe), buf);
	}

	coap_add_option(response, COAP_OPTION_CONTENT_FORMAT, coap_encode_var_bytes(buf, COAP_MEDIATYPE_TEXT_PLAIN), buf);
	coap_add_option(response, COAP_OPTION_MAXAGE, coap_encode_var_bytes(buf, 0x01), buf);

	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	asctime_r(timeinfo, (char *)buf);

	coap_add_data(response, strlen(buf), buf);
}

int main(int argc, char **argv)
{
	coap_context_t *ctx;
	fd_set readfds;
	int result;

	coap_set_log_level(LOG_DEBUG);

	coap_address_t serv_addr;
	coap_address_init(&serv_addr);
	serv_addr.addr.sin.sin_family = AF_INET;
	serv_addr.addr.sin.sin_addr.s_addr = INADDR_ANY;
	serv_addr.addr.sin.sin_port = htons(5683);
	ctx = coap_new_context(&serv_addr);
	if (!ctx)
		exit(EXIT_FAILURE);

	coap_resource_t *r;

	r = coap_resource_init(NULL, 0, 0);
	coap_register_handler(r, COAP_REQUEST_GET, handle_index_get);
	coap_add_resource(ctx, r);

	r = coap_resource_init((unsigned char *)"hello", 5, 0);
	coap_register_handler(r, COAP_REQUEST_GET, handle_hello_get);
	coap_add_resource(ctx, r);

	r = coap_resource_init((unsigned char *)"async", 5, 0);
	coap_register_handler(r, COAP_REQUEST_GET, handle_async_get);
	coap_add_resource(ctx, r);

	r = coap_resource_init((unsigned char *)"time", 4, 0);
	coap_register_handler(r, COAP_REQUEST_GET, handle_time_get);
	r->observable = 1;
	coap_add_resource(ctx, r);
	time_resource = r;

	while (1) {
		FD_ZERO(&readfds);
		FD_SET(ctx->sockfd, &readfds);

		struct timeval timeout = { 1 };

		result = select(FD_SETSIZE, &readfds, 0, 0, &timeout);

		if (result < 0) {
			if (errno != EINTR)
				perror("select");
		} else if (result > 0) {
			if (FD_ISSET(ctx->sockfd, &readfds)) {
				coap_read(ctx);
			}
		} else {	// Timeout
			if (time_resource) {
				time_resource->dirty = 1;
			}
		}

		if (async) {
			sleep(5);
			send_async_response(ctx, ctx->endpoint);
		}

		coap_check_notify(ctx);
	}

}
