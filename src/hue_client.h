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
	/**
	 * @brief Initialize hue client
	 * @param ip [in] : Philips hue bridge IP
	 */
	hue_client_t(const char* ip);

	/**
	 * @brief Destructor, close socket and free ip
	 */
	~hue_client_t(void);

	/**
	 * @brief Connect the socket
	 * @returns true if the socket successfully connected to the hue bridge
	 */
	bool connect(void);

	/**
	 * @brief Close the socket
	 */
	void close(void);

	/**
	 * @brief 
	 * @param json [in] : Send the JSON string to the hue
	 */
	void send_command(const char* json)const;
};

#endif /* __NYX_HUE_CLIENT_H__ */
