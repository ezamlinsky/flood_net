/*                                                                    Server.cpp
################################################################################
# Encoding: UTF-8                                                  Tab size: 4 #
#                                                                              #
#      IMPLEMENTATION OF NAKED SOCKET COMMUNICATION WITH A REMOTE SERVER       #
#                                                                              #
# Ordnung muss sein!                             Copyright (C) 2019, azarus.io #
################################################################################
*/
# include	<sys/epoll.h>
# include	<unistd.h>
# include	<climits>
# include	<iostream>
# include	<ServerList.hpp>
# include	<Server.hpp>

# define	MIN_LEN				(12)
# define	START_POS			(9)
# define	BUFFER_SIZE			(1 << 18)
# define	NO_HTTP_RESPONSE	(0)

using namespace std;

//============================================================================//
//			Local functions
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief		TODO: Write description
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static void PrintDebug (const string &str, bool is_response) {

	// Print response header
	if (is_response)
		cerr << "====== [RESPONSE] ==============================================================" << endl;

	// Print request header
	else
		cerr << "====== [REQUEST] ===============================================================" << endl;

	// Print request/response body
	cerr << str << endl;
	cerr << "================================================================================" << endl;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief		TODO: Write description
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// TODO: Сначала в строке нужно найти первый пробел и от него искать код ответа сервера
static int ResponseCode (const string &str) {

	// If response string have at least minimal length, then ...
	if (static_cast <ssize_t> (str.find ('\n')) >= MIN_LEN) {

		// Extract response code from the response string
		int code = strtoul (str.data() + START_POS, NULL, 10);

		// In case of any error, throw an exception
		if (code == LONG_MIN || code == LONG_MAX)
			throw (errno);

		// Return HTTP response code
		return code;
	}

	// Return NO_HTTP_RESPONSE. Means, that no HTTP response code is found
	return NO_HTTP_RESPONSE;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief		TODO: Write description
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static string DestinationIPv4 (int fd) {

	// Local variables that hold server details
	sockaddr_in server;
	socklen_t len = sizeof (sockaddr_in);

	// Get name of connected peer socket
	if (getpeername (fd, reinterpret_cast <sockaddr*> (&server), &len))
		throw (errno);

	// Extract port number
	string port = to_string (ntohs (server.sin_port));

	// Extract server address
	string destination = ExtractIPv4 (&server);

	// Return fully qualified peer info
	destination.push_back (':');
	destination += port;
	return destination;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief		TODO: Write description
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static string DestinationIPv6 (int fd) {

	// Local variables that hold server details
	sockaddr_in6 server;
	socklen_t len = sizeof (sockaddr_in6);

	// Get name of connected peer socket
	if (getpeername (fd, reinterpret_cast <sockaddr*> (&server), &len))
		throw (errno);

	// Extract port number
	string port = to_string (ntohs (server.sin6_port));

	// Extract server address
	string destination = ExtractIPv6 (&server);

	// Return fully qualified peer info
	destination.push_back (':');
	destination += port;
	return destination;
}

//============================================================================//
//			Global functions
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief		TODO: Write description
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
double TimeDiff (timeval start, timeval end) {

	// Compute time difference for seconds
	double seconds = static_cast <int64_t> (end.tv_sec) - static_cast <int64_t> (start.tv_sec);

	// Compute time difference for microseconds
	double microseconds = (static_cast <int64_t> (end.tv_usec) - static_cast <int64_t> (start.tv_usec));

	// Return time difference
	return seconds + microseconds * 1.0e-6;
}

//============================================================================//
//			Constructor and destructor
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Server::Server (void) {

	//#####################################################
//	cout << "Server.Constructor" << endl;										// TODO: Delete me
	//#####################################################

	// Set timestamps to default values
	conn_start = {0, 0};
	conn_end = {0, 0};
	send_start = {0, 0};
	send_end = {0, 0};
	recv_start = {0, 0};
	recv_end = {0, 0};

	// Reset request position to zero
	request_pos = 0;

	// Initialize file descriptors by default value
	epoll_fd = -1;
	socket_fd = -1;

	// Set debug flag to false
	debug_mode = false;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Server::~Server (void) {

	//#####################################################
//	cout << "Server.Destructor" << endl;										// TODO: Delete me
	//#####################################################

	// If socket file descriptor is in use, then ...
	if (socket_fd != -1) {

		// Delete socket file descriptor from epoll,
		if (epoll_fd != -1)
			epoll_ctl (epoll_fd, EPOLL_CTL_DEL, socket_fd, NULL);

		// Close the socket
		close (socket_fd);
	}
}

//============================================================================//
//			Methods
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void Server::Connect (addrinfo server, int epoll, bool debug) {

	// Set epoll events to listen for
	epoll_event event;
	event.events = EPOLLOUT;
	event.data.ptr = this;

	// Set timestamps to default values
	conn_start = {0, 0};
	conn_end = {0, 0};
	send_start = {0, 0};
	send_end = {0, 0};
	recv_start = {0, 0};
	recv_end = {0, 0};

	// Clear request and response strings
	request_str.clear();
	response_str.clear();

	// Reset request position to zero
	request_pos = 0;

	// Set debug flag
	debug_mode = debug;

	// If we still connected to some server, then close a connection
	Disconnect();

	// Save epoll file descriptor
	epoll_fd = epoll;

	// Get file descriptor for non blocking socket
	socket_fd = socket (server.ai_family, server.ai_socktype | SOCK_NONBLOCK, server.ai_protocol);

	//#####################################################
//	cout << "Server.Connect fd = " << socket_fd << endl;
	//#####################################################

	// In case of any error, throw an exception
	if (socket_fd == -1)
		throw (errno);

	// Add socket file descriptor to epoll
	if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, socket_fd, &event))
		throw (errno);

	// Collect a timestamp, when connection was initialized
	if (gettimeofday (&conn_start, NULL))
		throw (errno);

	// Connect to a target service
	if (connect (socket_fd, server.ai_addr, server.ai_addrlen) && errno != EINPROGRESS)
		throw (errno);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void Server::Disconnect (void) {

	// If socket file descriptor is in use, then ...
	if (socket_fd != -1) {

		//#####################################################
//		cout << "Server.Disonnect fd = " << socket_fd << endl;					// TODO: Delete me
		//#####################################################

		// Delete socket file descriptor from epoll
		if (epoll_fd != -1 && epoll_ctl (epoll_fd, EPOLL_CTL_DEL, socket_fd, NULL))
			throw (errno);

		// Shutdown the connection
		if (shutdown (socket_fd, SHUT_RDWR))
			throw (errno);

		// Close the socket
		if (close (socket_fd))
			throw (errno);

		// Set file descriptors to default values
		epoll_fd = -1;
		socket_fd = -1;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void Server::SetRequest (const string &request) {

	//#####################################################
//	cout << "Server.SetRequest" << endl;										// TODO: Delete me
	//#####################################################

	// Set request string
	request_str = request;

	// Reset request position to zero
	request_pos = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void Server::Send (void) {

	//#####################################################
//	cout << "Server.Send id = " << socket_fd << endl;							// TODO: Delete me
	//#####################################################

	// If request is not empty, then send it to a server
	if (!request_str.empty()) {

		// If request position is zero, then it is first time, we are processing
		// this request. Let's do some initial actions
		if (request_pos == 0) {

			// If debug mode is used, then print request text
			if (debug_mode)
				PrintDebug (request_str, false);

			// Clear response string for this request
			response_str.clear();

			// Collect a timestamp, when 'send' operation has been started
			if (gettimeofday (&send_start, NULL))
				throw (errno);

			// Send request to a server
			ssize_t bytes = send (socket_fd, request_str.data(), request_str.size(), 0);

			// In case of any error, throw an exception
			if (bytes == -1)
				throw (errno);

			// Set buffer position to amount of sent bytes
			request_pos = bytes;
		}

		// Else send next portion of request data
		else {

			// Send another portion of data to a server
			ssize_t bytes = send (socket_fd, request_str.data() + request_pos, request_str.size() - request_pos, 0);

			// In case of any error, throw an exception
			if (bytes == -1)
				throw (errno);

			// Increment buffer position to amount of sent bytes
			request_pos += bytes;
		}

		// If no more data to send to a server, then ...
		if (request_pos == request_str.size()) {

			// Change polling events to listen for server answer
			epoll_event event;
			event.events = EPOLLIN;
			event.data.ptr = this;

			// Update epoll events list
			if (epoll_ctl (epoll_fd, EPOLL_CTL_MOD, socket_fd, &event))
				throw (errno);

			// Collect a timestamp, when 'recv' operation has been started
			if (gettimeofday (&recv_start, NULL))
				throw (errno);
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
int Server::Recv (void) {

	//#####################################################
//	cout << "Server.Recv id = " << socket_fd << endl;							// TODO: Delete me
	//#####################################################

	// Get temporary buffer to store received data
	char buffer [BUFFER_SIZE];

	// If response string is empty, then it is a new response from a server
	if (response_str.empty()) {

		// Collect a timestamp, when 'send' action was accomplished
		if (gettimeofday (&send_end, NULL))
			throw (errno);
	}

	// Get response from a server
	ssize_t bytes = recv (socket_fd, buffer, BUFFER_SIZE, 0);

	// In case of any error, throw an exception
	if (bytes == -1)
		throw (errno);

	// Accumulate response data into response string
	response_str.append (buffer, bytes);

	// If EOF is reached, then ...
	if (bytes == 0) {

		// Collect a timestamp, when 'recv' action was accomplished
		if (gettimeofday (&recv_end, NULL))
			throw (errno);

		// If debug mode is used, then print response text
		if (debug_mode)
			PrintDebug (response_str, true);

		// Return HTTP response code
		return ResponseCode (response_str);
	}

	// Return IN_PROGRESS. This means, that request action is not accomplished
	return IN_PROGRESS;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
const string& Server::GetResponse (void) const noexcept {

	//#####################################################
//	cout << "Server.GetResponse" << endl;										// TODO: Delete me
	//#####################################################

	// Return response string
	return response_str;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
bool Server::IsReady (void) const {

	//#####################################################
//	cout << "Server.IsReady fd = " << socket_fd << endl;						// TODO: Delete me
	//#####################################################

	// If a server is connected and request string is empty, then we are ready
	// for a new requests
	return IsConnected() && request_str.empty();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
bool Server::IsConnected (void) const {

	//#####################################################
//	cout << "Server.IsConnected fd = " << socket_fd << endl;					// TODO: Delete me
	//#####################################################

	// Get socket error code
	int error_code = ErrorCode();

	// If socket is connected to a server, and connection accomplished timestamp
	// is not set
	if (!error_code && conn_end.tv_sec == 0 && conn_end.tv_usec == 0) {

		// then collect a timestamp, when connection action was really accomplished
		if (gettimeofday (&conn_end, NULL))
			throw (errno);
	}

	// Return true if error code is zero
	return !error_code;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
int Server::ErrorCode (void) const {

	//#####################################################
//	cout << "Server.ErrorCode" << endl;											// TODO: Delete me
	//#####################################################

	// Error number (pending socket error)
	socklen_t len = sizeof (int);
	int error;

	// Extract pending socket error
	if (getsockopt (socket_fd, SOL_SOCKET, SO_ERROR, &error, &len))
		throw (errno);

	// Return error code
	return error;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
string Server::Address (void) const {

	//#####################################################
//	cout << "Server.Address" << endl;											// TODO: Delete me
	//#####################################################

	// Domain type (version of IP protocol)
	socklen_t len = sizeof (int);
	int domain;

	// Extract version of IP protocol
	if (getsockopt (socket_fd, SOL_SOCKET, SO_DOMAIN, &domain, &len))
		throw (errno);

	// Extract IPv4 destination addressed
	if (domain == IP4)
		return DestinationIPv4 (socket_fd);

	// Extract IPv6 destination addressed
	else if (domain == IP6)
		return DestinationIPv6 (socket_fd);

	// If address family if unknown then return warning message
	else
		return string ("Unknown address family");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
double Server::ConnectingTime (void) const noexcept {

	//#####################################################
//	cout << "Server.ConnectingTime" << endl;									// TODO: Delete me
	//#####################################################

	// Return time difference between conn_end and conn_start
	return TimeDiff (conn_start, conn_end);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
double Server::SendingTime (void) const noexcept {

	//#####################################################
//	cout << "Server.SendingTime" << endl;										// TODO: Delete me
	//#####################################################

	// Return time difference between send_end and send_start
	return TimeDiff (send_start, send_end);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
double Server::RecevingTime (void) const noexcept {

	//#####################################################
//	cout << "Server.RecevingTime" << endl;										// TODO: Delete me
	//#####################################################

	// Return time difference between recv_end and recv_start
	return TimeDiff (recv_start, recv_end);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
size_t Server::RequestSize (void) const noexcept {

	//#####################################################
//	cout << "Server.RequestSize" << endl;										// TODO: Delete me
	//#####################################################

	// Return request size in bytes
	return request_str.size();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
size_t Server::ResponseSize (void) const noexcept {

	//#####################################################
//	cout << "Server.ResponseSize" << endl;										// TODO: Delete me
	//#####################################################

	// Return response size in bytes
	return response_str.size();
}
/*
################################################################################
#                                 END OF FILE                                  #
################################################################################
*/
