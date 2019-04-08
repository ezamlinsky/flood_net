/*                                                                    Client.hpp
################################################################################
# Encoding: UTF-8                                                  Tab size: 4 #
#                                                                              #
#                HIGHLY CONCURRENT CLIENT FOR BENCHMARK TESTING                #
#                                                                              #
# Ordnung muss sein!                             Copyright (C) 2019, azarus.io #
################################################################################
*/
# pragma	once
# include	<vector>
# include	<queue>
# include	<string>
# include	<ServerList.hpp>

//****************************************************************************//
/// @brief		TODO: Write description
/// @details	TODO: Write description
//****************************************************************************//
struct request_stat {
	double	conn_time;		///< TODO: Write description
	double	send_time;		///< TODO: Write description
	double	recv_time;		///< TODO: Write description
	size_t	send_rate;		///< TODO: Write description
	size_t	recv_rate;		///< TODO: Write description
	int		code;			///< TODO: Write description
};

//****************************************************************************//
/// @brief		TODO: Write description
/// @details	TODO: Write description
//****************************************************************************//
class Client {

private:
	socket_domain				domain;		///< TODO: Write description
	socket_type					type;		///< TODO: Write description
	std::vector <request_stat>	stats;		///< TODO: Write description
	bool						debug;		///< TODO: Write description

public:

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	Client (socket_domain domain, socket_type type, bool debug);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Do benchmark tests and collect statistics data
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	size_t Flood (const char *host, const char *port, std::queue <std::string> &requests, size_t concurrency);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
/// @brief	Return collected statistics data for benchmark test
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
	const std::vector <request_stat>& GetStatistics (void) const;
};
/*
################################################################################
#                                 END OF FILE                                  #
################################################################################
*/
