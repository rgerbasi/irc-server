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

#include <QApplication>

#include <QtWidgets>

#include "dialog.h"


#define MAX_RESPONSE (10 * 1024)

class IRCClient;
//addition number2 class to check user and password
class Verification : public QDialog{
public:
    void loginAction(char * username, char * password, int * success){
        printf("log in action\n");
        //heres where we do the thing;
        printf("user : %s\npass: %s\n", username, password);

        *success = 1;

    }

    //constructor
    Verification(int argc, char * argv[]){

        //USER VERIFICATION HAPPENS HERE
        createMenu();
        QVBoxLayout * mainLayout = new QVBoxLayout;
        QHBoxLayout * loginLayout = new QHBoxLayout();

        QVBoxLayout * userBox = new QVBoxLayout();
        QLabel * usernameLabel = new QLabel("Username:");
        QLineEdit * usernameText = new QLineEdit;
        userBox->addWidget(usernameLabel);
        userBox->addWidget(usernameText);

        QVBoxLayout * passwordBox = new QVBoxLayout();
        QLabel * passwordLabel = new QLabel("Password: ");
        QLineEdit * passwordText = new QLineEdit;
        passwordBox->addWidget(passwordLabel);
        passwordBox->addWidget(passwordText);

        loginLayout->addLayout(userBox);
        loginLayout->addLayout(passwordBox);

        QHBoxLayout *layoutButtons = new QHBoxLayout;
        QPushButton * logInButton = new QPushButton("Log In");
        layoutButtons->addWidget(logInButton);

        mainLayout->addLayout(loginLayout);
        mainLayout->addLayout(layoutButtons);

        printf("Verification setlayout manlayout\n");

        setLayout(mainLayout);

        char * username = (char *)usernameText->text().toStdString().c_str();
        char * password = (char *) passwordText->text().toStdString().c_str();
        int * success = 0;
        //client and verification steps
        IRCClient * client = new IRCClient(argc, argv);
        connect(logInButton, SIGNAL (released()), this, SLOT (loginAction(username, password, success)));

        if(*success){
            Dialog dialog(client);
            dialog.show();
            dialog.exec();
        }
    }




    void createMenu(){
        QMenuBar * menuBar = new QMenuBar;
        QMenu * fileMenu = new QMenu(tr("&File"), this);
        QAction * exitAction = fileMenu->addAction(tr("E&xit"));
        menuBar->addMenu(fileMenu);

        connect(exitAction, SIGNAL(triggered()), this, SLOT(accept()));
    }
};

//end of verification class

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //IRCClient * client = new IRCClient(argc, argv);
    Verification dialog(argc, argv);
    dialog.show();

    return app.exec();

}

