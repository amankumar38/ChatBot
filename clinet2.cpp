#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <limits>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "WSAStartup failed." << endl;
        return 1;
    }

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET)
    {
        cerr << "socket() failed." << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in server_address;
    ZeroMemory(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Replace with server IP
    server_address.sin_port = htons(PORT);

    if (connect(client_socket, (sockaddr *)&server_address, sizeof(server_address)) == SOCKET_ERROR)
    {
        cerr << "connect() failed." << endl;
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    string username;
    cout << "Enter your username: ";
    cin >> username;

    send(client_socket, username.c_str(), username.size(), 0);

    char buffer[BUFFER_SIZE] = {0};
    string received_message;

    while (true)
    {
        // Receive messages first
        int bytesReceived = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0)
        {
            if (bytesReceived == 0)
            {
                cerr << "Server disconnected." << endl;
            }
            else
            {
                cerr << "recv() failed: " << WSAGetLastError() << endl;
            }
            break;
        }

        received_message.append(buffer, bytesReceived);

        // Check if the received data contains a complete message (look for delimiter)
        size_t delimiterPos = received_message.find("|");
        while (delimiterPos != string::npos)
        {
            string recipient = received_message.substr(0, delimiterPos);
            string message = received_message.substr(delimiterPos + 1);

            cout << "Message from " << recipient << ": " << message << endl;

            // Remove the processed message from the received_message string
            received_message = received_message.substr(delimiterPos + 1 + message.length());
            delimiterPos = received_message.find("|");
        }

        // Clear buffer for next receive
        memset(buffer, 0, BUFFER_SIZE);

        // Send a message
        string recipient;
        cout << "Enter recipient username: ";
        cin >> recipient;

        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

        string message;
        cout << "Enter message: ";
        getline(cin, message);

        // Send recipient and message
        string combined_data = recipient + "|" + message;
        send(client_socket, combined_data.c_str(), combined_data.length(), 0);

        if (message == "exit")
        {
            break;
        }
    }

    closesocket(client_socket);
    WSACleanup();

    return 0;
}