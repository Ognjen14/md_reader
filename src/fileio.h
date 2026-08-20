#pragma once

#include <QByteArray>
#include <QFileSystemWatcher>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class FileIO : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString filePath READ filePath NOTIFY filePathChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY filePathChanged)
    Q_PROPERTY(bool hasFile READ hasFile NOTIFY filePathChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged)

public:
    explicit FileIO(QObject *parent = nullptr);

    QString filePath() const { return m_path; }
    QString fileName() const;
    bool hasFile() const { return !m_path.isEmpty(); }
    QString errorString() const { return m_errorString; }

    Q_INVOKABLE bool load(const QUrl &url);
    Q_INVOKABLE bool loadPath(const QString &path);
    Q_INVOKABLE bool reload();

    Q_INVOKABLE bool save(const QString &text);
    Q_INVOKABLE bool saveAs(const QUrl &url, const QString &text);

    Q_INVOKABLE void reset();

    Q_INVOKABLE QString urlToPath(const QUrl &url) const;
    Q_INVOKABLE QUrl pathToUrl(const QString &path) const;
    Q_INVOKABLE QUrl folderUrl(const QString &path) const;
    Q_INVOKABLE bool exists(const QString &path) const;

signals:
    void loaded(const QString &text);
    void saved();
    void filePathChanged();
    void errorStringChanged();
    void error(const QString &message);
    void fileChangedOnDisk();

private:
    bool writeTo(const QString &path, const QString &text);
    void setPath(const QString &path);
    void setError(const QString &message);
    void watch(const QString &path);
    void onWatcherFired(const QString &path);
    void checkDiskContents();

    QString m_path;
    QString m_errorString;

    QByteArray m_raw;
    QString m_text;
    QString m_eol = QStringLiteral("\n");
    bool m_hadBom = false;

    QFileSystemWatcher m_watcher;
    QTimer m_watchDebounce;
};
