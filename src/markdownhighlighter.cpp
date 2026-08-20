#include "markdownhighlighter.h"

#include <QRegularExpression>
#include <QTextBlock>
#include <QTextDocument>
#include <QTimer>

namespace {
constexpr qreal HeadingScale[6] = { 1.55, 1.35, 1.22, 1.12, 1.06, 1.0 };

const QRegularExpression &fencePattern()
{
    static const QRegularExpression re(QStringLiteral("^ {0,3}(`{3,}|~{3,})"));
    return re;
}

const QRegularExpression &quotePrefixPattern()
{
    static const QRegularExpression re(QStringLiteral("^ {0,3}(?:>[ \\t]?)+"));
    return re;
}

const QRegularExpression &headingPattern()
{
    static const QRegularExpression re(QStringLiteral("^ {0,3}(#{1,6})(?:([ \\t]+.*?)([ \\t]+#+)?[ \\t]*)?$"));
    return re;
}

const QRegularExpression &rulePattern()
{
    static const QRegularExpression re(QStringLiteral("^ {0,3}(?:(?:\\*[ \\t]*){3,}|(?:-[ \\t]*){3,}|(?:_[ \\t]*){3,})$"));
    return re;
}

const QRegularExpression &listPattern()
{
    static const QRegularExpression re(QStringLiteral("^([ \\t]*)([-*+]|\\d{1,9}[.)])([ \\t]+)(\\[[ xX]\\])?"));
    return re;
}

const QRegularExpression &setextPattern()
{
    static const QRegularExpression re(QStringLiteral("^ {0,3}(=+|-{2,})[ \\t]*$"));
    return re;
}

const QRegularExpression &codeSpanPattern()
{
    static const QRegularExpression re(QStringLiteral("(`+)([^`]+)\\1"));
    return re;
}

const QRegularExpression &imagePattern()
{
    static const QRegularExpression re(QStringLiteral("!\\[([^\\]]*)\\]\\(([^)]*)\\)"));
    return re;
}

const QRegularExpression &linkPattern()
{
    static const QRegularExpression re(QStringLiteral("(?<!!)\\[([^\\]]*)\\]\\(([^)]*)\\)"));
    return re;
}

const QRegularExpression &referenceLinkPattern()
{
    static const QRegularExpression re(QStringLiteral("(?<!!)\\[([^\\]]*)\\]\\[([^\\]]*)\\]"));
    return re;
}

const QRegularExpression &referenceDefPattern()
{
    static const QRegularExpression re(QStringLiteral("^ {0,3}(\\[[^\\]]+\\]:)([ \\t]*\\S+)"));
    return re;
}

const QRegularExpression &autoLinkPattern()
{
    static const QRegularExpression re(QStringLiteral("<(?:https?|mailto|ftp):[^ >]+>"));
    return re;
}

const QRegularExpression &strongPattern()
{
    static const QRegularExpression re(QStringLiteral("(\\*\\*|__)(?=\\S)(.+?)(?<=\\S)\\1"));
    return re;
}

const QRegularExpression &emphasisAsteriskPattern()
{
    static const QRegularExpression re(QStringLiteral("(?<!\\*)\\*(?!\\*|\\s)(.+?)(?<!\\s|\\*)\\*(?!\\*)"));
    return re;
}

const QRegularExpression &emphasisUnderscorePattern()
{
    static const QRegularExpression re(QStringLiteral("(?<![\\w_])_(?!_|\\s)(.+?)(?<!\\s|_)_(?![\\w_])"));
    return re;
}

const QRegularExpression &strikePattern()
{
    static const QRegularExpression re(QStringLiteral("~~(?=\\S)(.+?)(?<=\\S)~~"));
    return re;
}

const QRegularExpression &highlightPattern()
{
    static const QRegularExpression re(QStringLiteral("==(?=\\S)(.+?)(?<=\\S)=="));
    return re;
}

const QRegularExpression &footnoteRefPattern()
{
    static const QRegularExpression re(QStringLiteral("\\[\\^([^\\]\\s]+)\\]"));
    return re;
}

const QRegularExpression &footnoteDefPattern()
{
    static const QRegularExpression re(QStringLiteral("^ {0,3}(\\[\\^[^\\]\\s]+\\]:)"));
    return re;
}

const QRegularExpression &headingIdPattern()
{
    static const QRegularExpression re(QStringLiteral("\\{#[A-Za-z0-9_-]+\\}[ \\t]*$"));
    return re;
}

const QRegularExpression &definitionPattern()
{
    static const QRegularExpression re(QStringLiteral("^(:)([ \\t]+)(?=\\S)"));
    return re;
}

const QRegularExpression &subscriptPattern()
{
    static const QRegularExpression re(QStringLiteral("(?<!~)~(?!~)([^~\\s]+)~(?!~)"));
    return re;
}

const QRegularExpression &superscriptPattern()
{
    static const QRegularExpression re(QStringLiteral("\\^([^\\^\\s]+)\\^"));
    return re;
}

const QRegularExpression &emojiPattern()
{
    static const QRegularExpression re(QStringLiteral(":([a-z0-9_+-]{2,}):"));
    return re;
}


bool isEscaped(const QString &text, int position)
{
    int backslashes = 0;
    for (int i = position - 1; i >= 0 && text.at(i) == QLatin1Char('\\'); --i)
        ++backslashes;
    return (backslashes % 2) == 1;
}

}

MarkdownHighlighter::MarkdownHighlighter(QObject *parent)
    : QSyntaxHighlighter(parent)
{
    rebuildFormats();
}

void MarkdownHighlighter::setQuickDocument(QQuickTextDocument *document)
{
    if (m_quickDocument == document)
        return;
    m_quickDocument = document;
    QSyntaxHighlighter::setDocument(document ? document->textDocument() : nullptr);
    emit documentChanged();
}

void MarkdownHighlighter::schedulePaletteUpdate()
{
    rebuildFormats();
    emit paletteChanged();

    if (m_paletteUpdateQueued)
        return;
    m_paletteUpdateQueued = true;
    QTimer::singleShot(0, this, [this]() {
        m_paletteUpdateQueued = false;
        if (document())
            rehighlight();
    });
}

void MarkdownHighlighter::setBaseFontSize(qreal size)
{
    size = qBound(6.0, size, 48.0);
    if (qFuzzyCompare(m_baseFontSize, size))
        return;
    m_baseFontSize = size;
    schedulePaletteUpdate();
}

#define MD_COLOR_SETTER(Setter, Member)                 \
    void MarkdownHighlighter::Setter(const QColor &c)   \
    {                                                   \
        if (Member == c)                                \
            return;                                     \
        Member = c;                                     \
        schedulePaletteUpdate();                        \
    }

MD_COLOR_SETTER(setHeadingColor, m_headingColor)
MD_COLOR_SETTER(setBoldColor, m_boldColor)
MD_COLOR_SETTER(setItalicColor, m_italicColor)
MD_COLOR_SETTER(setCodeColor, m_codeColor)
MD_COLOR_SETTER(setCodeBackground, m_codeBackground)
MD_COLOR_SETTER(setInlineCodeColor, m_inlineCodeColor)
MD_COLOR_SETTER(setInlineCodeBackground, m_inlineCodeBackground)
MD_COLOR_SETTER(setLinkColor, m_linkColor)
MD_COLOR_SETTER(setMarkerColor, m_markerColor)
MD_COLOR_SETTER(setQuoteColor, m_quoteColor)
MD_COLOR_SETTER(setDelimiterColor, m_delimiterColor)
MD_COLOR_SETTER(setRuleColor, m_ruleColor)
MD_COLOR_SETTER(setHighlightBackground, m_highlightBackground)
MD_COLOR_SETTER(setCodeKeywordColor, m_codePalette.keyword)
MD_COLOR_SETTER(setCodeTypeColor, m_codePalette.type)
MD_COLOR_SETTER(setCodeStringColor, m_codePalette.string)
MD_COLOR_SETTER(setCodeNumberColor, m_codePalette.number)
MD_COLOR_SETTER(setCodeCommentColor, m_codePalette.comment)
MD_COLOR_SETTER(setCodeFunctionColor, m_codePalette.function)
MD_COLOR_SETTER(setCodeVariableColor, m_codePalette.variable)

#undef MD_COLOR_SETTER

void MarkdownHighlighter::rebuildFormats()
{
    m_boldFormat = QTextCharFormat();
    m_boldFormat.setFontWeight(QFont::Bold);
    if (m_boldColor.isValid())
        m_boldFormat.setForeground(m_boldColor);

    m_italicFormat = QTextCharFormat();
    m_italicFormat.setFontItalic(true);
    if (m_italicColor.isValid())
        m_italicFormat.setForeground(m_italicColor);

    m_codeFormat = QTextCharFormat();
    if (m_inlineCodeColor.isValid())
        m_codeFormat.setForeground(m_inlineCodeColor);

    if (m_inlineCodeBackground.isValid() && m_inlineCodeBackground.alpha() > 0)
        m_codeFormat.setBackground(m_inlineCodeBackground);

    m_codeBlockFormat = QTextCharFormat();
    if (m_codeColor.isValid())
        m_codeBlockFormat.setForeground(m_codeColor);
    if (m_codeBackground.isValid())
        m_codeBlockFormat.setBackground(m_codeBackground);

    m_fenceDelimiterFormat = QTextCharFormat();
    if (m_delimiterColor.isValid())
        m_fenceDelimiterFormat.setForeground(m_delimiterColor);
    if (m_codeBackground.isValid())
        m_fenceDelimiterFormat.setBackground(m_codeBackground);

    m_linkTextFormat = QTextCharFormat();
    if (m_linkColor.isValid())
        m_linkTextFormat.setForeground(m_linkColor);

    m_urlFormat = QTextCharFormat();
    if (m_delimiterColor.isValid())
        m_urlFormat.setForeground(m_delimiterColor);

    m_markerFormat = QTextCharFormat();
    m_markerFormat.setFontWeight(QFont::Bold);
    if (m_markerColor.isValid())
        m_markerFormat.setForeground(m_markerColor);

    m_quoteFormat = QTextCharFormat();
    if (m_quoteColor.isValid())
        m_quoteFormat.setForeground(m_quoteColor);

    m_delimiterFormat = QTextCharFormat();
    if (m_delimiterColor.isValid())
        m_delimiterFormat.setForeground(m_delimiterColor);

    m_ruleFormat = QTextCharFormat();
    if (m_ruleColor.isValid())
        m_ruleFormat.setForeground(m_ruleColor);

    m_strikeFormat = QTextCharFormat();
    m_strikeFormat.setFontStrikeOut(true);

    m_highlightFormat = QTextCharFormat();
    if (m_highlightBackground.isValid())
        m_highlightFormat.setBackground(m_highlightBackground);

    m_footnoteFormat = QTextCharFormat();
    if (m_linkColor.isValid())
        m_footnoteFormat.setForeground(m_linkColor);

    m_subscriptFormat = QTextCharFormat();
    m_subscriptFormat.setVerticalAlignment(QTextCharFormat::AlignSubScript);
    if (m_markerColor.isValid())
        m_subscriptFormat.setForeground(m_markerColor);

    m_superscriptFormat = QTextCharFormat();
    m_superscriptFormat.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    if (m_markerColor.isValid())
        m_superscriptFormat.setForeground(m_markerColor);
}

QTextCharFormat MarkdownHighlighter::headingFormat(int level) const
{
    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    format.setFontPointSize(m_baseFontSize * HeadingScale[qBound(1, level, 6) - 1]);
    if (m_headingColor.isValid())
        format.setForeground(m_headingColor);
    return format;
}

MarkdownHighlighter::Fence MarkdownHighlighter::fenceAt(const QString &text)
{
    Fence fence;
    const QRegularExpressionMatch match = fencePattern().match(text);
    if (!match.hasMatch())
        return fence;

    const QString run = match.captured(1);
    fence.kind = run.startsWith(QLatin1Char('`')) ? BacktickFence : TildeFence;
    fence.length = int(run.size());
    fence.end = match.capturedEnd(1);
    fence.canClose = text.mid(fence.end).trimmed().isEmpty();
    return fence;
}

void MarkdownHighlighter::applySpan(int start, int count, const QTextCharFormat &format)
{
    if (count <= 0)
        return;
    QTextCharFormat merged = QSyntaxHighlighter::format(start);
    merged.merge(format);
    setFormat(start, count, merged);
}

bool MarkdownHighlighter::isMasked(int start, int count) const
{
    for (int i = start; i < start + count && i < m_mask.size(); ++i) {
        if (m_mask.at(i))
            return true;
    }
    return false;
}

void MarkdownHighlighter::mask(int start, int count)
{
    for (int i = start; i < start + count && i < m_mask.size(); ++i)
        m_mask[i] = true;
}

void MarkdownHighlighter::highlightBlock(const QString &text)
{
    const int previousState = qMax(0, previousBlockState());
    const Fence fence = fenceAt(text);

    if (previousState != NoFence) {
        const int openKind = fenceKindOf(previousState);
        const int openLength = fenceLengthOf(previousState);
        const CodeSyntax::Language language = fenceLanguageOf(previousState);
        const bool closes = fence.kind == openKind && fence.length >= openLength
                            && fence.canClose;

        setFormat(0, text.length(), closes ? m_fenceDelimiterFormat : m_codeBlockFormat);
        if (closes) {
            setCurrentBlockState(NoFence);
        } else {
            const CodeSyntax::LineResult syntax = CodeSyntax::highlightLine(
                QStringView(text), language, syntaxStateOf(previousState));
            for (const CodeSyntax::Span &span : syntax.spans) {
                QTextCharFormat format;
                const QColor color = m_codePalette.color(span.role);
                if (!color.isValid())
                    continue;
                format.setForeground(color);
                applySpan(span.start, span.length, format);
            }
            Fence openFence;
            openFence.kind = openKind;
            openFence.length = openLength;
            setCurrentBlockState(encodeFence(openFence, language, syntax.state));
        }
        return;
    }

    if (fence.kind != NoFence) {
        setFormat(0, text.length(), m_fenceDelimiterFormat);
        if (fence.end < text.length())
            applySpan(fence.end, text.length() - fence.end, m_markerFormat);
        const CodeSyntax::Language language = CodeSyntax::languageFromInfo(
            QStringView(text).mid(fence.end));
        setCurrentBlockState(encodeFence(fence, language));
        return;
    }

    setCurrentBlockState(NoFence);

    if (text.isEmpty())
        return;

    m_mask.assign(text.length(), false);

    int contentStart = 0;
    const QRegularExpressionMatch quoteMatch = quotePrefixPattern().match(text);
    if (quoteMatch.hasMatch()) {
        contentStart = quoteMatch.capturedEnd(0);
        setFormat(0, text.length(), m_quoteFormat);
        applySpan(0, contentStart, m_markerFormat);
    }

    const QString content = contentStart > 0 ? text.mid(contentStart) : text;
    if (content.isEmpty())
        return;
    if (setextPattern().match(content).hasMatch()) {
        const QTextBlock previous = currentBlock().previous();
        if (previous.isValid() && !previous.text().trimmed().isEmpty()) {
            applySpan(contentStart, content.length(),
                      headingFormat(content.trimmed().startsWith(QLatin1Char('=')) ? 1 : 2));
            return;
        }
    }

    if (rulePattern().match(content).hasMatch()) {
        applySpan(contentStart, content.length(), m_ruleFormat);
        return;
    }

    const QRegularExpressionMatch headingMatch = headingPattern().match(content);
    if (headingMatch.hasMatch()) {
        const int level = int(headingMatch.capturedLength(1));
        applySpan(contentStart, content.length(), headingFormat(level));
        const QRegularExpressionMatch idMatch = headingIdPattern().match(content);
        if (idMatch.hasMatch()) {
            applySpan(contentStart + idMatch.capturedStart(), idMatch.capturedLength(),
                      m_delimiterFormat);
            mask(contentStart + idMatch.capturedStart(), idMatch.capturedLength());
        }

        applySpan(contentStart + headingMatch.capturedStart(1),
                  headingMatch.capturedLength(1), m_delimiterFormat);
        if (headingMatch.capturedStart(3) >= 0) {
            applySpan(contentStart + headingMatch.capturedStart(3),
                      headingMatch.capturedLength(3), m_delimiterFormat);
        }
        highlightInline(text, contentStart);
        return;
    }

    const QRegularExpressionMatch footnoteDef = footnoteDefPattern().match(content);
    if (footnoteDef.hasMatch()) {
        applySpan(contentStart + footnoteDef.capturedStart(1), footnoteDef.capturedLength(1),
                  m_footnoteFormat);
        mask(contentStart + footnoteDef.capturedStart(1), footnoteDef.capturedLength(1));
        highlightInline(text, contentStart);
        return;
    }

    const QRegularExpressionMatch definition = definitionPattern().match(content);
    if (definition.hasMatch()) {
        applySpan(contentStart + definition.capturedStart(1), definition.capturedLength(1),
                  m_markerFormat);
    }

    const QRegularExpressionMatch refDef = referenceDefPattern().match(content);
    if (refDef.hasMatch()) {
        applySpan(contentStart + refDef.capturedStart(1), refDef.capturedLength(1), m_linkTextFormat);
        applySpan(contentStart + refDef.capturedStart(2), refDef.capturedLength(2), m_urlFormat);
        return;
    }

    const QRegularExpressionMatch listMatch = listPattern().match(content);
    if (listMatch.hasMatch()) {
        applySpan(contentStart + listMatch.capturedStart(2), listMatch.capturedLength(2), m_markerFormat);
        if (listMatch.capturedStart(4) >= 0) {
            applySpan(contentStart + listMatch.capturedStart(4),
                      listMatch.capturedLength(4), m_markerFormat);
        }
    }

    if (content.trimmed().startsWith(QLatin1Char('|'))) {
        for (int i = 0; i < text.length(); ++i) {
            if (text.at(i) == QLatin1Char('|') && !isEscaped(text, i))
                applySpan(i, 1, m_delimiterFormat);
        }
    }

    highlightInline(text, contentStart);
}

void MarkdownHighlighter::highlightInline(const QString &text, int from)
{
    for (auto it = codeSpanPattern().globalMatch(text, from); it.hasNext();) {
        const QRegularExpressionMatch match = it.next();
        if (isEscaped(text, match.capturedStart()))
            continue;
        applySpan(match.capturedStart(), match.capturedLength(), m_codeFormat);
        applySpan(match.capturedStart(1), match.capturedLength(1), m_delimiterFormat);
        applySpan(match.capturedEnd(2), match.capturedLength(1), m_delimiterFormat);
        mask(match.capturedStart(), match.capturedLength());
    }

    const auto applyLink = [this, &text](const QRegularExpressionMatch &match, bool isImage) {
        const int start = match.capturedStart();
        if (isMasked(start, match.capturedLength()) || isEscaped(text, start))
            return;
        applySpan(start, match.capturedLength(), m_delimiterFormat);
        applySpan(match.capturedStart(1), match.capturedLength(1),
                  isImage ? m_markerFormat : m_linkTextFormat);
        applySpan(match.capturedStart(2), match.capturedLength(2), m_urlFormat);
        mask(start, match.capturedLength());
    };

    for (auto it = imagePattern().globalMatch(text, from); it.hasNext();)
        applyLink(it.next(), true);
    for (auto it = linkPattern().globalMatch(text, from); it.hasNext();)
        applyLink(it.next(), false);
    for (auto it = referenceLinkPattern().globalMatch(text, from); it.hasNext();)
        applyLink(it.next(), false);

    for (auto it = autoLinkPattern().globalMatch(text, from); it.hasNext();) {
        const QRegularExpressionMatch match = it.next();
        if (isMasked(match.capturedStart(), match.capturedLength()))
            continue;
        applySpan(match.capturedStart(), match.capturedLength(), m_linkTextFormat);
        mask(match.capturedStart(), match.capturedLength());
    }

    for (auto it = strongPattern().globalMatch(text, from); it.hasNext();) {
        const QRegularExpressionMatch match = it.next();
        if (isMasked(match.capturedStart(), match.capturedLength())
            || isEscaped(text, match.capturedStart())) {
            continue;
        }
        applySpan(match.capturedStart(), match.capturedLength(), m_boldFormat);
        applySpan(match.capturedStart(1), 2, m_delimiterFormat);
        applySpan(match.capturedEnd(2), 2, m_delimiterFormat);
        mask(match.capturedStart(), 2);
        mask(match.capturedEnd(2), 2);
    }

    const auto applyEmphasis = [this, &text](const QRegularExpressionMatch &match) {
        const int start = match.capturedStart();
        if (isMasked(start, match.capturedLength()) || isEscaped(text, start))
            return;
        applySpan(start, match.capturedLength(), m_italicFormat);
        applySpan(start, 1, m_delimiterFormat);
        applySpan(match.capturedEnd() - 1, 1, m_delimiterFormat);
    };

    for (auto it = emphasisAsteriskPattern().globalMatch(text, from); it.hasNext();)
        applyEmphasis(it.next());
    for (auto it = emphasisUnderscorePattern().globalMatch(text, from); it.hasNext();)
        applyEmphasis(it.next());

    for (auto it = strikePattern().globalMatch(text, from); it.hasNext();) {
        const QRegularExpressionMatch match = it.next();
        if (isMasked(match.capturedStart(), match.capturedLength())
            || isEscaped(text, match.capturedStart())) {
            continue;
        }
        applySpan(match.capturedStart(), match.capturedLength(), m_strikeFormat);
        applySpan(match.capturedStart(), 2, m_delimiterFormat);
        applySpan(match.capturedEnd() - 2, 2, m_delimiterFormat);
        mask(match.capturedStart(), match.capturedLength());
    }

    const auto applyWrapped = [this, &text](const QRegularExpressionMatch &match,
                                            const QTextCharFormat &body, int delimiter) {
        const int start = match.capturedStart();
        if (isMasked(start, match.capturedLength()) || isEscaped(text, start))
            return;
        applySpan(start, match.capturedLength(), body);
        applySpan(start, delimiter, m_delimiterFormat);
        applySpan(match.capturedEnd() - delimiter, delimiter, m_delimiterFormat);
        mask(start, match.capturedLength());
    };

    for (auto it = highlightPattern().globalMatch(text, from); it.hasNext();)
        applyWrapped(it.next(), m_highlightFormat, 2);

    for (auto it = footnoteRefPattern().globalMatch(text, from); it.hasNext();)
        applyWrapped(it.next(), m_footnoteFormat, 1);

    for (auto it = subscriptPattern().globalMatch(text, from); it.hasNext();)
        applyWrapped(it.next(), m_subscriptFormat, 1);

    for (auto it = superscriptPattern().globalMatch(text, from); it.hasNext();)
        applyWrapped(it.next(), m_superscriptFormat, 1);

    for (auto it = emojiPattern().globalMatch(text, from); it.hasNext();) {
        const QRegularExpressionMatch match = it.next();
        if (isMasked(match.capturedStart(), match.capturedLength()))
            continue;
        applySpan(match.capturedStart(), match.capturedLength(), m_markerFormat);
        mask(match.capturedStart(), match.capturedLength());
    }
}
