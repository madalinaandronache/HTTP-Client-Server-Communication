#ifndef _REQUESTS_
#define _REQUESTS_

#include <string>

using namespace std;

// computes and returns a GET request string 
std::string compute_get_request(string host, string url, string query_params,
							string cookies[], int cookies_count, string token);

// computes and returns a POST request string
std::string compute_post_request(string host, string url, string content_type, string body_data,
                                string cookies[], int cookies_count, string token);

// computes and returns a DELETE request string
std::string compute_delete_request(string host, string url, string query_params,
							string cookies[], int cookies_count, string token);

#endif
