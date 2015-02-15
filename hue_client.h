#ifndef __NYX_HUE_CLIENT_H__
#define __NYX_HUE_CLIENT_H__


class hue_client_t
{
private:
	/// Philips Hue IP
	char* _ip;
	/// Socket
	int _socket;

public:
	/// Constructor
	hue_client_t(const char* ip);
	/// Destructor
	~hue_client_t(void);

	/// Connect the socket
	bool connect(void);
	/// Close the socket
	void close(void);

	/// Send a JSON string to the hue
	void send_command(const char* json)const;
};

#endif /* __NYX_HUE_CLIENT_H__ */
