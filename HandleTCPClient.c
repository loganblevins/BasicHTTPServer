#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for recv() and send() */
#include <fcntl.h>
#include <unistd.h>     /* for close() */
#include <string.h>
#include "Utilities.h"

#define RCVBUFSIZE 2048   /* Size of receive buffer, maximum size for GET request */

void DieWithError(char *errorMessage);  /* Error handling function */

void HandleTCPClient(int clntSocket )
{
    char buffer[RCVBUFSIZE + 1];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */
    int i; /* Various counting variables that need to have retained values. */
    int j;
    int fd;

    /* Receive message from client */
    if ((recvMsgSize = recv(clntSocket, buffer, RCVBUFSIZE, 0)) < 0)
    { 
        DieWithError("recv() failed");
    }

    // Make sure the size of the data received is within our valid range. 
    // Set the null terminator so we know where the end is.
    //
    if ( recvMsgSize > 0 && recvMsgSize < RCVBUFSIZE )
    {
        buffer[recvMsgSize] = 0;
    }
    else
    {
        buffer[0] = 0;
    }

    // Check for carriage returns and newlines in the buffer and adjust buffer.
    //
    for ( i = 0; i < recvMsgSize; i++ )
    {
        if ( buffer[i] == '\r' || buffer[i] == '\n' )
        {
            buffer[i] = '*';
        }
    }
    
    // Check that we have a GET request. If not, throw 400. Otherwise, attempt to process the request.
    //
    if ( strncmp( buffer, "GET ", 4 ) )
    {
        sprintf( buffer, "HTTP/1.0 400 Bad Request\r\nConnection: close\r\n\r\n" );
        send( clntSocket, buffer, strlen( buffer ), 0 );
    }
    else
    {
        // Look for whitespce in the buffer and adjust buffer.
        // 
        for ( i = 4; i < RCVBUFSIZE; i++ )
        {
            if ( buffer[i] == ' ' )
            {
                buffer[i] = 0;
                break;
            }   
        }

        // Check if we have a request for a '/' file. If so, load index.html by default.
        //
        if ( !strncmp( &buffer[0], "GET /\0", 6 ) )
        {
            strcpy( buffer, "GET /index.html" );
        }

        int bufferLength = strlen( buffer );
        char *fileName = 0;
        int extensionLength;

        // Compare the file extension in the request to one that we support.
        //
        for ( i = 0; extensions[i].extension != 0; i++ )
        {
            extensionLength = strlen( extensions[i].extension );
            if ( !strncmp( &buffer[bufferLength - extensionLength], extensions[i].extension, extensionLength ) )
            {
                fileName = extensions[i].filetype;
                break;
            }
        }

        // Check if we have a valid file extension. Treat it as if we don't even have it.
        // 
        if ( fileName == 0 )
        {
            sprintf( buffer, "HTTP/1.0 404 Not Found\r\nConnection: close\r\n\r\n" );
            send( clntSocket, buffer, strlen( buffer ), 0 );
        } /* Check if we can open the file. */
        else if ( ( fd = open( &buffer[5], O_RDONLY ) ) != -1 )
        {
            sprintf( buffer, "HTTP/1.0 200 OK\r\nContent-Type: %s\r\nConnection: close\r\n\r\n", fileName );
            send( clntSocket, buffer, strlen( buffer ), 0 );
        }   /* We must have raised an error while opening the file. */
        else
        {
            sprintf( buffer, "HTTP/1.0 404 Not Found\r\nConnection: close\r\n\r\n" );
            send( clntSocket, buffer, strlen( buffer ), 0 );
        }
    }

    // Continue to receive requests from the client.
    //
    while ( ( recvMsgSize = read( fd, buffer, RCVBUFSIZE ) ) > 0 )
    {
        send( clntSocket, buffer, recvMsgSize, 0 );
    }
        
    close(clntSocket);    /* Close client socket */
}

