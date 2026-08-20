#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QtQml/qqmlregistration.h>

class FontCatalog : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QStringList families READ families CONSTANT)
    Q_PROPERTY(QStringList monoFamilies READ monoFamilies CONSTANT)
    Q_PROPERTY(QStringList bundled READ bundled CONSTANT)

public:
    explicit FontCatalog(QObject *parent = nullptr);

    static void loadBundled();

    QStringList families() const { return m_families; }
    QStringList monoFamilies() const { return m_monoFamilies; }
    QStringList bundled() const { return s_bundled; }

private:
    static QStringList s_bundled;

    QStringList m_families;
    QStringList m_monoFamilies;
};
