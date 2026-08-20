#include "documentoutline.h"

#include <QRegularExpression>

namespace {

struct FenceToken {
    QString run;
    bool canClose = false;
};

FenceToken fenceTokenFor(const QString &line)
{
    static const QRegularExpression fence(QStringLiteral("^ {0,3}(`{3,}|~{3,})"));
    const QRegularExpressionMatch match = fence.match(line);
    if (!match.hasMatch())
        return {};

    return {match.captured(1), line.mid(match.capturedEnd(1)).trimmed().isEmpty()};
}

QString plainTitle(QString text)
{
    static const QRegularExpression code(QStringLiteral("`+([^`]*)`+"));
    static const QRegularExpression image(QStringLiteral("!\\[([^\\]]*)\\]\\([^)]*\\)"));
    static const QRegularExpression inlineLink(QStringLiteral("\\[([^\\]]*)\\]\\([^)]*\\)"));
    static const QRegularExpression refLink(QStringLiteral("\\[([^\\]]*)\\]\\[[^\\]]*\\]"));
    static const QRegularExpression strong(QStringLiteral("(\\*\\*|__)(.+?)\\1"));
    static const QRegularExpression emphasis(QStringLiteral("(\\*|_)(?=\\S)(.+?)(?<=\\S)\\1"));
    static const QRegularExpression strike(QStringLiteral("~~(.+?)~~"));
    static const QRegularExpression escaped(QStringLiteral("\\\\([\\\\`*_{}\\[\\]()#+\\-.!~>])"));
    static const QRegularExpression whitespace(QStringLiteral("\\s+"));

    text.replace(code, QStringLiteral("\\1"));
    text.replace(image, QStringLiteral("\\1"));
    text.replace(inlineLink, QStringLiteral("\\1"));
    text.replace(refLink, QStringLiteral("\\1"));
    text.replace(strong, QStringLiteral("\\2"));
    text.replace(emphasis, QStringLiteral("\\2"));
    text.replace(strike, QStringLiteral("\\1"));
    text.replace(escaped, QStringLiteral("\\1"));
    text.replace(whitespace, QStringLiteral(" "));

    return text.trimmed();
}

}

DocumentOutline::DocumentOutline(QObject *parent)
    : QAbstractListModel(parent)
{
}

void DocumentOutline::setSource(const QString &source)
{
    if (m_source == source)
        return;
    m_source = source;
    emit sourceChanged();
    rebuild();
}

void DocumentOutline::rebuild()
{
    static const QRegularExpression atx(
        QStringLiteral("^ {0,3}(#{1,6})(?:[ \\t]+(.*?))?[ \\t]*$"));
    static const QRegularExpression atxClosing(QStringLiteral("[ \\t]+#+[ \\t]*$"));
    static const QRegularExpression setext(QStringLiteral("^ {0,3}(=+|-{2,})[ \\t]*$"));

    beginResetModel();
    m_headings.clear();

    const QStringList lines = m_source.split(QLatin1Char('\n'));

    QString openFence;
    bool inFrontMatter = false;
    QString previousLine;
    int previousLineIndex = -1;

    for (int i = 0; i < lines.size(); ++i) {
        const QString &line = lines.at(i);
        const QString trimmed = line.trimmed();

        if (i == 0 && trimmed == QLatin1String("---")) {
            inFrontMatter = true;
            continue;
        }
        if (inFrontMatter) {
            if (trimmed == QLatin1String("---") || trimmed == QLatin1String("..."))
                inFrontMatter = false;
            continue;
        }

        const FenceToken fence = fenceTokenFor(line);
        if (!openFence.isEmpty()) {
            if (!fence.run.isEmpty() && fence.run.at(0) == openFence.at(0)
                && fence.run.size() >= openFence.size() && fence.canClose) {
                openFence.clear();
            }
            previousLine.clear();
            continue;
        }
        if (!fence.run.isEmpty()) {
            openFence = fence.run;
            previousLine.clear();
            continue;
        }

        const QRegularExpressionMatch atxMatch = atx.match(line);
        if (atxMatch.hasMatch()) {
            QString title = atxMatch.captured(2);
            title.remove(atxClosing);
            m_headings.append({plainTitle(title), int(atxMatch.captured(1).size()), 0, i});
            previousLine.clear();
            continue;
        }

        if (!previousLine.trimmed().isEmpty() && setext.match(line).hasMatch()) {
            const int level = trimmed.startsWith(QLatin1Char('=')) ? 1 : 2;
            m_headings.append({plainTitle(previousLine.trimmed()), level, 0, previousLineIndex});
            previousLine.clear();
            continue;
        }

        previousLine = line;
        previousLineIndex = i;
    }

    int shallowest = 6;
    for (const Heading &heading : std::as_const(m_headings))
        shallowest = qMin(shallowest, heading.level);
    for (Heading &heading : m_headings)
        heading.depth = heading.level - shallowest;

    endResetModel();
    emit countChanged();
}

int DocumentOutline::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : int(m_headings.size());
}

QVariant DocumentOutline::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_headings.size())
        return {};

    const Heading &heading = m_headings.at(index.row());
    switch (role) {
    case TitleRole:
        return heading.title;
    case LevelRole:
        return heading.level;
    case DepthRole:
        return heading.depth;
    case LineRole:
        return heading.line;
    default:
        return {};
    }
}

QHash<int, QByteArray> DocumentOutline::roleNames() const
{
    return {
        {TitleRole, QByteArrayLiteral("title")},
        {LevelRole, QByteArrayLiteral("level")},
        {DepthRole, QByteArrayLiteral("depth")},
        {LineRole, QByteArrayLiteral("line")},
    };
}

int DocumentOutline::lineAt(int index) const
{
    if (index < 0 || index >= m_headings.size())
        return -1;
    return m_headings.at(index).line;
}

int DocumentOutline::indexForLine(int line) const
{
    int result = -1;
    for (int i = 0; i < m_headings.size(); ++i) {
        if (m_headings.at(i).line > line)
            break;
        result = i;
    }
    return result;
}
