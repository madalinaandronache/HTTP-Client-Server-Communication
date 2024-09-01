#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <nlohmann/json.hpp>

#include "helpers.hpp"
#include "requests.hpp"

using namespace std;
using namespace nlohmann;

#define HOST "34.246.184.49"
#define PAYLOAD_TYPE "application/json"

int get_status_code(string response)
{
    size_t first_space = response.find(' ');
    size_t second_space = response.find(' ', first_space + 1);

    if (first_space != std::string::npos && second_space != std::string::npos)
    {
        string status_code_str = response.substr(first_space + 1, second_space - first_space - 1);
        return stoi(status_code_str);
    }

    return -1;
}

json get_error(string response)
{
    int error_start = response.find("{\"error");
    response = response.substr(error_start);

    json parsed_response = json::parse(response);

    return parsed_response["error"].get<string>();
}

void register_user(int sockfd)
{
    string username, password, message, response;

    // Read the username and the password
    cout << "username=";
    getline(cin, username);
    cout << "password=";
    getline(cin, password);

    // Create a new JSON object
    json new_object;
    new_object[USERNAME] = username;
    new_object[PASSWORD] = password;

    // Compute the POST request and send it to the server to the specific access route
    message = compute_post_request(HOST, REGISTER_ROUTE, PAYLOAD_TYPE, new_object.dump(), NULL, 0, "");
    send_to_server(sockfd, message);

    // We get the response from the server
    response = receive_from_server(sockfd);

    // Get the status code from the HTTP response
    int status_code = get_status_code(response);

    if (status_code == 201)
    {
        cout << status_code << " - SUCCESS: " << "\"User succesfully registered!\"\n";
    }
    else
    {
        cout << status_code << " - ERROR: " << get_error(response) << '\n';
    }

    return;
}

string get_cookie(string response)
{
    int cookie_start = response.find("Set-Cookie:");
    string cookie = response.substr(cookie_start + strlen("Set-Cookie: "));

    int cookie_end = cookie.find(";");
    cookie = cookie.substr(0, cookie_end);

    return cookie;
}

// The login cookie is returned
string login_user(int sockfd)
{
    string username, password, message, response;

    // Read the username and the password
    cout << "username=";
    getline(cin, username);
    cout << "password=";
    getline(cin, password);

    // Create a new JSON object
    json new_object;
    new_object[USERNAME] = username;
    new_object[PASSWORD] = password;

    // Compute the POST request and send it to the server to the specific access route
    message = compute_post_request(HOST, LOGIN_ROUTE, PAYLOAD_TYPE, new_object.dump(), NULL, 0, "");
    send_to_server(sockfd, message);

    // We get the response from the server
    response = receive_from_server(sockfd);

    // Get the status code from the HTTP response
    int status_code = get_status_code(response);

    if (status_code == 200)
    {
        cout << status_code << " - SUCCESS: " << "\"User succesfully logged in!\"\n";

        // Process the session cookie
        return get_cookie(response);
    }
    else
    {
        cout << status_code << " - ERROR: " << get_error(response) << '\n';
    }

    return "";
}

string get_token(string response)
{
    int token_start = response.find("{\"token");
    response = response.substr(token_start);

    json parsed_response = json::parse(response);
    string token = parsed_response["token"];

    return token;
}

// The token is returned
string enter_library(int sockfd, string cookie)
{
    string message, response;

    // First, we have the login cookie
    string cookies[1] = {cookie};

    // Compute the GET request and send it to the server to the specific access route
    message = compute_get_request(HOST, LIBRARY_ACCESS_ROUTE, "", cookies, 1, "");
    send_to_server(sockfd, message);

    // We get the response from the server
    response = receive_from_server(sockfd);

    // Get the status code from the HTTP response
    int status_code = get_status_code(response);

    if (status_code == 200)
    {
        cout << status_code << " - SUCCESS: " << "\"User has access to the library!\"\n";

        // Extract the token to have demonstrate that we have access to the library
        return get_token(response);
    }
    else
    {
        cout << status_code << " - ERROR: " << get_error(response) << '\n';
    }

    return NULL;
}

void list_books(string response)
{
    int books_start = response.find("[");
    response = response.substr(books_start);
    json parsed_response = json::parse(response);

    for (size_t i = 0; i < parsed_response.size(); i++)
    {
        cout << "id: " << parsed_response[i][ID] << '\n';
        cout << "title: " << parsed_response[i][TITLE] << '\n' << '\n';
    }

    return;
}

void get_books(int sockfd, string cookie, string token)
{
    string message, response;

    // First, we have the login cookie
    string cookies[1] = {cookie};

    // Compute the GET request and send it to the server to the specific access route
    // Give the TOKEN as parameter to demonstrate the access
    message = compute_get_request(HOST, LIBRARY_BOOKS_ROUTE, "", cookies, 1, token);
    send_to_server(sockfd, message);

    // We get the response from the server
    response = receive_from_server(sockfd);

    // Get the status code from the HTTP response
    int status_code = get_status_code(response);

    if (status_code == 200)
    {
        cout << status_code << " - SUCCESS: " << "\"The books in the library are:\"\n"
             << '\n';

        list_books(response);
    }
    else
    {
        cout << status_code << " - ERROR: " << get_error(response) << '\n';
    }

    return;
}

void details_book(string response)
{
    int books_start = response.find("{");
    response = response.substr(books_start);
    json parsed_response = json::parse(response);

    cout << "id: " << parsed_response[ID].get<int>() << '\n';
    cout << "title: " << parsed_response[TITLE].get<string>() << '\n';
    cout << "author: " << parsed_response[AUTHOR].get<string>() << '\n';
    cout << "genre: " << parsed_response[GENRE].get<string>() << '\n';
    cout << "publisher: " << parsed_response[PUBLISHER].get<string>() << '\n';
    cout << "page_count: " << parsed_response[PAGE_COUNT] << '\n'
         << '\n';
}

void get_book(int sockfd, string cookie, string token)
{
    string message, response, id_str;

    // First, we have the login cookie
    string cookies[1] = {cookie};

    cout << "id=";
    getline(cin, id_str);

    try {
        size_t pos = 0;
        stoi(id_str, &pos);

        if (pos != id_str.length()) {
            cout << "ERROR: " << "\"Please introduce a valid book ID! (no extra characters)\"\n";
            return;
        }
    } catch (const invalid_argument &) {
        cout << "ERROR: " << "\"Please introduce a valid book ID! (no extra characters)\"\n";
        return;
    }

    string route = LIBRARY_BOOKS_ROUTE;
    route = route + "/" + id_str;

    // Compute the GET request and send it to the server to the specific access route
    // Give the TOKEN as parameter to demonstrate the access
    message = compute_get_request(HOST, route, "", cookies, 1, token);
    send_to_server(sockfd, message);

    // We get the response from the server
    response = receive_from_server(sockfd);

    // Get the status code from the HTTP response
    int status_code = get_status_code(response);

    if (status_code == 200)
    {
        cout << status_code << " - SUCCESS: " << "\"The book details are:\"\n"
             << '\n';

        details_book(response);
    }
    else
    {
        cout << status_code << " - ERROR: " << get_error(response) << '\n';
    }

    return;
}

void add_book(int sockfd, string cookie, string token)
{
    int page_count;
    string message, response;
    string title, author, genre, page_count_str, publisher;

    // First, we have the login cookie
    string cookies[1] = {cookie};

    cout << "title=";
    getline(cin, title);
    cout << "author=";
    getline(cin,author);
    cout << "genre=";
    getline(cin,genre);
    cout << "publisher=";
    getline(cin,publisher);
    cout << "page_count=";
    getline(cin, page_count_str);

    try {
        size_t pos = 0;
        page_count = std::stoi(page_count_str, &pos);

        if (pos != page_count_str.length()) {
            cout << "ERROR: " << "\"Please introduce a valid number of pages! (no extra characters)\"\n";
            return;
        }
    } catch (const invalid_argument &) {
        cout << "ERROR: " << "\"Please introduce a valid number of pages! (no extra characters)\"\n";
        return;
    }

    json new_object;
    new_object[TITLE] = title;
    new_object[AUTHOR] = author;
    new_object[GENRE] = genre;
    new_object[PUBLISHER] = publisher;
    new_object[PAGE_COUNT] = page_count;

    // Compute the POST request and send it to the server to the specific route
    // Give the TOKEN as parameter to demonstrate the access
    message = compute_post_request(HOST, LIBRARY_BOOKS_ROUTE, PAYLOAD_TYPE, new_object.dump(), cookies, 1, token);
    send_to_server(sockfd, message);

    // We get the response from the server
    response = receive_from_server(sockfd);

    // Get the status code from the HTTP response
    int status_code = get_status_code(response);

    if (status_code == 200)
    {
        cout << status_code << " - SUCCESS: " << "\"The book was added succesfully!\"\n";
    }
    else
    {
        cout << status_code << " - ERROR: " << get_error(response) << '\n';
    }

    return;
}

void logout_user(int sockfd, string &cookie, string &token)
{
    string message, response;

    // First, we have the login cookie
    string cookies[1] = {cookie};

    // Compute the GET request and send it to the server to the specific access route
    message = compute_get_request(HOST, LOGOUT_ROUTE, "", cookies, 1, "");
    send_to_server(sockfd, message);

    // We get the response from the server
    response = receive_from_server(sockfd);

    // Get the status code from the HTTP response
    int status_code = get_status_code(response);

    if (status_code == 200)
    {
        cout << status_code << " - SUCCESS: " << "\"User logged out!\"\n";

        // Get the login cookie and the token ready for another session
        cookie = "";
        token = "";
    }
    else
    {
        cout << status_code << " - ERROR: " << get_error(response) << '\n';
    }

    return;
}

void delete_book(int sockfd, string cookie, string token) 
{
    string message, response, id_str;

    // First, we have the login cookie
    string cookies[1] = {cookie};

    cout << "id=";
    getline(cin, id_str);

    try {
        size_t pos = 0;
        stoi(id_str, &pos);

        if (pos != id_str.length()) {
            cout << "ERROR: " << "\"Please introduce a valid book ID! (no extra characters)\"\n";
            return;
        }
    } catch (const invalid_argument &) {
        cout << "ERROR: " << "\"Please introduce a valid book ID! (no extra characters)\"\n";
        return;
    }

    string route = LIBRARY_BOOKS_ROUTE;
    route = route + "/" + id_str;

    // Compute the DELETE request and send it to the server to the specific access route
    // Give the TOKEN as parameter to demonstrate the access
    message = compute_delete_request(HOST, route, "", cookies, 1, token);
    send_to_server(sockfd, message);

    // We get the response from the server
    response = receive_from_server(sockfd);

    // Get the status code from the HTTP response
    int status_code = get_status_code(response);

    if (status_code == 200)
    {
        cout << status_code << " - SUCCESS: " << "\"Book deleted successfully!\"\n";
    }
    else
    {
        cout << status_code << " - ERROR: " << get_error(response) << '\n';
    }

    return;
}

int main()
{
    string input, cookie, token;
    int sockfd, loggedIn = 0;

    while (true)
    {

        getline(cin, input);

        if (input == "exit")
        {
            return 0;
        }
        else
        {
            sockfd = open_connection(HOST, 8080, AF_INET, SOCK_STREAM, 0);
            if (input == "register")
            {
                if(loggedIn == 1) {
                    // The user is already logged in
                    cout << "ERROR: " << "\"You are already logged in!\"\n";
                } else {
                    register_user(sockfd);
                }
            }
            else if (input == "login")
            {
                if(loggedIn == 1) {
                    // The user is already logged in
                    cout << "ERROR: " << "\"You are already logged in!\"\n";
                } else {
                    cookie = login_user(sockfd);
                    if(cookie != "") {
                        loggedIn = 1;
                    }
                }
            }
            else if (input == "enter_library")
            {
                if(loggedIn == 0) {
                    // The user is not logged in
                    cout << "ERROR: " << "\"You are not logged in!\"\n";
                } else {
                    token = enter_library(sockfd, cookie);
                }
            }
            else if (input == "get_books")
            {
                if(loggedIn == 0) {
                    // The user is not logged in
                    cout << "ERROR: " << "\"You are not logged in!\"\n";
                } else {
                    get_books(sockfd, cookie, token);
                }
            }
            else if (input == "get_book")
            {
                if(loggedIn == 0) {
                    // The user is not logged in
                    cout << "ERROR: " << "\"You are not logged in!\"\n";
                } else {
                    get_book(sockfd, cookie, token);
                }
            }
            else if (input == "add_book")
            {
                if(loggedIn == 0) {
                    // The user is not logged in
                    cout << "ERROR: " << "\"You are not logged in!\"\n";
                } else {
                    add_book(sockfd, cookie, token);
                }
            }
            else if (input == "delete_book")
            {  
                if(loggedIn == 0) {
                    // The user is not logged in
                    cout << "ERROR: " << "\"You are not logged in!\"\n";
                } else {
                    delete_book(sockfd, cookie, token);
                }
            }
            else if (input == "logout")
            {   
                if(loggedIn == 0) {
                    // The user is not logged in
                    cout << "ERROR: " << "\"You are not logged in!\"\n";
                } else { 
                    logout_user(sockfd, cookie, token);
                    loggedIn = 0;
                }
            }
            else
            {
                cout << "ERROR: Not a valid command. Try again!" << '\n';
            }
        }
    }

    close(sockfd);
    return 0;
}
