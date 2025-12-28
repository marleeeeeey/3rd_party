# ASIO example with TCP sockets in ASYNC mode

This is a client-server application used an ASIO library with TCP sockets in ASYNC mode.

Clients connect to the server and send messages to all other clients via the server.

- Server is a single-threaded application running ASIO IO context and managing client sessions in the rooms.
- Client is a double-threaded application:
    - The first thread is used for reading messages from stdin and
    - the second thread is used for running ASIO IO context.