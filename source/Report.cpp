/*                                                                    Report.cpp
################################################################################
# Encoding: UTF-8                                                  Tab size: 4 #
#                                                                              #
#                       BENCHMARK REPORT FILE GENERATOR                        #
#                                                                              #
# Ordnung muss sein!                             Copyright (C) 2019, azarus.io #
################################################################################
*/
# include	<iostream>
# include	<fstream>
# include	<algorithm>
# include	<Report.hpp>

# define	MILLISECONDS_SCALE	(1000)
# define	KBYTES_SCALE		(1.0 / 1024)

using namespace std;

//============================================================================//
//			Local functions
//============================================================================//

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief		TODO: Write description
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
template <typename type>
static type Average (const vector <type> &data) {

	// Initialize sum value with zero
	type sum = 0;

	// Get vector size
	size_t count = data.size();

	// Compute average value
	for (type element: data) {
		sum += element;
	}

	// Return average value
	return sum / count;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief		TODO: Write description
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static void PrintTime (fstream &file, const char* tittle, vector <double> &data) {

	// Get vector size
	size_t count = data.size();

	// Sort vector in ascending order
	sort (data.begin(), data.end());

	// Compute average value
	double average = Average (data);

	// Print statistics title
	file << tittle << ',';

	// Print decile values
	for (size_t i = 10; i <= 100; i+=10) {
		size_t position = i * count / 100;
		size_t index = position ? position - 1 : position;
		file << data [index] * MILLISECONDS_SCALE << ',';
	}

	// Print average value
	file << average * MILLISECONDS_SCALE << endl;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief		TODO: Write description
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
static void PrintRate (fstream &file, const char* tittle, vector <size_t> &data) {

	// Get vector size
	size_t count = data.size();

	// Sort vector in descending order
	sort (data.begin(), data.end());
	reverse (data.begin(), data.end());

	// Compute average value
	size_t average = Average (data);

	// Print statistics title
	file << tittle << ',';

	// Print decile values
	for (size_t i = 10; i <= 100; i+=10) {
		size_t position = i * count / 100;
		size_t index = position ? position - 1 : position;
		file << data [index] * KBYTES_SCALE << ',';
	}

	// Print average value
	file << average * KBYTES_SCALE << endl;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void Report::AddStats (const vector <request_stat> &stats) {

	// Iterate trough all elements and add another portions of statistics data
	// to the report
	for (request_stat element: stats) {
		conn_time.push_back (element.conn_time);
		send_time.push_back (element.send_time);
		recv_time.push_back (element.recv_time);
		send_rate.push_back (element.send_rate);
		recv_rate.push_back (element.recv_rate);
		code.push_back (element.code);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void Report::SaveStats (const char* file_name) {

	// Create file object
	fstream file;

	// Open the file for writing and erase all existing content
	file.open (file_name, fstream::out | fstream::trunc);

	// Print statistics header
	file << "Counter,10%,20%,30%,40%,50%,60%,70%,80%,90%,100%,average" << endl;

	// Print statistics data
	PrintTime (file, "Connecting time (ms)", conn_time);
	PrintTime (file, "Sending time (ms)", recv_time);
	PrintTime (file, "Receiving time (ms)", send_time);
	PrintRate (file, "Sending rate (kbs)", send_rate);
	PrintRate (file, "Receiving rate (kbs)", recv_rate);

	// When everything is done, close the file
	file.close();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @details	TODO: Write description
/// @return		TODO: Write description
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void Report::PrintReport (void) {

	// Sort vector in ascending order
	sort (code.begin(), code.end());

	// Print report header
	cout << "================================================================================" << endl;
	cout << "STATS REPORT:" << endl;

	// If vector is not empty
	size_t size = code.size();
	if (size) {

		// Get first unique code value and set counter to '1'
		int last_code = code[0];
		size_t count = 1;

		// Iterate trough all the data starting from the second element
		for (size_t i = 1; i < size; i++) {

			// Get value of current element
			int current_code = code[i];

			// If new unique code is found
			if (last_code != current_code) {

				// Then print counter value for previous status code
				cout << "--------------------------------------------------------------------------------" << endl;
				cout << "Code: " << last_code << "       Total: " << count << endl;

				// Set new status code value and reset counter to '1'
				last_code = current_code;
				count = 1;

				// And skip following code
				continue;
			}

			// If found code is the same as previous,
			// then increment appearance counter
			count++;
		}

		// When there is no more elements left, then print counter value for last unique code
		cout << "--------------------------------------------------------------------------------" << endl;
		cout << "Code: " << last_code << "       Total: " << count << endl;
	}

	// Print report footer
	cout << "================================================================================" << endl;
}
/*
################################################################################
#                                 END OF FILE                                  #
################################################################################
*/
