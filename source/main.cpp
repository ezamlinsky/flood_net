# include	<string.h>
# include	<iostream>
# include	<sstream>

# include	"ServerList.hpp"
# include	"Server.hpp"
# include	"Client.hpp"
# include	"Report.hpp"

using namespace std;

//==============================================================================
void CreateRequestsQueue (queue <string> &requests, size_t count) {

	// Create 'REQUESTS' amount of new requests
	for (size_t i = 0; i < count; i++) {

		// Request body
		stringstream body;
		body << "{\"json\": true, \"code\": \"azarustokens\", \"scope\": \"azarustokens\", \"table\": \"users\", \"lower_bound\": \"" << i << "\", \"upper_bound\": \"" << i << "\"}\n";

		// Length of request body
		const string str = body.str();
		size_t length = str.size();

		// JSON request to EOS.IO block-chain
		stringstream request;
		request << "POST /v1/chain/get_table_rows HTTP/1.0\r\n";
		request << "Host: eos.azarus.io:8888\r\n";
		request << "Accept: */*\r\n";
		request << "Connection: close\r\n";
		request << "content-length: " << length << "\r\n\r\n";
		request << str;

		// Push new request into queue
		requests.push (request.str());
	}
}
//==============================================================================
int main (void) {

	//--------------------------
	size_t requests_count = 1000;
	size_t concurrency = 100;
	bool debug = false;
	//--------------------------

	size_t rate;

	// Create requests queue
	queue <string> requests_queue;
	CreateRequestsQueue (requests_queue, requests_count);

	// Create client object
	Client client (IP4, TCP, debug);

	// Start testing
	try {
		rate = client.Flood ("eos.azarus.io", "8888", requests_queue, concurrency);
	}
	catch (const char *str) {
		cerr << str << endl;
	}
	catch (int code) {
		char buffer [1024];
		cerr << strerror_r (code, buffer, 1024) << endl;
	}

	// Print requests rate
	cout << "RATE: " << rate << " rps/s" << endl;

	// Print statistics information
	Report report;
	report.AddStats (client.GetStatistics());
	report.SaveStats ("stat.csv");
	report.PrintReport ();

	// Return normal exit state
	return 0;
}
