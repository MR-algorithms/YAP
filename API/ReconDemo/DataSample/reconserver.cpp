#include "reconserver.h"
#include "reconclientsocket.h"

ReconServer::ReconServer(QObject * parent, int port) :
    QTcpServer(parent),
    clientSocket(nullptr)
{
    listen(QHostAddress::Any, port);
}

void ReconServer::slotDataReceived(int length)
{
    emit signalDataReceived(length);
}

void ReconServer::slotDisconnected(int socketDescriptor)
{
    clientSocket->close();
    //..clientSocket->disconnectFromHost();

    //..clientSocket->waitForDisconnected();
    //delete clientSocket;
    clientSocket = nullptr;
    emit signalDisconnected(socketDescriptor);
}

void ReconServer::incomingConnection(qintptr socketDescriptor)
{
    if (clientSocket != nullptr)
    {
        // output error message: only one connection allowed
        return;
    }

    try
    {
        clientSocket = new ReconClientSocket(this);
    }
    catch (std::bad_alloc& )
    {
        qDebug() << "Error allocating memroy for ReconClientSocket.";
        return;
    }

    connect(clientSocket, &ReconClientSocket::signalDisconnected, this, &ReconServer::slotDisconnected);
    connect(clientSocket, &ReconClientSocket::signalDataReceived, this, &ReconServer::slotDataReceived);

    clientSocket->setSocketDescriptor(socketDescriptor);
    //this->addPendingConnection(clientSocket);//?
}





