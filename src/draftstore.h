#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

class DraftStore : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit DraftStore(QObject *parent = nullptr);

    Q_INVOKABLE QString newKey();

    Q_INVOKABLE void write(const QString &key, const QString &path, const QString &text);
    Q_INVOKABLE void discard(const QString &key);

    Q_INVOKABLE QVariantList pending() const;

private:
    QString ensureDirectory() const;
    QString fileFor(const QString &key) const;

    QString m_directory;
};
