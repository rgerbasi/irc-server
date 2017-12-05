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


//host: 120.10.12.218
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
//IRCClient
IRCClient::IRCClient(){
    //constructor
    this->host = (char *) "";
    this->port = 0;
    this->username = (char *) "";
    this->password = (char *) "";

}

int IRCClient::open_client_socket(char * host, int port){
    //open client socket
    // Initialize socket address structure
    printf("in open client socket\n");
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
        printf("get host by name returned null\n");
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

    int sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    // Connect the socket to the specified server
   // printf("before connecting socket\n");
    if (connect(sock, (struct sockaddr *)&socketAddress,
            sizeof(socketAddress)) < 0) {
        printf("error from connect\n");
        perror("connect");
        exit(1);
    }

    //printf("AAH\n");
    return sock;

}
int IRCClient::sendCommand(char *  host, int port, char * command, char * response){
    //send command
    printf("SEND COMMAND TO SERVER HOST=%s port=%d command=%s\n",host, port, command);
    int sock = open_client_socket( host, port);
    //printf("after openclient socket\n");
    if (sock<0) {
        return 0;
    }

    // Send command
    printf("writing command to socket\n");
    write(sock, command, strlen(command));
    write(sock, "\r\n",2);

    //Print copy to stdout
    //write(1, command, strlen(command));
    //write(1, "\r\n",2);

    // Keep reading until connection is closed or MAX_REPONSE
    printf("response building\n");
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
//end of IRC CLIENT class
//slots of verification class
void Verification::printUsage(){
    printf("Usage: client host port\n");
    exit(1);
}
void Verification::createMenu(){
    QMenuBar * menuBar = new QMenuBar;
    QMenu * fileMenu = new QMenu(tr("&File"), this);
    QAction * exitAction = fileMenu->addAction(tr("E&xit"));
    menuBar->addMenu(fileMenu);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
}
void Verification::loginAction(){
//login action user clien tot talk to server
    //construct dialog here?
   //LineEdit*test;

    char * username = strdup((char *) usernameText->text().toStdString().c_str());
    char * password = strdup((char *) passwordText->text().toStdString().c_str());
    //printf("User is %s\npass is %s\n", username, password);
    std::string command = "LOG-IN";
    command = command + " " + username + " " + password;
    char * response = new char[MAX_RESPONSE];
    //printf("command is %s\n", command.c_str());
    //sending command ot server to check log in


    client->sendCommand(client->host,client->port,(char *)command.c_str(), response);
    //printf("respone is %s\n", response);
    if(!strcmp(response, "OK\r\n")){
        QMessageBox msgbox;
        msgbox.setText("Welcome Sister.");
        msgbox.exec();
        client->username = username;
        client->password = password;
        Dialog dialog(client,this);
        printf("before dialog show\n");
        dialog.show();
        //dialog.exec();

        } else {
        QMessageBox msgbox;
        msgbox.setText("Log in error.");
        msgbox.exec();
    }
}
void Verification::newUserAction(){
//new user actoin use clien tto talk to server
    char * username = (char *) usernameText->text().toStdString().c_str();
    char * password = (char *) passwordText->text().toStdString().c_str();
    std::string command = "ADD-USER";
    command = command + " " + username + " " + password;
    char * response = new char[MAX_RESPONSE];
    client->sendCommand(client->host,client->port,(char *)command.c_str(), response);
    if(!strcmp(response, "OK\r\n")){
        QMessageBox msgbox;
        msgbox.setText("New Sister added.");
        msgbox.exec();
    }
}
Verification::Verification(int argc, char *argv[]){
    if (argc != 3) printUsage();

    char * host = argv[1];
    int port = atoi(argv[2]);

       //build client
    client = new IRCClient();
    client->host = host;
    client->port = port;
   //building gui
    createMenu();
    QVBoxLayout * mainLayout = new QVBoxLayout;
    QHBoxLayout * loginLayout = new QHBoxLayout();
    QVBoxLayout * userBox = new QVBoxLayout();
    QLabel * usernameLabel = new QLabel("Username:");
    usernameText = new QLineEdit;
    userBox->addWidget(usernameLabel);
    userBox->addWidget(usernameText);
    QVBoxLayout * passwordBox = new QVBoxLayout();
    QLabel * passwordLabel = new QLabel("Password: ");
    passwordText = new QLineEdit;
    passwordBox->addWidget(passwordLabel);
    passwordBox->addWidget(passwordText);
    loginLayout->addLayout(userBox);
    loginLayout->addLayout(passwordBox);
    QHBoxLayout *layoutButtons = new QHBoxLayout;
    QPushButton * logInButton = new QPushButton("Log In");
    QPushButton * addnewUser = new QPushButton("New User");
    layoutButtons->addWidget(addnewUser);
    layoutButtons->addWidget(logInButton);
    mainLayout->addLayout(loginLayout);
    mainLayout->addLayout(layoutButtons);
    setLayout(mainLayout);

    connect(logInButton, SIGNAL (released()), this, SLOT (loginAction()));
    connect(addnewUser, SIGNAL (released()), this, SLOT (newUserAction()));


}
//reminder : command is one string but it will contain all the arguements for commands
//host: 120.10.12.218
void Dialog::sendAction()
{    
     printf("Send Button\n");
     //char response[MAX_RESPONSE];
    //client->sendCommand(client->host, client->port)

}
void Dialog::createRoomAction(){

    printf("create room button");
    bool ok;
    QString room = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("Room name:"), QLineEdit::Normal, QDir::home().dirName(), &ok);
    if(ok && !room.isEmpty()){
        //if user clicked ok and text isnt empty we will create a room
        printf("WOO\n");

    }

}
void Dialog::newUserAction()
{
    printf("New User Button\n");


/*
    std::string command = "ADD-USER";
    command = command + " " + username + " " + password;
    char * response = new char[MAX_RESPONSE];
    client->sendCommand(client->host,client->port,(char *)command.c_str(), response);
    if(!strcmp(response, "OK\r\n")){
        QMessageBox msgbox;
        msgbox.setText("New Sister added.");
        msgbox.exec();
    }
 */
}

void Dialog::timerAction()
{
    printf("Timer wakeup\n");
    messageCount++;

    char message[50];
    sprintf(message,"Timer Refresh New message %d",messageCount);
    allMessages->append(message);
}

Dialog::Dialog(IRCClient * client, Verification * verification)
{
    createMenu();
    //
    this->client = client;

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
    //adition
    QPushButton * enterRoomButton = new QPushButton("Enter Room");
    layoutButtons->addWidget(enterRoomButton);
    layoutButtons->addWidget(sendButton);

    // Setup actions for buttons
    connect(sendButton, SIGNAL (released()), this, SLOT (sendAction()));
    connect(createRoomButton, SIGNAL (released()), this, SLOT (createRoomAction()));

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

    // Add layout to main windo

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


//NEW WINDOW FOR USER VERIFICATION CLASS

