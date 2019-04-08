/*                                                                    Client.cpp
################################################################################
# Encoding: UTF-8                                                  Tab size: 4 #
#                                                                              #
#                HIGHLY CONCURRENT CLIENT FOR BENCHMARK TESTING                #
#                                                                              #
# Ordnung muss sein!                             Copyright (C) 2019, azarus.io #
################################################################################
*/
# include	<sys/epoll.h>
# include	<unistd.h>
# include	<thread>
# include	<mutex>

//-------------------
# include	<string.h>
# include	<iostream>
//-------------------

# include	<Server.hpp>
# include	<Client.hpp>

# define	MAX_EVENTS			(256)
# define	EPOLL_TIMEOUT		(1000)

using namespace std;

//============================================================================//
//			Local functions
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief		TODO: Write description
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static void worker (
	ServerList &list,
	vector <request_stat> &stats,
	queue <string> &requests,
	mutex &mtx,
	size_t concurrency,
	bool debug
) {

	// TODO: Delete me
	try {

	// If concurrency value is zero, then exit
	if (concurrency == 0)
		return;

	// Open an epoll file descriptor
	int epfd = epoll_create (1);

	// In case of any error, throw an exception
	if (epfd == -1)
		throw (errno);

	// Create a vector of servers for concurrent requests
	vector <Server> servers (concurrency);

	// Process all concurrent connections
	for (size_t i = 0; i < concurrency; i++) {

		// Connect to target service
		servers[i].Connect (list.GetServer(), epfd, debug);
	}

	// Reset counter for requests that are in progress state
	size_t in_progress = 0;

	// Create array for epoll event structures
	epoll_event events [MAX_EVENTS];

	// Process all requests until everything is done
	int continue_flag = true;
	while (continue_flag) {

		// TODO: Где-то здесь dead lock. Решить эту проблему

		// Wait for new events
		int count = epoll_wait (epfd, events, MAX_EVENTS, EPOLL_TIMEOUT);

		// In case of any error, throw an exception
		if (count == -1)
			throw (errno);

		//-------------------
		// TODO: Delete me
		if (count == 0) {
			cout << "in_progress = " << in_progress << endl;
			mtx.lock();
			cout << "q size = " << requests.size() << endl;
			mtx.unlock();
			for (size_t i = 0; i < concurrency; i++) {
				cout << "---------------" << endl;
				cout << "Connected[" << i << "] = " << servers[i].IsConnected() << endl;
				cout << "Ready[" << i << "] = " << servers[i].IsReady() << endl;
				cout << "Request_size[" << i << "] = " << servers[i].RequestSize() << endl;
				cout << "Response_size[" << i << "] = " << servers[i].ResponseSize() << endl;
			}
		}
		//-------------------

		// Process all new events
		for (int j = 0; j < count; j++) {

			// Extract pointer to Server object which event we should handle
			Server *server = reinterpret_cast <Server *> (events[j].data.ptr);

			// In case of any socket error
			if (events[j].events & (EPOLLHUP | EPOLLERR)) {
				//----------------------------
				cout << "EPOLLHUP | EPOLLERR" << endl;
				//----------------------------

				// Throw an exception with server error code
				throw (server -> ErrorCode ());
			}

			// If the socket is ready to 'send' operation
			if (events[j].events & EPOLLOUT) {
				//----------------------------
				//cout << "EPOLLOUT" << endl;
				//----------------------------

				// Check if a server is ready for new request
				if (server -> IsReady()) {

					// Lock mutex to read requests queue
					mtx.lock();

					// Check if requests queue is empty
					bool empty = requests.empty();

					// If requests queue is not empty
					if (!empty) {

						// then start new request to a server
						server -> SetRequest (requests.front());

						// and remove it from the queue
						requests.pop();

						// Increment in progress counter
						in_progress++;
					}

					// Unlock mutex when read queue operation is accomplished
					mtx.unlock();

					// If requests queue is empty, then ...
					if (empty) {

						// Close current connection
						server -> Disconnect();

						// Skip following loop code
						continue;
					}
				}

				// Send a request to a sever
				server -> Send ();
			}

			// If the socket is ready to 'recv' operation
			if (events[j].events & EPOLLIN) {
				//----------------------------
				//cout << "EPOLLIN" << endl;
				//----------------------------

				// Get response status code from a server
				int status = server -> Recv();

				// If we have all response data, then ...
				if (status != IN_PROGRESS) {

					// Decrement in progress counter
					in_progress--;

					// Close current connection
					server -> Disconnect();

					// Lock mutex to read requests queue
					mtx.lock();

					// Update statistics information
					request_stat statistics;
					statistics.conn_time = server -> ConnectingTime();
					statistics.send_time = server -> SendingTime();
					statistics.recv_time = server -> RecevingTime();
					statistics.send_rate = server -> RequestSize() / statistics.send_time;
					statistics.recv_rate = server -> ResponseSize() / statistics.recv_time;
					statistics.code = status;
					stats.push_back (statistics);

					// Check if requests queue is empty
					bool empty = requests.empty();

					// Unlock mutex when read queue operation is accomplished
					mtx.unlock();

					// If requests queue is not empty
					if (!empty) {

						// Then establish a new connection to a server
						server -> Connect (list.GetServer(), epfd, debug);
					}

					// If requests queue is empty and no requests are in process,
					if (empty && in_progress == 0) {

						// Then reset continue_flag
						continue_flag = false;
					}
				}
			}
		}
	}

	// Close epoll file descriptor
	if (close (epfd))
		throw (errno);
	}

	// TODO: Delete me
	catch (int code) {
		char buffer [1024];
		cerr << strerror_r (code, buffer, 1024) << endl;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
Client::Client (socket_domain domain, socket_type type, bool debug) {

	// Set domain and type values
	Client::domain = domain;
	Client::type = type;

	// Set debug flag
	Client::debug = debug;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
size_t Client::Flood (const char *host, const char *port, queue <string> &requests, size_t concurrency) {

	// Get number of requests to process
	size_t count = requests.size();

	// Check requests queue size
	if (count == 0)
		return 0;

	// Check concurrency value
	if (concurrency == 0)
		return 0;

	// Get server list for target service
	ServerList list (host, port, domain, type);

	// Get max amount of parallel threads
	size_t threads_count = thread::hardware_concurrency();

	// Create array of threads
	thread threads [threads_count];

	// Create mutex for multi threaded requests processing
	mutex mtx;

	// Create structures to store timestamps
	timeval start, end;

	// Collect a timestamp, when benchmark was started
	if (gettimeofday (&start, NULL))
		throw (errno);

	// Launch a group of threads
	for (size_t i = 0; i < threads_count; i++) {

		// Calculate thread concurrency
		size_t thread_concurrency = i ? (concurrency / threads_count) : (concurrency / threads_count + concurrency % threads_count);

		// Launch a new thread
		threads[i] = thread (worker, ref (list), ref (stats), ref (requests), ref (mtx), thread_concurrency, debug);
	}

	// Join the threads with the main thread
	for (size_t i = 0; i < threads_count; i++) {
		threads[i].join();
	}

	// Collect a timestamp, when benchmark was ended
	if (gettimeofday (&end, NULL))
		throw (errno);

	// Return requests rate
	return count / TimeDiff (start, end);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
const vector <request_stat>& Client::GetStatistics (void) const {

	// Return statistics vector
	return stats;
}
/*
################################################################################
#                                 END OF FILE                                  #
################################################################################
*/
