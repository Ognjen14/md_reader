#include "draftstore.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>
#include <QStringConverter>
#include <QTextStream>
#include <QUuid>
#include <QVariantMap>

namespace {
const QLatin1String Magic("MDReaderDraft/1");
const QLatin1String Suffix(".draft");
}

DraftStore::DraftStore(QObject *parent)
    : QObject(parent)
{
    m_directory = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
                  + QStringLiteral("/drafts");
}

QString DraftStore::ensureDirectory() const
{
    QDir().mkpath(m_directory);
    return m_directory;
}

QString DraftStore::fileFor(const QString &key) const
{
    return m_directory + QLatin1Char('/') + key + Suffix;
}

QString DraftStore::newKey()
{
    return QUuid::createUuid().toString(QUuid::Id128);
}

void DraftStore::write(const QString &key, const QString &path, const QString &text)
{
    if (key.isEmpty())
        return;

    ensureDirectory();

    QSaveFile file(fileFor(key));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    out << Magic << '\n' << path << '\n' << '\n' << text;
    out.flush();

    file.commit();
}

void DraftStore::discard(const QString &key)
{
    if (key.isEmpty())
        return;
    QFile::remove(fileFor(key));
}

QVariantList DraftStore::pending() const
{
    QVariantList drafts;

    QDir dir(m_directory);
    if (!dir.exists())
        return drafts;

    const QFileInfoList entries =
        dir.entryInfoList({QStringLiteral("*") + Suffix}, QDir::Files, QDir::Time);

    for (const QFileInfo &info : entries) {
        QFile file(info.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly))
            continue;

        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8);

        if (in.readLine() != Magic)
            continue;

        const QString path = in.readLine();
        in.readLine();
        const QString text = in.readAll();

        QVariantMap draft;
        draft.insert(QStringLiteral("key"), info.completeBaseName());
        draft.insert(QStringLiteral("path"), path);
        draft.insert(QStringLiteral("text"), text);
        drafts.append(draft);
    }

    return drafts;
}
