#ifndef _SERVER_HPP_
#define _SERVER_HPP_

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#include "exchange.hpp"

// Simple TELNET server
class Server {
public:
  Server(Exchange& exchange) : m_exchange(exchange) {}

  void run( int port ) {
    bind_and_listen( port );
    await_connections();
  }

  void await_connections() {
    int clients[30];
    int max_clients = 30;
    memset(clients, 0, sizeof(clients));

    std::cout << "Waiting for incoming connection" << std::endl;

    fd_set read_fds;
    int activity;
    int max_fd;

    // Connect / Disconnect (select) loop
    while( true) {
      FD_ZERO(&read_fds);
      FD_SET(m_server, &read_fds);

      // Looking for the max fd because select needs it
      max_fd = m_server;
      for(int i = 0; i < max_clients; i++) {
        if( clients[i] > max_fd ) {
          max_fd = clients[i];
        }

        if( clients[i] != 0 ) {
          FD_SET( clients[i], &read_fds );
        }
      }

      std::cout << "Polling..." << max_fd + 1 << std::endl;

      // Select on sockets
      activity = select( max_fd + 1, &read_fds, NULL, NULL, NULL );
      if( (activity < 0) && (errno != EINTR) )  {
        printf("select error");
      }

      // Check for new connections
      if( FD_ISSET(m_server, &read_fds) ) {
        struct sockaddr_in client_address;
        socklen_t client_length = sizeof(client_address);
        int client;

        std::cout << "New connection detected...";

        if( (client = accept(m_server, (struct sockaddr *)&client_address, &client_length) ) > 0 ) {
          std::cout << "Accepted!" << std::endl;

          // Make connection non-blocking
          fcntl(client, F_SETFL, O_NONBLOCK);

          // Drop connections if there are too many, otherwise add it to the set
          for(int i = 0; i < max_clients; i++) {
            if( clients[i] == 0 ) {
              clients[i] = client;
              m_socket_buffer[ client ] = "";
              break;
            }
          }

          // Send welcome message
          send_string( client, welcome_message() + "\n" );
        } else {
          perror("accept");
          exit(-1);
        }
      }

      // Spin through connected sockets and check for data
      std::cout << "Polling the client sockets" << std::endl;
      for(int i = 0; i < max_clients; i++) {
        int client = clients[i];

        // Check socket if the descriptor is present
        if( (client != 0) && (FD_ISSET(client, &read_fds)) ) {
          std::cout << "Handling socket " << client << "." << std::endl;

          try {
            handle( client );
          } catch( const char * reason ) {
            std::cout << "Client disconnected." << std::endl;

            // Close connect to client
            close( client );
            clients[i] = 0;
            m_socket_buffer.erase( client );
          }
        }
      }

      // Loop... back to top.
    }
  }

  void handle( int client ) {
    std::cout << "Parsing Request...";
    auto request = read_line( client );
    auto response = process_request(request);
    send_string( client, response + "\n" );
    std::cout << "Replied!" << std::endl;
  }

  std::string process_request( std::string request ) {
    if( request.find("buy") != std::string::npos ) {
      auto order = Order<Buy>( parse_price( request ), parse_size( request ) );
      m_exchange.insert( order );
      return order.to_string();
    }
    else if( request.find("clear") != std::string::npos ) {
      m_exchange.clear();
      return "ok";
    }
    else if( request.find("sell") != std::string::npos ) {
      auto order = Order<Sell>( parse_price( request ), parse_size( request ) );
      m_exchange.insert( order );
      return order.to_string();
    }
    else if( request.find("print") != std::string::npos ) {
      return m_exchange.to_string();
    }

    return "error: no such command";
  }

  float parse_price( std::string request ) {
    auto tokens = split( request );

    if( tokens.size() >= 3 ) {
      return std::atof( tokens[1].c_str() );
    }

    std::cout << "Warning: Order was submitted without a price." << std::endl;
    return 100.00;
  }

  float parse_size( std::string request ) {
    auto tokens = split( request );

    if( tokens.size() >= 3 ) {
      return std::atof( tokens[2].c_str() );
    }

    std::cout << "Warning: Order was submitted without a size." << std::endl;
    return 30;
  }

  std::vector< std::string > split( std::string some_string ) {
    char separator = ' ';
    std::vector< std::string > output;
    std::string temp = "";
    for( char& some_char : some_string ) {
      if( (some_char != separator) && (some_char != '\n') && (some_char != '\r') ) {
        temp += some_char;
      } else {
        output.push_back( temp );
        temp = "";
      }
    }

    if( !temp.empty() ) {
      output.push_back( temp );
    }

    return output;
  }

  std::string welcome_message() {
    return "Commands: 'buy', 'clear', 'sell', and 'print'";
  }

  void bind_and_listen( int port ) {
    struct sockaddr_in server_address;

    // setup socket address structure
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // create socket
    m_server  = socket(PF_INET, SOCK_STREAM, 0);
    if (!m_server ) {
      perror("socket");
      exit(-1);
    }

    // set socket to immediately reuse port when the application closes
    int reuse = 1;
    if (setsockopt(m_server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
      perror("setsockopt");
      exit(-1);
    }

    // call bind to associate the socket with our local address and
    // port
    if (bind(m_server, (const struct sockaddr *)&server_address, sizeof(server_address) ) < 0) {
      perror("bind");
      exit(-1);
    }

    // convert the socket to listen for incoming connections
    if (listen(m_server, SOMAXCONN) < 0) {
      perror("listen");
      exit(-1);
    }
  }

  std::string read_once( int client ) {
    char buffer[1025];
    ssize_t bytes_read = recv(client , buffer, 1024, 0);

    if (bytes_read < 0) {
      if (errno == EINTR) {
        // the socket call was interrupted -- try again
        return "";
      } else {
        // an error occurred, so break out
        throw "disconnected";
      }

    } else if (bytes_read == 0) {
      // the socket is closed
      throw "disconnected";
    }

    return std::string(buffer, bytes_read);
  }

  std::string read_until(int client, char terminator) {

    // Copy the remaining buffer before we read
    std::string response = m_socket_buffer[client];

    // Read until we see the terminating char
    while( response.find(terminator) == std::string::npos ) {
      response.append( read_once( client ) );
    }

    // Store any buffer beyond terminator
    size_t termination_index = response.find(terminator);

    m_socket_buffer[client] = response.substr(termination_index + 1);

    // Return up until the terminator
    return response.substr(0, (termination_index + 1) );
  }

  std::string read_line( int client ) {
    return read_until(client, '\n');
  }

  bool send_string(int client, std::string message) {
    // A nice pretty command line thing
    message += "> ";

    // prepare to send message
    const char * message_cstr = message.c_str();
    size_t bytes_left = message.length();
    ssize_t bytes_written;
    // loop to be sure it is all sent
    while (bytes_left) {
      if ( (bytes_written = send(client, message_cstr, bytes_left, 0) ) < 0) {
        if (errno == EINTR) {
          // the socket call was interrupted -- try again
          continue;
        } else if( errno == EWOULDBLOCK ) {
          return false;
        } else {
          // an error occurred, so break out
          perror("write");
          return false;
        }
      } else if (bytes_written == 0) {
        // the socket is closed
        return false;
      }
      bytes_left -= bytes_written;
      message_cstr += bytes_written;
    }
    return true;
  }

private:
  std::unordered_map< int, std::string > m_socket_buffer;
  Exchange& m_exchange;
  int m_server;
};

#endif // _SERVER_HPP_
