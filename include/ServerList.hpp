/*                                                                ServerList.hpp
################################################################################
# Encoding: UTF-8                                                  Tab size: 4 #
#                                                                              #
#   COLLECT A LIST OF SERVERS, WHICH HANDLE ALL REQUESTS FOR TARGET SERVICE    #
#                                                                              #
# Ordnung muss sein!                             Copyright (C) 2019, azarus.io #
################################################################################
*/
# pragma	once
# include	<sys/socket.h>
# include	<netdb.h>
# include	<mutex>
# include	<string>

//****************************************************************************//
/// @brief		TODO: Write description
/// @details	TODO: Write description
//****************************************************************************//
enum socket_domain {
	IP4 = AF_INET,		///< TODO: Write description
	IP6 = AF_INET6,		///< TODO: Write description
};

//****************************************************************************//
/// @brief		TODO: Write description
/// @details	TODO: Write description
//****************************************************************************//
enum socket_type {
	TCP = SOCK_STREAM,	///< TODO: Write description
	UDP = SOCK_DGRAM,	///< TODO: Write description
};

//****************************************************************************//
/// @brief		TODO: Write description
/// @details	TODO: Write description
//****************************************************************************//
class ServerList {

private:
	addrinfo			*list;	///< TODO: Write description
	mutable std::mutex	mtx;	///< TODO: Write description

public:

//============================================================================//
//			Constructor and destructor
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Сonstructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	ServerList (const char *host, const char *port, socket_domain domain, socket_type type);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Destructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	~ServerList (void);

//============================================================================//
//			List of servers
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Return a server address from the list, using round-robin algorithm
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	addrinfo GetServer (void);

//============================================================================//
//			Info
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Return amount of servers, which are associated with target service
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	size_t Size (void) const;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Print list of all servers, which are associated with target service
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	void Print (void) const;
};

//****************************************************************************//
/// @brief		TODO: Write description
//****************************************************************************//
std::string ExtractIPv4 (const sockaddr_in *address);

//****************************************************************************//
/// @brief		TODO: Write description
//****************************************************************************//
std::string ExtractIPv6 (const sockaddr_in6 *address);
/*
################################################################################
#                                 END OF FILE                                  #
################################################################################
*/