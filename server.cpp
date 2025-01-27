#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

using namespace std;
#pragma comment(lib, "Ws2_32.lib")

const int PORT = 8080;
const int MAX_CLIENTS = 100;

struct Client {
    SOCKET client_socket;
    string username;
};

vector<Client> clients;
mutex client_mutex;
condition_variable client_cv;

void handle_client(SOCKET client_socket) {
    char buffer[1024] = { 0 };
    string username;

    // Receive username from client
    int bytesReceived = recv(client_socket, buffer, 1024, 0);
    if (bytesReceived <= 0) {
        // Handle error or disconnection
        return;
    }
    username = buffer;

    {
        lock_guard<mutex> lock(client_mutex);
        Client new_client = { client_socket, username };
        clients.push_back(new_client);
    }

    client_cv.notify_all();

    // Announce new user to other clients
    string joinMessage = username + " has joined the chat.\n";
    for (auto& client : clients) {
        if (client.client_socket != client_socket) {
            send(client.client_socket, joinMessage.c_str(), joinMessage.size(), 0);
        }
    }

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        bytesReceived = recv(client_socket, buffer, 1024, 0);
        if (bytesReceived <= 0) {
            break; // Handle disconnection
        }

        // Prepend username to the message
        string message = username + ": " + buffer + "\n";

        {
            lock_guard<mutex> lock(client_mutex);
            for (auto& client : clients) {
                if (client.client_socket != client_socket) {
                    send(client.client_socket, message.c_str(), message.size(), 0);
                }
            }
        }
    }

    {
        lock_guard<mutex> lock(client_mutex);
        for (auto it = clients.begin(); it != clients.end(); ++it) {
            if (it->client_socket == client_socket) {
                clients.erase(it);
                break;
            }
        }
    }

    // Announce user leaving to other clients
    string leaveMessage = username + " has left the chat.\n";
    for (auto& client : clients) {
        send(client.client_socket, leaveMessage.c_str(), leaveMessage.size(), 0);
    }

    closesocket(client_socket);
}

int main() {

    cout << "Starting server...." << endl;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed." << endl;
        return 1;
    }

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        cerr << "socket() failed." << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in server_address;
    ZeroMemory(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        cerr << "bind() failed." << endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "listen() failed." << endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    vector<thread> client_threads;

    while (true) {
        SOCKET client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket == INVALID_SOCKET) {
            cerr << "accept() failed." << endl;
            break;
        }

        cout << "New client connected" << endl;

        client_threads.emplace_back(handle_client, client_socket);

        if (client_threads.size() >= MAX_CLIENTS) {
            cout << "Maximum client limit reached." << endl;
        }
    }

    for (auto& thread : client_threads) {
        thread.join();
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}