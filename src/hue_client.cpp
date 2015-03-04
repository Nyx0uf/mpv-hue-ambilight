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


#define NYX_BUFSIZ 1024


hue_client_t::hue_client_t(const std::string& ip)
{
	this->_ip = ip;
	this->_socket = -1;
}

hue_client_t::~hue_client_t(void)
{
	this->close();
}

bool hue_client_t::connect(void)
{
	if (this->is_connected())
		return false;

	struct hostent* host = NULL;
	struct sockaddr_in sin;
	int ret = 0;

	host = gethostbyname(this->_ip.c_str());
	if (!host)
	{
		NYX_ERRLOG("[!] Failed to resolv %s\n", this->_ip.c_str());
		return false;
	}

	this->_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == this->_socket)
	{
		NYX_ERRLOG("[!] Failed to create socket\n");
		return false;
	}
	memset(&sin, 0x00, sizeof(sin));
	sin.sin_family = host->h_addrtype;
	memcpy(&sin.sin_addr, host->h_addr, host->h_length);
	sin.sin_port = htons(80);

	ret = ::connect(this->_socket, (struct sockaddr*)&sin, sizeof(sin));
	if (-1 == ret)
	{
		NYX_ERRLOG("[!] Failed to connect to %s:80\n", this->_ip.c_str());
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

bool hue_client_t::send_command(const char* json, const uint8_t lamp_n)const
{
	/**
	 * http://192.168.0.150/debug/clip.html
	 * /api/newdeveloper/lights/3/state
	 * {"on":true, "transitiontime":5, "sat":255, "bri":255,"xy":[1.0,0.3154]}
	 */
	if (!this->is_connected())
		return false;

	char buffer[NYX_BUFSIZ] = {0x00};
	const int size = snprintf(buffer, NYX_BUFSIZ, "PUT /api/newdeveloper/lights/%d/state HTTP/1.1\r\nHost:%s\r\nContent-type:application/json\r\nContent-Length:%lu\r\n\n%s", lamp_n, this->_ip.c_str(), strlen(json), json);
	ssize_t ret = send(this->_socket, buffer, (size_t)size, 0);
	return (ret != -1);
}
