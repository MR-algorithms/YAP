#include "reconserver.h"
#include "reconclientsocket.h"

ReconServer::ReconServer(QObject * parent, int port) :
    QTcpServer(parent),
    clientSocket(nullptr)
{
    listen(QHostAddress::Any, port);
}

void ReconServer::slotDataReceived(QString message, int length)
{
    emit signalDataReceived(message, length);
}

void ReconServer::slotDisconnected(int socketDescriptor)
{
    delete clientSocket;
    clientSocket = nullptr;
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
}





