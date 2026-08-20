#include "singleinstance.h"

#include <QLocalSocket>

#include <memory>

#ifdef Q_OS_WIN
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#endif

namespace {
constexpr int ConnectTimeoutMs = 500;
constexpr int WriteTimeoutMs = 1000;
}

SingleInstance::SingleInstance(const QString &key, QObject *parent)
    : QObject(parent)
    , m_key(key)
{
    connect(&m_server, &QLocalServer::newConnection, this, &SingleInstance::acceptConnection);
}

bool SingleInstance::handOff(const QString &payload)
{
    QLocalSocket socket;
    socket.connectToServer(m_key);
    if (!socket.waitForConnected(ConnectTimeoutMs))
        return false;

#ifdef Q_OS_WIN
    AllowSetForegroundWindow(ASFW_ANY);
#endif

    socket.write(payload.toUtf8());
    socket.write("\n");
    socket.flush();
    socket.waitForBytesWritten(WriteTimeoutMs);
    socket.disconnectFromServer();
    return true;
}

bool SingleInstance::listen()
{
    if (m_server.listen(m_key))
        return true;
    QLocalServer::removeServer(m_key);
    return m_server.listen(m_key);
}

void SingleInstance::acceptConnection()
{
    QLocalSocket *socket = m_server.nextPendingConnection();
    if (!socket)
        return;

    auto buffer = std::make_shared<QByteArray>();

    connect(socket, &QLocalSocket::readyRead, this, [this, socket, buffer]() {
        buffer->append(socket->readAll());

        for (int end = buffer->indexOf('\n'); end != -1; end = buffer->indexOf('\n')) {
            const QByteArray line = buffer->left(end);
            buffer->remove(0, end + 1);
            emit messageReceived(QString::fromUtf8(line));
        }
    });

    connect(socket, &QLocalSocket::disconnected, socket, &QLocalSocket::deleteLater);
}
