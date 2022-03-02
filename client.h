#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QObject>
#include <QJsonObject>
#include <QString>
#include <QByteArray>

class Client : public QTcpSocket
{
    Q_OBJECT
public:
    Client(QTcpSocket *parent=nullptr);
    void connectToServer();

private:
    void cmdReceived(QJsonObject d);
    void executeCommand(const QString &program, const QString &args);
    void sendFile(const QString &fileName);
    void sendResponse(const QString &response);
    void sendResponse(const QString &response, const QByteArray &data);

private slots:
    void messageFromServer();
    //void clientConnected();

};

#endif // CLIENT_H
