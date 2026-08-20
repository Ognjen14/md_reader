#pragma once

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QtQml/qqmlregistration.h>

class DocumentOutline : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        LevelRole,
        DepthRole,
        LineRole
    };
    Q_ENUM(Roles)

    explicit DocumentOutline(QObject *parent = nullptr);

    QString source() const { return m_source; }
    void setSource(const QString &source);

    int count() const { return int(m_headings.size()); }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE int lineAt(int index) const;

    Q_INVOKABLE int indexForLine(int line) const;

signals:
    void sourceChanged();
    void countChanged();

private:
    struct Heading {
        QString title;
        int level = 1;
        int depth = 0;
        int line = 0;
    };

    void rebuild();

    QString m_source;
    QList<Heading> m_headings;
};
