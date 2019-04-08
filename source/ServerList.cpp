/*                                                                ServerList.cpp
################################################################################
# Encoding: UTF-8                                                  Tab size: 4 #
#                                                                              #
#   COLLECT A LIST OF SERVERS, WHICH HANDLE ALL REQUESTS FOR TARGET SERVICE    #
#                                                                              #
# Ordnung muss sein!                             Copyright (C) 2019, azarus.io #
################################################################################
*/
# include	<arpa/inet.h>
# include	<iostream>
# include	<ServerList.hpp>

using namespace std;

//============================================================================//
//			Local functions
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief		TODO: Write description
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static string ExtractAddress (addrinfo server) {

	// Extract IPv4 address
	if (server.ai_family == IP4) {
		return ExtractIPv4 (reinterpret_cast <sockaddr_in *> (server.ai_addr));
	}

	// Extract IPv6 address
	else if (server.ai_family == IP6) {
		return ExtractIPv6 (reinterpret_cast <sockaddr_in6 *> (server.ai_addr));
	}

	// If address family if unknown then return warning message
	else {
		return string ("Unknown address family");
	}
}

//============================================================================//
//			Global functions
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief		TODO: Write description
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
string ExtractIPv4 (const sockaddr_in *address) {

	// Get temporary buffer for server address
	char buffer [INET_ADDRSTRLEN];

	// Convert the network address into a character string
	const char *result = inet_ntop (IP4, &(address -> sin_addr), buffer, INET_ADDRSTRLEN);

	// In case of any error, throw an exception
	if (result == NULL)
		throw (errno);

	// Return server address
	return string (result);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief		TODO: Write description
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
string ExtractIPv6 (const sockaddr_in6 *address) {

	// Get temporary buffer for server address
	char buffer [INET6_ADDRSTRLEN];

	// Convert the network address into a character string
	const char *result = inet_ntop (IP6, &(address -> sin6_addr), buffer, INET6_ADDRSTRLEN);

	// In case of any error, throw an exception
	if (result == NULL)
		throw (errno);

	// Return server address
	return string (result);
}

//============================================================================//
//			Constructor and destructor
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
ServerList::ServerList (const char *host, const char *port, socket_domain domain, socket_type type) {

	// Initialize server list by default value
	list = NULL;

	// Specify search criteria for resolve host name operation
	addrinfo hints;
	hints.ai_flags = 0;
	hints.ai_family = domain;
	hints.ai_socktype = type;
	hints.ai_protocol = 0;
	hints.ai_addrlen = 0;
	hints.ai_addr = NULL;
	hints.ai_canonname = NULL;
	hints.ai_next = NULL;

	// Resolve host name
	int result = getaddrinfo (host, port, &hints, &list);

	// In case of any error, throw an exception
	if (result) {
		if (result == EAI_SYSTEM)
			throw (errno);
		else
			throw (gai_strerror (result));
	}

	// Get the last element in a linked list
	addrinfo *last = list;
	while (last -> ai_next != NULL)
		last = last -> ai_next;

	// Convert linked list to circular list
	last -> ai_next = list;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
ServerList::~ServerList (void) {

	// Get the last element in a linked list
	addrinfo *last = list;
	while (last -> ai_next != list)
		last = last -> ai_next;

	// Convert circular list to linked list
	last -> ai_next = NULL;

	// Free the memory, that was allocated by linked list
	freeaddrinfo (list);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
addrinfo ServerList::GetServer (void) {

	// Lock mutex
	mtx.lock();

	// Extract a server address from the list
	addrinfo server = *list;

	// Do round-robin rotation to next resolved host names
	list = list -> ai_next;

	// Unlock mutex
	mtx.unlock();

	// Return a server address
	return server;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
size_t ServerList::Size (void) const {

	// Lock mutex
	mtx.lock();

	// Set initial count value
	size_t count = 1;

	// Iterate through all elements in circular list
	addrinfo *ptr = list;
	while (ptr -> ai_next != list) {
		count++;
		ptr = ptr -> ai_next;
	}

	// Unlock mutex
	mtx.unlock();

	// Return servers count
	return count;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void ServerList::Print (void) const {

	// Lock mutex
	mtx.lock();

	// Set initial count value
	size_t count = 1;

	// Print first server
	addrinfo *ptr = list;
	cout << "Server #" << count << ": " << ExtractAddress (*ptr) << endl;

	// Print all additional servers
	while (ptr -> ai_next != list) {
		count++;
		ptr = ptr -> ai_next;
		cout << "Server #" << count << ": " << ExtractAddress (*ptr) << endl;
	}

	// Unlock mutex
	mtx.unlock();
}
/*
################################################################################
#                                 END OF FILE                                  #
################################################################################
*/
