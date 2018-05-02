#include "event2/http.h"
#include "event2/http_struct.h"
#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/dns.h"
#include "event2/thread.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/queue.h>
#include <event.h>

// gcc -g -Wall http_client_example.c -I/opt/ -I/opt/include -L/opt/lib -levent -L/opt/lib -levent_core 
// ./a.out http://www.baidu.com

void remoteReadCallback(struct evhttp_request* req, void* arg)
{
	fprintf(stderr, "< body:\n\n\n");

	ev_ssize_t len;
	struct evbuffer *evbuf;
	 
	// 	Returns the input buffer. 
	evbuf = evhttp_request_get_input_buffer(req);
	/*
	 * size_t evbuffer_get_length(const struct evbuffer *buf)	
	 * Returns the total number of bytes stored in the evbuffer.
	 *
	 * Parameters
	 * buf	pointer to the evbuffer
	 *
	 * Returns
	 * the number of bytes stored in the evbuffer
	 * */
	len = evbuffer_get_length(evbuf);
	fprintf(stderr, "< len:%zu\n", len);

	/*
	 * unsigned char* evbuffer_pullup(struct evbuffer *buf, ev_ssize_t 	size)		
	 * Makes the data at the beginning of an evbuffer contiguous.
	 *
	 * Parameters
	 * buf	the evbuffer to make contiguous
	 * size	the number of bytes to make contiguous, or -1 to make the entire buffer contiguous.
	 *
	 * Returns
	 * a pointer to the contiguous memory array
	 * */
	fwrite(evbuffer_pullup(evbuf, len), len, 1, stdout);
	/*
	 * int evbuffer_drain(struct evbuffer *buf, size_t 	len)		
	 * Remove a specified number of bytes data from the beginning of an evbuffer.
	 *
	 * Parameters
	 * buf	the evbuffer to be drained
	 * len	the number of bytes to drain from the beginning of the buffer
	 * Returns
	 * 0 on success, -1 on failure.
	 * */
	evbuffer_drain(evbuf, len);

	event_base_loopexit((struct event_base*)arg, NULL);
} 

int readHeaderDoneCallback(struct evhttp_request* remote_rsp, void* arg)
{
	fprintf(stderr, "< HTTP/1.1 %d %s\n", evhttp_request_get_response_code(remote_rsp), evhttp_request_get_response_code_line(remote_rsp));
	struct evkeyvalq* headers = evhttp_request_get_input_headers(remote_rsp);
	struct evkeyval* header;
	TAILQ_FOREACH(header, headers, next)
	{
		fprintf(stderr, "< %s: %s\n", header->key, header->value);
	}
	fprintf(stderr, "< \n");
	return 0;
}

void readChunkCallback(struct evhttp_request* remote_rsp, void* arg)
{
	char buf[4096];
    struct evbuffer* evbuf = evhttp_request_get_input_buffer(remote_rsp);
    int n = 0;
    while ((n = evbuffer_remove(evbuf, buf, 4096)) > 0)
	{
		fwrite(buf, n, 1, stdout);
	}
}

void remoteRequestErrorCallback(enum evhttp_request_error error, void* arg)
{
	fprintf(stderr, "request failed\n");
	event_base_loopexit((struct event_base*)arg, NULL);
}

void remoteConnectionCloseCallback(struct evhttp_connection* connection, void* arg)
{
	fprintf(stderr, "remote connection closed\n");
	event_base_loopexit((struct event_base*)arg, NULL);
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("usage:%s url", argv[1]);
		return 1;
	}
			    
	char* url = argv[1];
	struct evhttp_uri* uri = evhttp_uri_parse(url);
	if (!uri)
	{
		fprintf(stderr, "parse url failed!\n");
		return 1;
	}

	struct event_base* base = event_base_new();
	if (!base)
	{
		fprintf(stderr, "create event base failed!\n");
		return 1;
	}

	struct evdns_base* dnsbase = evdns_base_new(base, 1);
	if (!dnsbase)
	{
		fprintf(stderr, "create dns base failed!\n");
	}
	assert(dnsbase);

	/*
	 * struct evhttp_request* evhttp_request_new(void(*)(struct evhttp_request *, void *)	cb, void* arg)		
	 * Creates a new request object that needs to be filled in with the request parameters.
	 *
	 * The callback is executed when the request completed or an error occurred.
	 * */
	struct evhttp_request* request = evhttp_request_new(remoteReadCallback, base);

	/*
	 * void evhttp_request_set_header_cb( struct evhttp_request * , int(*)(struct evhttp_request *, void *)cb )		
	 * Register callback for additional parsing of request headers.
	 *
	 * Parameters
	 * cb	will be called after receiving and parsing the full header. It allows analyzing the header and possibly closing the connection by returning a value < 0.
	 * */
	evhttp_request_set_header_cb(request, readHeaderDoneCallback);

	/*
	 * void evhttp_request_set_chunked_cb(struct evhttp_request * , void(*)(struct evhttp_request *, void *))cb )		
	 * Enable delivery of chunks to requestor.
	 *
	 * Parameters
	 * cb	will be called after every read of data with the same argument as the completion callback. Will never be called on an empty response. 
	 * 		May drain the input buffer; it will be drained automatically on return.
	 * */
	//evhttp_request_set_chunked_cb(request, readChunkCallback);

	/*
	 * void evhttp_request_set_error_cb	(struct evhttp_request *, void(*)(enum evhttp_request_error, void *))		
	 * Set a callback for errors.
	 *
	 * See also
	 * evhttp_request_error for error types.
	 *
	 * On error, both the error callback and the regular callback will be called, error callback is called before the regular callback.
	 * */
	evhttp_request_set_error_cb(request, remoteRequestErrorCallback);

	/*
	 * const char* evhttp_uri_get_host(const struct evhttp_uri *uri)	
	 * Return the host part of an evhttp_uri, or NULL if it has no host set.
	 *
	 * The host may either be a regular hostname (conforming to the RFC 3986 "regname" production), or an IPv4 address, or the empty string, 
	 * or a bracketed IPv6 address, or a bracketed 'IP-Future' address.
	 *
	 * Note that having a NULL host means that the URI has no authority section, but having an empty-string host means that the URI has an authority 
	 * section with no host part. For example, "mailto:user@example.com" has a host of NULL, but "file:///etc/motd" has a host of "".
	 * */
	const char* host = evhttp_uri_get_host(uri);
	if (!host)
    {
    	fprintf(stderr, "parse host failed!\n");
		return 1;
	}

	/*
	 *  int evhttp_uri_get_port(const struct evhttp_uri *uri)	
	 *  Return the port part of an evhttp_uri, or -1 if there is no port set.
	 * */
	int port = evhttp_uri_get_port(uri);
	if (port < 0) port = 80;

	const char* request_url = url;
	/*
	 * Return the path part of an evhttp_uri, or NULL if it has no path set.
	 * */
	const char* path = evhttp_uri_get_path(uri);
	if (path == NULL || strlen(path) == 0)
	{
		request_url = "/";
	}

	printf("url:%s host:%s port:%d path:%s request_url:%s\n", url, host, port, path, request_url);

	/*
	 * struct evhttp_connection* evhttp_connection_base_new(struct event_base *base, struct evdns_base *dnsbase, const char *address, ev_uint16_t port)		
	 * Create and return a connection object that can be used to for making HTTP requests.
	 *
	 * The connection object tries to resolve address and establish the connection when it is given an http request object.
	 *
	 * Parameters
	 * base	    the event_base to use for handling the connection
	 * dnsbase	the dns_base to use for resolving host names; if not specified host name resolution will block.
	 * address	the address to which to connect
	 * port		the port to connect to
	 *
	 * Returns
	 * an evhttp_connection object that can be used for making requests
	 * */
	struct evhttp_connection* connection =  evhttp_connection_base_new(base, dnsbase, host, port);
	if (!connection)
	{
		fprintf(stderr, "create evhttp connection failed!\n");
	    return 1;
	}

	/*
	 * void evhttp_connection_set_closecb(struct evhttp_connection *evcon, void(*)(struct evhttp_connection *, void *), void *)		
	 * Set a callback for connection close.
	 * */
	evhttp_connection_set_closecb(connection, remoteConnectionCloseCallback, base);

	/*
	 *  int evhttp_add_header(struct evkeyvalq *headers, const char *key, const char *value)		
	 *  Adds a header to a list of existing headers.
	 *
	 *  Parameters
	 *  headers	the evkeyvalq object to which to add a header
	 *  key	    the name of the header
	 *  value	the value belonging to the header
	 *  Returns
	 *  0 on success, -1 otherwise.
	 * */
    evhttp_add_header(evhttp_request_get_output_headers(request), "Host", host);

	/*
	 * int evhttp_make_request(struct evhttp_connection *evcon, struct evhttp_request *req, enum evhttp_cmd_type type, const char *uri)		
	 *
	 * Make an HTTP request over the specified connection.
	 *
	 * The connection gets ownership of the request. On failure, the request object is no longer valid as it has been freed.
	 *
	 * Parameters
	 * evcon	the evhttp_connection object over which to send the request
	 * req		the previously created and configured request object
	 * type		the request type EVHTTP_REQ_GET, EVHTTP_REQ_POST, etc.
	 * uri		the URI associated with the request
	 * Returns
	 * 0 on success, -1 on failure
	 * */
    evhttp_make_request(connection, request, EVHTTP_REQ_GET, request_url);

    event_base_dispatch(base);

	evdns_base_free(dnsbase, 1);
	evhttp_connection_free(connection);
	event_base_free(base);
	evhttp_uri_free(uri);

    return 0;
}

