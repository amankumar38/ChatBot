# ChatBot

Client-Server Chat Application

This project implements a simple chat application with a server and multiple clients.

Features:

Multi-client support: Allows multiple clients to connect to the server and chat with each other.
Usernames: Clients can specify usernames upon connecting.
Message broadcasting: Messages sent by one client are broadcast to all other connected clients.
Join/Leave notifications: Displays messages when clients join or leave the chat.
Thread safety: Utilizes threads to handle client connections concurrently.
How to Use:

Compile:

Open the project in VS Code.
Ensure you have the necessary include directories and libraries set up in your VS Code project settings.
Compile the code using the appropriate compiler (e.g., g++ or MSVC).
Run:

Server: Run the server executable in a terminal.
Clients: Open multiple terminal windows or instances of the client executable to connect to the server.
Usage:

Server: The server will listen for incoming client connections on the specified port (8080 by default).
Clients:
When a client connects, it will be prompted to enter a username.
Type messages in the terminal and press Enter to send them to all other connected clients.
To exit, close the client's terminal window.
VS Code Setup:

Create a new project or open an existing one.
Install the C/C++ extension for VS Code.
Create a tasks.json file:
In the VS Code command palette, type "Tasks: Configure Task" and select "Create tasks.json file from template".
Choose "Others" as the template.
Replace the existing command with the appropriate compiler command for your system (e.g., g++ -o server server.cpp).
Create a launch.json file:
In the VS Code command palette, type "Debug: Open configurations" and select "create a launch.json file".
Choose "C/C++ Launch" as the configuration.
Modify the "program" and "args" properties to match the paths of your server and client executables.
Building and Running in VS Code:

Build:
Press Run icon to build the project.
Run:
Server: Run the server by pressing F5 or clicking the "Start Debugging" button.
Clients: Open multiple terminal windows to run the client executable.
