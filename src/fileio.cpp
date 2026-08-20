#include "fileio.h"

#include <QFile>
#include <QFileInfo>
#include <QSaveFile>

namespace {
const char *const Utf8Bom = "\xEF\xBB\xBF";
constexpr int BomLength = 3;
}

FileIO::FileIO(QObject *parent)
    : QObject(parent)
{
    m_watchDebounce.setSingleShot(true);
    m_watchDebounce.setInterval(250);
    connect(&m_watchDebounce, &QTimer::timeout, this, &FileIO::checkDiskContents);
    connect(&m_watcher, &QFileSystemWatcher::fileChanged, this, &FileIO::onWatcherFired);
}

QString FileIO::fileName() const
{
    return m_path.isEmpty() ? QString() : QFileInfo(m_path).fileName();
}

QString FileIO::urlToPath(const QUrl &url) const
{
    if (url.isEmpty())
        return QString();
    return url.isLocalFile() ? url.toLocalFile() : url.toString();
}

QUrl FileIO::pathToUrl(const QString &path) const
{
    return path.isEmpty() ? QUrl() : QUrl::fromLocalFile(path);
}

QUrl FileIO::folderUrl(const QString &path) const
{
    if (path.isEmpty())
        return QUrl();
    return QUrl::fromLocalFile(QFileInfo(path).absolutePath());
}

bool FileIO::exists(const QString &path) const
{
    return !path.isEmpty() && QFileInfo::exists(path);
}

bool FileIO::load(const QUrl &url)
{
    return loadPath(urlToPath(url));
}

bool FileIO::loadPath(const QString &path)
{
    if (path.isEmpty()) {
        setError(tr("No file selected."));
        return false;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        setError(tr("Could not open %1: %2").arg(QFileInfo(path).fileName(), file.errorString()));
        return false;
    }
    const QByteArray raw = file.readAll();
    file.close();

    QByteArray body = raw;
    m_hadBom = body.startsWith(Utf8Bom);
    if (m_hadBom)
        body.remove(0, BomLength);

    QString text = QString::fromUtf8(body);
    m_eol = text.contains(QLatin1String("\r\n")) ? QStringLiteral("\r\n") : QStringLiteral("\n");
    text.replace(QLatin1String("\r\n"), QLatin1String("\n"));
    text.replace(QLatin1Char('\r'), QLatin1Char('\n'));

    m_raw = raw;
    m_text = text;
    setError(QString());
    setPath(QFileInfo(path).absoluteFilePath());
    watch(m_path);

    emit loaded(m_text);
    return true;
}

bool FileIO::reload()
{
    return m_path.isEmpty() ? false : loadPath(m_path);
}

bool FileIO::save(const QString &text)
{
    if (m_path.isEmpty()) {
        setError(tr("No file to save to."));
        return false;
    }
    return writeTo(m_path, text);
}

bool FileIO::saveAs(const QUrl &url, const QString &text)
{
    const QString path = urlToPath(url);
    if (path.isEmpty()) {
        setError(tr("No file selected."));
        return false;
    }
    return writeTo(QFileInfo(path).absoluteFilePath(), text);
}

bool FileIO::writeTo(const QString &path, const QString &text)
{
    QByteArray out;
    if (text == m_text && !m_raw.isEmpty() && path == m_path) {
        out = m_raw;
    } else {
        QString body = text;
        if (m_eol != QLatin1String("\n"))
            body.replace(QLatin1Char('\n'), m_eol);
        out = body.toUtf8();
        if (m_hadBom)
            out.prepend(Utf8Bom, BomLength);
    }

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        setError(tr("Could not write %1: %2").arg(QFileInfo(path).fileName(), file.errorString()));
        return false;
    }
    if (file.write(out) != out.size() || !file.commit()) {
        setError(tr("Could not write %1: %2").arg(QFileInfo(path).fileName(), file.errorString()));
        return false;
    }

    m_raw = out;
    m_text = text;
    setError(QString());
    setPath(path);
    watch(m_path);

    emit saved();
    return true;
}

void FileIO::reset()
{
    watch(QString());
    m_raw.clear();
    m_text.clear();
    m_eol = QStringLiteral("\n");
    m_hadBom = false;
    setError(QString());
    setPath(QString());
}

void FileIO::setPath(const QString &path)
{
    if (m_path == path)
        return;
    m_path = path;
    emit filePathChanged();
}

void FileIO::setError(const QString &message)
{
    if (m_errorString != message) {
        m_errorString = message;
        emit errorStringChanged();
    }
    if (!message.isEmpty())
        emit error(message);
}

void FileIO::watch(const QString &path)
{
    const QStringList watched = m_watcher.files();
    if (!watched.isEmpty())
        m_watcher.removePaths(watched);
    m_watchDebounce.stop();
    if (!path.isEmpty())
        m_watcher.addPath(path);
}

void FileIO::onWatcherFired(const QString &path)
{
    Q_UNUSED(path)
    m_watchDebounce.start();
}

void FileIO::checkDiskContents()
{
    if (m_path.isEmpty())
        return;
    if (!m_watcher.files().contains(m_path) && QFileInfo::exists(m_path))
        m_watcher.addPath(m_path);

    QFile file(m_path);
    if (!file.open(QIODevice::ReadOnly))
        return;
    const QByteArray raw = file.readAll();
    file.close();

    if (raw == m_raw)
        return;

    emit fileChangedOnDisk();
}
