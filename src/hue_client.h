#ifndef __NYX_HUE_CLIENT_H__
#define __NYX_HUE_CLIENT_H__


#include <string>


class hue_client_t
{
private:
	/// Philips Hue IP
	std::string _ip;
	/// Socket
	int _socket;

public:
	/**
	 * @brief Initialize hue client
	 * @param ip [in] : Philips hue bridge IP
	 */
	hue_client_t(const std::string& ip);

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
	 * @brief Connect the socket
	 * @returns true if the socket successfully connected to the hue bridge
	 */
	inline bool is_connected(void)const {return (this->_socket != -1);}

	/**
	 * @brief 
	 * @param json [in] : Send the JSON string to the hue
	 * @param lamp_n [in] : Lamp number
	 * @returns true if OK
	 */
	bool send_command(const char* json, const uint8_t lamp_n)const;
};

#endif /* __NYX_HUE_CLIENT_H__ */
