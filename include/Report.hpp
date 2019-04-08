/*                                                                    Report.hpp
################################################################################
# Encoding: UTF-8                                                  Tab size: 4 #
#                                                                              #
#                       BENCHMARK REPORT FILE GENERATOR                        #
#                                                                              #
# Ordnung muss sein!                             Copyright (C) 2019, azarus.io #
################################################################################
*/
# pragma	once
# include	<vector>
# include	<Client.hpp>

//****************************************************************************//
/// @brief		TODO: Write description
/// @details	TODO: Write description
//****************************************************************************//
class Report {

private:
	std::vector <double>	conn_time;		///< TODO: Write description
	std::vector <double>	send_time;		///< TODO: Write description
	std::vector <double>	recv_time;		///< TODO: Write description
	std::vector <size_t>	send_rate;		///< TODO: Write description
	std::vector <size_t>	recv_rate;		///< TODO: Write description
	std::vector <int>		code;			///< TODO: Write description

public:

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Add new portion of statistics information to the report
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	void AddStats (const std::vector <request_stat> &stats);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Save final benchmark report to a file
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	void SaveStats (const char* file_name);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Print summary report with HTTP status codes received from a web server
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	void PrintReport (void);
};
/*
################################################################################
#                                 END OF FILE                                  #
################################################################################
*/
