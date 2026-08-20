#include "fontcatalog.h"

#include <QDirIterator>
#include <QFontDatabase>

QStringList FontCatalog::s_bundled;

void FontCatalog::loadBundled()
{
    QDirIterator it(QStringLiteral(":/qt/qml/MDReader/fonts"),
                    QDirIterator::Subdirectories);

    while (it.hasNext()) {
        const QString path = it.next();
        if (!path.endsWith(QLatin1String(".ttf"), Qt::CaseInsensitive)
            && !path.endsWith(QLatin1String(".otf"), Qt::CaseInsensitive)) {
            continue;
        }

        const int id = QFontDatabase::addApplicationFont(path);
        if (id < 0)
            continue;

        const QStringList families = QFontDatabase::applicationFontFamilies(id);
        for (const QString &family : families) {
            if (!s_bundled.contains(family))
                s_bundled.append(family);
        }
    }

    s_bundled.sort(Qt::CaseInsensitive);
}

FontCatalog::FontCatalog(QObject *parent)
    : QObject(parent)
{
    const QStringList installed = QFontDatabase::families();
    m_families.reserve(installed.size());

    for (const QString &family : installed) {
        if (family.startsWith(QLatin1Char('@')))
            continue;

        m_families.append(family);
        if (QFontDatabase::isFixedPitch(family))
            m_monoFamilies.append(family);
    }
}
