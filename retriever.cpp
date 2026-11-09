#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_address> <filename>" << std::endl;
        return 1;
    }

    std::string server_address = argv[1];
    std::string filename = argv[2];

    std::string request = "GET /" + filename + " HTTP/1.1\r\n"
                          "Host: " + server_address + "\r\n"
                          "Connection: close\r\n"
                          "\r\n";

    struct addrinfo hints{}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(server_address.c_str(), "80", &hints, &res) != 0) {
        std::cerr << "Error: could not resolve server address" << std::endl;
        return 1;
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) {
        std::cerr << "Error: could not create socket" << std::endl;
        freeaddrinfo(res);
        return 1;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
        std::cerr << "Error: could not connect to server" << std::endl;
        close(sockfd);
        freeaddrinfo(res);
        return 1;
    }

    freeaddrinfo(res);

    send(sockfd, request.c_str(), request.size(), 0);

    char buffer[4096];
    ssize_t bytes;
    while ((bytes = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        std::cout << buffer;
    }

    close(sockfd);
    return 0;
}
