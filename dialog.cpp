/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:

**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include "dialog.h"



//my additions
#include <time.h>
//#include <curses.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_RESPONSE (10 * 1024)

int open_client_socket(char * host, int port) {
    // Initialize socket address structure
    struct  sockaddr_in socketAddress;

    // Clear sockaddr structure
    memset((char *)&socketAddress,0,sizeof(socketAddress));

    // Set family to Internet
    socketAddress.sin_family = AF_INET;

    // Set port
    socketAddress.sin_port = htons((u_short)port);

    // Get host table entry for this host
    struct  hostent  *ptrh = gethostbyname(host);
    if ( ptrh == NULL ) {
        perror("gethostbyname");
        exit(1);
    }

    // Copy the host ip address to socket address structure
    memcpy(&socketAddress.sin_addr, ptrh->h_addr, ptrh->h_length);

    // Get TCP transport protocol entry
    struct  protoent *ptrp = getprotobyname("tcp");
    if ( ptrp == NULL ) {
        perror("getprotobyname");
        exit(1);
    }

    // Create a tcp socket
    int sock;
    sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    // Connect the socket to the specified server
    if (connect(sock, (struct sockaddr *)&socketAddress,
            sizeof(socketAddress)) < 0) {
        perror("connect");
        exit(1);
    }

//	printf("AAH");
    return sock;
};


class IRCClient{
private:
    int argc;
    char * argv[];
public:
    char * user;
    char * password;
    char * host;
    char * sport;
    int port;
    int socket;

    IRCClient(int argc, char * argv[]){
        //constructor
        //parsing all of argv and doing stuff woo
        if(argc < 5) printUsage();
        this->host = argv[1];
        this->port = atoi(argv[2]);
        this->user = argv[3];
        this->password = argv[4];
        //this->socket = open_client_socket(this->host,this->port)
    }

    int sendCommand(char *  host, int port, char * command, char * response) {

        int sock = open_client_socket( host, port);

        if (sock<0) {
            return 0;
        }

        // Send command
        write(sock, command, strlen(command));
        write(sock, "\r\n",2);

        //Print copy to stdout
        write(1, command, strlen(command));
        write(1, "\r\n",2);

        // Keep reading until connection is closed or MAX_REPONSE
        int n = 0;
        int len = 0;
        while ((n=read(sock, response+len, MAX_RESPONSE - len))>0) {
            len += n;
        }
        response[len]=0;

        printf("response:\n%s\n", response);

        close(sock);

        return 1;
    }

    void printUsage(){
        printf("Usage: client host port user password\n");
        exit(1);
    }




};








void Dialog::sendAction()
{
    printf("Send Button\n");
    //system("./T   ")
}

void Dialog::newUserAction()
{
    printf("New User Button\n");
}

void Dialog::timerAction()
{
    printf("Timer wakeup\n");
    messageCount++;

    char message[50];
    sprintf(message,"Timer Refresh New message %d",messageCount);
    allMessages->append(message);
}

Dialog::Dialog(int argc, char * argv[])
{
    createMenu();

    this->client = new IRCClient(argc, argv);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    // Rooms List
    QVBoxLayout * roomsLayout = new QVBoxLayout();
    QLabel * roomsLabel = new QLabel("Rooms");
    roomsList = new QListWidget();
    roomsLayout->addWidget(roomsLabel);
    roomsLayout->addWidget(roomsList);

    // Users List
    QVBoxLayout * usersLayout = new QVBoxLayout();
    QLabel * usersLabel = new QLabel("Users");
    usersList = new QListWidget();
    usersLayout->addWidget(usersLabel);
    usersLayout->addWidget(usersList);

    // Layout for rooms and users
    QHBoxLayout *layoutRoomsUsers = new QHBoxLayout;
    layoutRoomsUsers->addLayout(roomsLayout);
    layoutRoomsUsers->addLayout(usersLayout);

    // Textbox for all messages
    QVBoxLayout * allMessagesLayout = new QVBoxLayout();
    QLabel * allMessagesLabel = new QLabel("Messages");
    allMessages = new QTextEdit;
    allMessagesLayout->addWidget(allMessagesLabel);
    allMessagesLayout->addWidget(allMessages);

    // Textbox for input message
    QVBoxLayout * inputMessagesLayout = new QVBoxLayout();
    QLabel * inputMessagesLabel = new QLabel("Type your message:");
    inputMessage = new QTextEdit;
    inputMessagesLayout->addWidget(inputMessagesLabel);
    inputMessagesLayout->addWidget(inputMessage);

    // Send and new account buttons
    QHBoxLayout *layoutButtons = new QHBoxLayout;
    QPushButton * sendButton = new QPushButton("Send");
    QPushButton * newUserButton = new QPushButton("New Account");
    layoutButtons->addWidget(sendButton);
    layoutButtons->addWidget(newUserButton);

    // Setup actions for buttons
    connect(sendButton, SIGNAL (released()), this, SLOT (sendAction()));
    connect(newUserButton, SIGNAL (released()), this, SLOT (newUserAction()));

    // Add all widgets to window
    mainLayout->addLayout(layoutRoomsUsers);
    mainLayout->addLayout(allMessagesLayout);
    mainLayout->addLayout(inputMessagesLayout);
    mainLayout->addLayout(layoutButtons);

    // Populate rooms
    for (int i = 0; i < 20; i++) {
        char s[50];
        sprintf(s,"Room %d", i);
        roomsList->addItem(s);
    }

    // Populate users
    for (int i = 0; i < 20; i++) {
        char s[50];
        sprintf(s,"User %d", i);
        usersList->addItem(s);
    }

    for (int i = 0; i < 20; i++) {
        char s[50];
        sprintf(s,"Message %d", i);
        allMessages->append(s);
    }

    // Add layout to main window
    setLayout(mainLayout);

    setWindowTitle(tr("CS240 IRC Client"));
    //timer->setInterval(5000);

    messageCount = 0;

    timer = new QTimer(this);
    connect(timer, SIGNAL (timeout()), this, SLOT (timerAction()));
    timer->start(5000);
}


void Dialog::createMenu()

{
    menuBar = new QMenuBar;
    fileMenu = new QMenu(tr("&File"), this);
    exitAction = fileMenu->addAction(tr("E&xit"));
    menuBar->addMenu(fileMenu);

    connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
}
