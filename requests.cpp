#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <nlohmann/json.hpp>

#include "helpers.hpp"
#include "requests.hpp"

using namespace std;
using namespace nlohmann;

// Function to build a GET request
string compute_get_request(string host, string url, string query_params,
                           string cookies[], int cookies_count, string token){ 
    string message;
    string line;

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (!query_params.empty()) {
        line = "GET " + url + "?" + query_params + " HTTP/1.1";
    } else {
        line = "GET " + url + " HTTP/1.1";
    }

    compute_message(message, line);

    // Step 2: add the host header
    line = "Host: " + host;
    compute_message(message, line);

    // Add the token
    if (token.size() > 0)
    {
        line = "Authorization: Bearer " + token;
        compute_message(message, line);
    }

    // Step 3: add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {

        line = "Cookie: ";

        for (int i = 0; i < cookies_count; i++) {
            if (i > 0)
                line += ";";
            line += cookies[i];
        }
        compute_message(message, line);
    }

    // Step 4: add the final new line
    compute_message(message, "");

    return message;
}

// Function to build a POST request
string compute_post_request(string host, string url, string content_type, string body_data,
                            string cookies[], int cookies_count, string token) {
    string message;
    string line;

    // Step 1: write the method name, URL, and protocol type
    line = "POST " + url + " HTTP/1.1";
    compute_message(message, line);

    // Step 2: add the host header
    line = "Host: " + host;
    compute_message(message, line);

    if (token.size() > 0)
    {
        line = "Authorization: Bearer " + token;
        compute_message(message, line);
    }

    // Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
    line = "Content-Type: " + content_type;
    compute_message(message, line);

    line = "Content-Length: " + to_string(body_data.size());
    compute_message(message, line);

    // Step 4: add cookies if any
    if (cookies != NULL)
    {
        line = "Cookie: ";
        for (int i = 0; i < cookies_count; i++)
        {
            if (i > 0)
                line += ";";
            line += cookies[i];
        }
        compute_message(message, line);
    }

    // Step 5: add new line at end of header
    compute_message(message, "");

    // Step 6: add the actual payload data
    message += body_data;

    return message;
}

string compute_delete_request(string host, string url, string query_params,
                              string cookies[], int cookies_count, string token)
{
    string message;
    string line;

     // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params.size() > 0)
    {
        line = "DELETE " + url + "?" + query_params + " HTTP/1.1";
    }
    else
    {
        line = "DELETE " + url + " HTTP/1.1";
    }

    compute_message(message, line);

    // Step 2: add the host
    line = "Host:" + host;
    compute_message(message, line);

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (token.size() > 0)
    {
        line = "Authorization: Bearer " + token;
        compute_message(message, line);
    }

    if (cookies != NULL)
    {
        line = "Cookie: ";
        for (int i = 0; i < cookies_count; i++)
        {
            if (i > 0)
                line += ";";
            line += cookies[i];
        }
        compute_message(message, line);
    }

    // Step 4: add final new line
    compute_message(message, "");
    
    return message;
}
