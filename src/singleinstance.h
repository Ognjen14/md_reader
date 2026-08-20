#pragma once

#include <QLocalServer>
#include <QObject>
#include <QString>

class SingleInstance : public QObject
{
    Q_OBJECT

public:
    explicit SingleInstance(const QString &key, QObject *parent = nullptr);

    bool handOff(const QString &payload);
    bool listen();

signals:
    void messageReceived(const QString &payload);

private:
    void acceptConnection();

    QString m_key;
    QLocalServer m_server;
};
