/*                                                                    Server.hpp
################################################################################
# Encoding: UTF-8                                                  Tab size: 4 #
#                                                                              #
#      IMPLEMENTATION OF NAKED SOCKET COMMUNICATION WITH A REMOTE SERVER       #
#                                                                              #
# Ordnung muss sein!                             Copyright (C) 2019, azarus.io #
################################################################################
*/
# pragma	once
# include	<sys/time.h>
# include	<netdb.h>
# include	<string>

# define	IN_PROGRESS			(-1)

//****************************************************************************//
/// @brief		TODO: Write description
/// @details	TODO: Write description
//****************************************************************************//
class Server {

private:

	// Connection action timestamps
	mutable timeval	conn_start;		///< TODO: Write description
	mutable timeval	conn_end;		///< TODO: Write description

	// Sending action timestamps
	mutable timeval	send_start;		///< TODO: Write description
	mutable timeval	send_end;		///< TODO: Write description

	// Receiving action timestamps
	mutable timeval	recv_start;		///< TODO: Write description
	mutable timeval	recv_end;		///< TODO: Write description

	// Request and response strings
	std::string	request_str;		///< TODO: Write description
	std::string	response_str;		///< TODO: Write description

	// Request buffer position
	size_t	request_pos;			///< TODO: Write description

	// File descriptors
	int		epoll_fd;				///< TODO: Write description
	int		socket_fd;				///< TODO: Write description

	// Debug flag
	bool	debug_mode;				///< TODO: Write description

public:

//============================================================================//
//			Constructor and destructor
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Default constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	Server (void);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Destructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	~Server (void);

//============================================================================//
//			Methods
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Start a new connection to a server
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	void Connect (addrinfo server, int epoll, bool debug);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Drop existing connection to a server
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	void Disconnect (void);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Set request string to send to a server
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	void SetRequest (const std::string &request);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Send request to a server
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	void Send (void);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Receive response from a server
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	int Recv (void);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Return response string received from a server
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	const std::string& GetResponse (void) const noexcept;

//============================================================================//
//			Status
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Return true if a server is ready to handle a new request
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	bool IsReady (void) const;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Return true if we are connected to a server
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	bool IsConnected (void) const;

//============================================================================//
//			Info
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Return error code value for client's socket
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	int ErrorCode (void) const;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Return IP address of a server we connected to
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	std::string Address (void) const;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Return time amount spent to connecting action
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	double ConnectingTime (void) const noexcept;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Return time amount spent to sending request action
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	double SendingTime (void) const noexcept;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Return time amount spent to receiving request action
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	double RecevingTime (void) const noexcept;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Return size of request message sent to a server (in bytes)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	size_t RequestSize (void) const noexcept;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Return size of response message received from a server (in bytes)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	size_t ResponseSize (void) const noexcept;
};

//****************************************************************************//
/// @brief		TODO: Write description
//****************************************************************************//
double TimeDiff (timeval start, timeval end);
/*
################################################################################
#                                 END OF FILE                                  #
################################################################################
*/
