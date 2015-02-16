#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "hue_client.h"
#include "global.h"


#define NYX_BUFSIZ 2048


hue_client_t::hue_client_t(const char* ip)
{
	const size_t len = strlen(ip);
	this->_ip = (char*)calloc(len + 1, sizeof(char));
	memcpy(this->_ip, ip, len);
	this->_socket = -1;
}

hue_client_t::~hue_client_t(void)
{
	this->close();
	free(this->_ip);
}

bool hue_client_t::connect(void)
{
	struct hostent* host = NULL;
	struct sockaddr_in sin;

	if (NULL == (host = gethostbyname(this->_ip)))
	{
		NYX_ERRLOG("[!] Can't resolv %s\n", this->_ip);
		return false;
	}

	if (-1 == (this->_socket = ::socket(AF_INET, SOCK_STREAM, 0)))
	{
		NYX_ERRLOG("[!] Unable to create socket\n");
		return false;
	}
	memset(&sin, 0x00, sizeof(sin));
	sin.sin_family = host->h_addrtype;
	memcpy((char*)&sin.sin_addr, host->h_addr, host->h_length);
	sin.sin_port = htons(80);

	if (-1 == ::connect(this->_socket, (struct sockaddr*)&sin, sizeof(sin)))
	{
		NYX_ERRLOG("[!] Unable to connect to %s:80\n", this->_ip);
		this->close();
		return false;
	}

	return true;
}

void hue_client_t::close(void)
{
	if (this->_socket != -1)
	{
		::close(this->_socket);
		this->_socket = -1;
	}
}

void hue_client_t::send_command(const char* json)const
{
/*
 http://192.168.0.150/debug/clip.html
 /api/newdeveloper/lights/3/state
 {"on":true, "transitiontime":5, "sat":255, "bri":255,"xy":[1.0,0.3154]}
 */
	char buffer[NYX_BUFSIZ] = {0x00};
	sprintf(buffer, "PUT /api/newdeveloper/lights/1/state HTTP/1.1\r\nHost: %s\r\nContent-type: 	application/json\r\nContent-Length: %lu\r\n\n%s", this->_ip, strlen(json), json);
	send(this->_socket, buffer, strlen(buffer), 0);
}
