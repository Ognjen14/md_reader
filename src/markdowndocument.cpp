#include "markdowndocument.h"

#include <QAbstractTextDocumentLayout>
#include <QClipboard>
#include <QGuiApplication>
#include <QHash>
#include <QQuickItem>
#include <QQuickTextDocument>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFragment>
#include <QTextFrame>
#include <QTextList>
#include <QTextTable>

#include <algorithm>
#include <functional>

namespace {

const QHash<QString, QString> &emojiTable()
{
    static const QHash<QString, QString> table = {
        {QStringLiteral("joy"), QStringLiteral("\U0001F602")},
        {QStringLiteral("smile"), QStringLiteral("\U0001F604")},
        {QStringLiteral("grin"), QStringLiteral("\U0001F601")},
        {QStringLiteral("wink"), QStringLiteral("\U0001F609")},
        {QStringLiteral("thinking"), QStringLiteral("\U0001F914")},
        {QStringLiteral("cry"), QStringLiteral("\U0001F622")},
        {QStringLiteral("sob"), QStringLiteral("\U0001F62D")},
        {QStringLiteral("heart"), QStringLiteral("❤️")},
        {QStringLiteral("thumbsup"), QStringLiteral("\U0001F44D")},
        {QStringLiteral("+1"), QStringLiteral("\U0001F44D")},
        {QStringLiteral("thumbsdown"), QStringLiteral("\U0001F44E")},
        {QStringLiteral("-1"), QStringLiteral("\U0001F44E")},
        {QStringLiteral("ok_hand"), QStringLiteral("\U0001F44C")},
        {QStringLiteral("clap"), QStringLiteral("\U0001F44F")},
        {QStringLiteral("pray"), QStringLiteral("\U0001F64F")},
        {QStringLiteral("muscle"), QStringLiteral("\U0001F4AA")},
        {QStringLiteral("fire"), QStringLiteral("\U0001F525")},
        {QStringLiteral("star"), QStringLiteral("⭐")},
        {QStringLiteral("sparkles"), QStringLiteral("✨")},
        {QStringLiteral("zap"), QStringLiteral("⚡")},
        {QStringLiteral("boom"), QStringLiteral("\U0001F4A5")},
        {QStringLiteral("tada"), QStringLiteral("\U0001F389")},
        {QStringLiteral("rocket"), QStringLiteral("\U0001F680")},
        {QStringLiteral("bug"), QStringLiteral("\U0001F41B")},
        {QStringLiteral("wrench"), QStringLiteral("\U0001F527")},
        {QStringLiteral("hammer"), QStringLiteral("\U0001F528")},
        {QStringLiteral("gear"), QStringLiteral("⚙️")},
        {QStringLiteral("lock"), QStringLiteral("\U0001F512")},
        {QStringLiteral("key"), QStringLiteral("\U0001F511")},
        {QStringLiteral("bulb"), QStringLiteral("\U0001F4A1")},
        {QStringLiteral("memo"), QStringLiteral("\U0001F4DD")},
        {QStringLiteral("book"), QStringLiteral("\U0001F4D6")},
        {QStringLiteral("books"), QStringLiteral("\U0001F4DA")},
        {QStringLiteral("pushpin"), QStringLiteral("\U0001F4CC")},
        {QStringLiteral("package"), QStringLiteral("\U0001F4E6")},
        {QStringLiteral("mag"), QStringLiteral("\U0001F50D")},
        {QStringLiteral("chart"), QStringLiteral("\U0001F4C8")},
        {QStringLiteral("calendar"), QStringLiteral("\U0001F4C5")},
        {QStringLiteral("alarm_clock"), QStringLiteral("⏰")},
        {QStringLiteral("hourglass"), QStringLiteral("⌛")},
        {QStringLiteral("warning"), QStringLiteral("⚠️")},
        {QStringLiteral("no_entry"), QStringLiteral("⛔")},
        {QStringLiteral("x"), QStringLiteral("❌")},
        {QStringLiteral("heavy_check_mark"), QStringLiteral("✔️")},
        {QStringLiteral("white_check_mark"), QStringLiteral("✅")},
        {QStringLiteral("question"), QStringLiteral("❓")},
        {QStringLiteral("exclamation"), QStringLiteral("❗")},
        {QStringLiteral("information_source"), QStringLiteral("ℹ️")},
        {QStringLiteral("point_right"), QStringLiteral("\U0001F449")},
        {QStringLiteral("arrow_right"), QStringLiteral("➡️")},
        {QStringLiteral("recycle"), QStringLiteral("♻️")},
        {QStringLiteral("construction"), QStringLiteral("\U0001F6A7")},
        {QStringLiteral("checkered_flag"), QStringLiteral("\U0001F3C1")},
        {QStringLiteral("trophy"), QStringLiteral("\U0001F3C6")},
        {QStringLiteral("coffee"), QStringLiteral("☕")},
        {QStringLiteral("beer"), QStringLiteral("\U0001F37A")},
        {QStringLiteral("computer"), QStringLiteral("\U0001F4BB")},
        {QStringLiteral("iphone"), QStringLiteral("\U0001F4F1")},
        {QStringLiteral("bell"), QStringLiteral("\U0001F514")},
        {QStringLiteral("eyes"), QStringLiteral("\U0001F440")},
        {QStringLiteral("ghost"), QStringLiteral("\U0001F47B")},
        {QStringLiteral("skull"), QStringLiteral("\U0001F480")},
        {QStringLiteral("snake"), QStringLiteral("\U0001F40D")},
        {QStringLiteral("penguin"), QStringLiteral("\U0001F427")},
        {QStringLiteral("cat"), QStringLiteral("\U0001F431")},
        {QStringLiteral("dog"), QStringLiteral("\U0001F436")},
    };
    return table;
}

const QHash<char16_t, char16_t> &subscriptTable()
{
    static const QHash<char16_t, char16_t> table = {
        {u'0', 0x2080}, {u'1', 0x2081}, {u'2', 0x2082}, {u'3', 0x2083}, {u'4', 0x2084},
        {u'5', 0x2085}, {u'6', 0x2086}, {u'7', 0x2087}, {u'8', 0x2088}, {u'9', 0x2089},
        {u'+', 0x208A}, {u'-', 0x208B}, {u'=', 0x208C}, {u'(', 0x208D}, {u')', 0x208E},
        {u'a', 0x2090}, {u'e', 0x2091}, {u'o', 0x2092}, {u'x', 0x2093}, {u'h', 0x2095},
        {u'k', 0x2096}, {u'l', 0x2097}, {u'm', 0x2098}, {u'n', 0x2099}, {u'p', 0x209A},
        {u's', 0x209B}, {u't', 0x209C}, {u'i', 0x1D62}, {u'r', 0x1D63}, {u'u', 0x1D64},
        {u'v', 0x1D65}, {u'j', 0x2C7C},
    };
    return table;
}

const QHash<char16_t, char16_t> &superscriptTable()
{
    static const QHash<char16_t, char16_t> table = {
        {u'0', 0x2070}, {u'1', 0x00B9}, {u'2', 0x00B2}, {u'3', 0x00B3}, {u'4', 0x2074},
        {u'5', 0x2075}, {u'6', 0x2076}, {u'7', 0x2077}, {u'8', 0x2078}, {u'9', 0x2079},
        {u'+', 0x207A}, {u'-', 0x207B}, {u'=', 0x207C}, {u'(', 0x207D}, {u')', 0x207E},
        {u'a', 0x1D43}, {u'b', 0x1D47}, {u'c', 0x1D9C}, {u'd', 0x1D48}, {u'e', 0x1D49},
        {u'f', 0x1DA0}, {u'g', 0x1D4D}, {u'h', 0x02B0}, {u'i', 0x2071}, {u'j', 0x02B2},
        {u'k', 0x1D4F}, {u'l', 0x02E1}, {u'm', 0x1D50}, {u'n', 0x207F}, {u'o', 0x1D52},
        {u'p', 0x1D56}, {u'r', 0x02B3}, {u's', 0x02E2}, {u't', 0x1D57}, {u'u', 0x1D58},
        {u'v', 0x1D5B}, {u'w', 0x02B7}, {u'x', 0x02E3}, {u'y', 0x02B8}, {u'z', 0x1DBB},
    };
    return table;
}

QString translateRun(const QString &run, const QHash<char16_t, char16_t> &table)
{
    QString out;
    out.reserve(run.size());
    for (const QChar ch : run) {
        const auto it = table.constFind(ch.toLower().unicode());
        if (it == table.constEnd())
            return {};
        out.append(QChar(*it));
    }
    return out;
}

QString headingSlug(const QString &title)
{
    QString slug;
    slug.reserve(title.size());
    for (const QChar ch : title) {
        if (ch.isLetterOrNumber() || ch == u'-' || ch == u'_')
            slug.append(ch.toLower());
        else if (ch.isSpace())
            slug.append(u'-');
    }
    return slug;
}

constexpr qreal CodeHeaderHeight = 32.0;
constexpr qreal CodeFootPadding = 12.0;

}

MarkdownDocument::MarkdownDocument(QObject *parent)
    : QObject(parent)
{
}

void MarkdownDocument::setDocument(QQuickTextDocument *document)
{
    if (m_document == document)
        return;

    QObject::disconnect(m_contentsConnection);
    QObject::disconnect(m_layoutConnection);
    m_document = document;

    if (m_document) {
        if (QTextDocument *doc = m_document->textDocument()) {
            m_contentsConnection = connect(doc, &QTextDocument::contentsChanged,
                                           this, &MarkdownDocument::invalidateHeadingCache);
            if (QAbstractTextDocumentLayout *layout = doc->documentLayout()) {
                m_layoutConnection = connect(layout, &QAbstractTextDocumentLayout::documentSizeChanged,
                                             this, [this]() {
                    m_codeRegionsDirty = true;
                    emit codeRegionsChanged();
                });
            }
            doc->setBaseUrl(m_baseUrl);
        }
    }

    disableViewportCulling();
    invalidateHeadingCache();
    emit documentChanged();
}

void MarkdownDocument::disableViewportCulling()
{
    if (!m_document)
        return;
    if (auto *item = qobject_cast<QQuickItem *>(m_document->parent()))
        item->setFlag(QQuickItem::ItemObservesViewport, false);
}


void MarkdownDocument::setBaseUrl(const QUrl &url)
{
    if (m_baseUrl == url)
        return;
    m_baseUrl = url;

    if (m_document) {
        if (QTextDocument *doc = m_document->textDocument())
            doc->setBaseUrl(m_baseUrl);
    }

    emit baseUrlChanged();
}

void MarkdownDocument::invalidateHeadingCache()
{
    m_headingCacheDirty = true;
    m_codeRegionsDirty = true;
    emit codeRegionsChanged();
}

bool MarkdownDocument::isCodeBlock(const QTextBlock &block)
{
    if (!block.isValid())
        return false;
    const QTextBlockFormat format = block.blockFormat();
    return format.hasProperty(QTextFormat::BlockCodeFence)
           || format.hasProperty(QTextFormat::BlockCodeLanguage)
           || format.nonBreakableLines();
}

void MarkdownDocument::rebuildCodeRegions() const
{
    m_codeRegions.clear();
    m_codeRegionsDirty = false;

    if (!m_document)
        return;
    QTextDocument *doc = m_document->textDocument();
    if (!doc)
        return;
    QAbstractTextDocumentLayout *layout = doc->documentLayout();
    if (!layout)
        return;

    for (QTextBlock block = doc->begin(); block.isValid();) {
        if (!isCodeBlock(block)) {
            block = block.next();
            continue;
        }

        if (doc->frameAt(block.position()) != doc->rootFrame()) {
            block = block.next();
            continue;
        }
        const qreal textTop = layout->blockBoundingRect(block).top();
        const qreal top = textTop - CodeHeaderHeight;
        QString language = block.blockFormat().stringProperty(QTextFormat::BlockCodeLanguage);
        QStringList lines;
        qreal bottom = textTop;

        QTextBlock last = block;
        for (; last.isValid() && isCodeBlock(last); last = last.next()) {
            lines.append(last.text());
            bottom = layout->blockBoundingRect(last).bottom() + CodeFootPadding;
            if (language.isEmpty()) {
                language = last.blockFormat().stringProperty(QTextFormat::BlockCodeLanguage);
            }
        }
        while (!lines.isEmpty() && lines.constLast().trimmed().isEmpty())
            lines.removeLast();

        if (!lines.isEmpty()) {
            m_codeRegions.append(QVariantMap {
                {QStringLiteral("top"), top},
                {QStringLiteral("height"), bottom - top},
                {QStringLiteral("headerHeight"), CodeHeaderHeight},
                {QStringLiteral("language"), language},
                {QStringLiteral("text"), lines.join(QLatin1Char('\n'))},
            });
        }

        block = last;
    }
}

QVariantList MarkdownDocument::codeRegions() const
{
    if (m_codeRegionsDirty)
        rebuildCodeRegions();
    return m_codeRegions;
}

qreal MarkdownDocument::anchorTop(const QString &anchor) const
{
    if (!m_document)
        return -1;
    QTextDocument *doc = m_document->textDocument();
    if (!doc)
        return -1;
    QAbstractTextDocumentLayout *layout = doc->documentLayout();
    if (!layout)
        return -1;

    QString wanted = anchor;
    if (wanted.startsWith(u'#'))
        wanted.remove(0, 1);
    wanted = QUrl::fromPercentEncoding(wanted.toUtf8()).toLower();
    if (wanted.isEmpty())
        return -1;

    const auto custom = m_customAnchors.constFind(wanted);
    if (custom != m_customAnchors.constEnd()) {
        const QTextBlock block = doc->findBlockByNumber(custom.value());
        if (block.isValid())
            return layout->blockBoundingRect(block).top();
    }

    QHash<QString, int> seen;
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        if (block.blockFormat().headingLevel() <= 0)
            continue;

        QString slug = headingSlug(block.text());
        if (slug.isEmpty())
            continue;
        const int previous = seen[slug]++;
        if (previous > 0)
            slug += QLatin1Char('-') + QString::number(previous);

        if (slug == wanted)
            return layout->blockBoundingRect(block).top();
    }

    return -1;
}

void MarkdownDocument::copyToClipboard(const QString &text) const
{
    if (QClipboard *clipboard = QGuiApplication::clipboard())
        clipboard->setText(text);
}

void MarkdownDocument::setLineHeight(qreal percent)
{
    percent = qBound(100.0, percent, 300.0);
    if (qFuzzyCompare(m_lineHeight, percent))
        return;
    m_lineHeight = percent;
    emit styleChanged();
}

#define MD_DOC_COLOR_SETTER(Setter, Member)             \
    void MarkdownDocument::Setter(const QColor &color)  \
    {                                                   \
        if (Member == color)                            \
            return;                                     \
        Member = color;                                 \
        emit styleChanged();                            \
    }

MD_DOC_COLOR_SETTER(setCodeBackground, m_codeBackground)
MD_DOC_COLOR_SETTER(setHeadingColor, m_headingColor)
MD_DOC_COLOR_SETTER(setCodeColor, m_codeColor)
MD_DOC_COLOR_SETTER(setInlineCodeColor, m_inlineCodeColor)
MD_DOC_COLOR_SETTER(setInlineCodeBackground, m_inlineCodeBackground)
MD_DOC_COLOR_SETTER(setLinkColor, m_linkColor)
MD_DOC_COLOR_SETTER(setQuoteColor, m_quoteColor)
MD_DOC_COLOR_SETTER(setBoldColor, m_boldColor)
MD_DOC_COLOR_SETTER(setItalicColor, m_italicColor)
MD_DOC_COLOR_SETTER(setMarkerColor, m_markerColor)
MD_DOC_COLOR_SETTER(setRuleColor, m_ruleColor)
MD_DOC_COLOR_SETTER(setHighlightBackground, m_highlightBackground)
MD_DOC_COLOR_SETTER(setCodeKeywordColor, m_codePalette.keyword)
MD_DOC_COLOR_SETTER(setCodeTypeColor, m_codePalette.type)
MD_DOC_COLOR_SETTER(setCodeStringColor, m_codePalette.string)
MD_DOC_COLOR_SETTER(setCodeNumberColor, m_codePalette.number)
MD_DOC_COLOR_SETTER(setCodeCommentColor, m_codePalette.comment)
MD_DOC_COLOR_SETTER(setCodeFunctionColor, m_codePalette.function)
MD_DOC_COLOR_SETTER(setCodeVariableColor, m_codePalette.variable)
MD_DOC_COLOR_SETTER(setCheckedColor, m_checkedColor)
MD_DOC_COLOR_SETTER(setUncheckedColor, m_uncheckedColor)
MD_DOC_COLOR_SETTER(setTableBorderColor, m_tableBorderColor)
MD_DOC_COLOR_SETTER(setTableHeaderBackground, m_tableHeaderBackground)
MD_DOC_COLOR_SETTER(setTableStripeBackground, m_tableStripeBackground)

#undef MD_DOC_COLOR_SETTER

#define MD_DOC_STRING_SETTER(Setter, Member)              \
    void MarkdownDocument::Setter(const QString &value)   \
    {                                                     \
        if (Member == value)                              \
            return;                                       \
        Member = value;                                   \
        emit styleChanged();                              \
    }

MD_DOC_STRING_SETTER(setCodeFont, m_codeFont)
MD_DOC_STRING_SETTER(setCheckboxFont, m_checkboxFont)
MD_DOC_STRING_SETTER(setCheckedGlyph, m_checkedGlyph)
MD_DOC_STRING_SETTER(setUncheckedGlyph, m_uncheckedGlyph)

#undef MD_DOC_STRING_SETTER

void MarkdownDocument::restyle()
{
    if (!m_document || m_applying)
        return;
    QTextDocument *doc = m_document->textDocument();
    if (!doc)
        return;

    m_applying = true;
    applyStyle(doc);
    m_applying = false;

    disableViewportCulling();
}

void MarkdownDocument::applyStyle(QTextDocument *doc)
{
    if (!doc)
        return;

    QTextCursor outer(doc);
    outer.beginEditBlock();

    applyExtendedSyntax(doc);
    applyTaskListStyle(doc);

    struct PendingFormat {
        int position;
        int length;
        QTextCharFormat format;
    };
    QList<PendingFormat> pending;

    QTextCursor cursor(doc);
    cursor.beginEditBlock();
    CodeSyntax::Language codeLanguage = CodeSyntax::Language::Plain;
    int codeSyntaxState = 0;
    bool previousWasCode = false;
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        QTextBlockFormat format = block.blockFormat();
        const int heading = format.headingLevel();
        const bool isCode = isCodeBlock(block);
        const bool isQuote = format.intProperty(QTextFormat::BlockQuoteLevel) > 0;
        const bool isRule = format.hasProperty(QTextFormat::BlockTrailingHorizontalRulerWidth);
        bool hasImage = false;

        if (isRule && m_ruleColor.isValid())
            format.setBackground(m_ruleColor);

        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            const QTextFragment fragment = it.fragment();
            if (!fragment.isValid())
                continue;

            const QTextCharFormat existing = fragment.charFormat();
            QTextCharFormat overlay;

            if (existing.isImageFormat())
                hasImage = true;

            const bool isMono = isCode || existing.hasProperty(QTextFormat::FontFixedPitch);
            const bool isBold = existing.fontWeight() > QFont::Normal;

            if (isMono) {
                const QColor foreground = heading > 0 ? m_headingColor
                                        : existing.isAnchor() ? m_linkColor
                                        : isCode ? m_codeColor
                                                 : m_inlineCodeColor;
                if (foreground.isValid())
                    overlay.setForeground(foreground);

                if (!isCode && m_inlineCodeBackground.isValid()
                    && m_inlineCodeBackground.alpha() > 0) {
                    overlay.setBackground(m_inlineCodeBackground);
                }
                if (!m_codeFont.isEmpty())
                    overlay.setFontFamilies({m_codeFont});

                if (existing.isAnchor())
                    overlay.setFontUnderline(true);

                if (heading > 0) {
                    const QTextCharFormat headingFormat = block.charFormat();

                    if (headingFormat.hasProperty(QTextFormat::FontPointSize)) {
                        overlay.setFontPointSize(headingFormat.fontPointSize());
                    }
                    if (headingFormat.hasProperty(QTextFormat::FontSizeAdjustment)) {
                        overlay.setProperty(QTextFormat::FontSizeAdjustment,
                                            headingFormat.property(QTextFormat::FontSizeAdjustment));
                    } else if (!headingFormat.hasProperty(QTextFormat::FontPointSize)) {
                        overlay.setProperty(QTextFormat::FontSizeAdjustment,
                                            4 - qBound(1, heading, 6));
                    }

                    if (headingFormat.fontWeight() > QFont::Normal)
                        overlay.setFontWeight(headingFormat.fontWeight());
                }
            } else if (heading > 0 && m_headingColor.isValid()) {
                overlay.setForeground(m_headingColor);
            } else if (existing.isAnchor() && m_linkColor.isValid()) {
                overlay.setForeground(m_linkColor);
                overlay.setFontUnderline(true);
            } else if (isQuote && m_quoteColor.isValid()) {
                overlay.setForeground(m_quoteColor);
            } else if (isBold && m_boldColor.isValid()) {
                overlay.setForeground(m_boldColor);
            } else if (existing.fontItalic() && m_italicColor.isValid()) {
                overlay.setForeground(m_italicColor);
            } else if (existing.fontStrikeOut() && m_quoteColor.isValid()) {
                overlay.setForeground(m_quoteColor);
            } else {
                continue;
            }

            if (overlay.properties().isEmpty())
                continue;

            pending.append({fragment.position(), fragment.length(), overlay});
        }

        if (isCode) {
            const CodeSyntax::Language language = CodeSyntax::languageFromInfo(
                QStringView(format.stringProperty(QTextFormat::BlockCodeLanguage)));
            if (!previousWasCode || language != codeLanguage)
                codeSyntaxState = 0;
            codeLanguage = language;
            const CodeSyntax::LineResult syntax = CodeSyntax::highlightLine(
                QStringView(block.text()), codeLanguage, codeSyntaxState);
            codeSyntaxState = syntax.state;
            for (const CodeSyntax::Span &span : syntax.spans) {
                const QColor color = m_codePalette.color(span.role);
                if (!color.isValid())
                    continue;
                QTextCharFormat overlay;
                overlay.setForeground(color);
                pending.append({block.position() + span.start, span.length, overlay});
            }
        } else {
            codeLanguage = CodeSyntax::Language::Plain;
            codeSyntaxState = 0;
        }
        previousWasCode = isCode;

        if (doc->frameAt(block.position()) != doc->rootFrame()) {
            format.setLineHeight(125.0, QTextBlockFormat::ProportionalHeight);
            format.setTopMargin(0);
            format.setBottomMargin(0);
            cursor.setPosition(block.position());
            cursor.setBlockFormat(format);
            continue;
        }

        if (heading > 0) {
            format.setLineHeight(120.0, QTextBlockFormat::ProportionalHeight);
            format.setTopMargin(heading == 1 ? 10 : 26);
            format.setBottomMargin(heading <= 2 ? 10 : 6);
        } else if (isCode) {
            format.setLineHeight(130.0, QTextBlockFormat::ProportionalHeight);
            format.setLeftMargin(14);
            format.setRightMargin(14);
            format.clearBackground();
            format.setNonBreakableLines(false);

            const bool opensRun = !isCodeBlock(block.previous());
            const bool closesRun = !isCodeBlock(block.next());
            format.setTopMargin(opensRun ? CodeHeaderHeight : 0);
            format.setBottomMargin(closesRun ? CodeFootPadding : 0);
        } else {
            format.setLineHeight(m_lineHeight, QTextBlockFormat::ProportionalHeight);
            format.setTopMargin(0);
            format.setBottomMargin(12);
        }

        if (hasImage)
            format.setLineHeight(100.0, QTextBlockFormat::ProportionalHeight);

        cursor.setPosition(block.position());
        cursor.setBlockFormat(format);
    }

    for (const PendingFormat &edit : std::as_const(pending)) {
        cursor.setPosition(edit.position);
        cursor.setPosition(edit.position + edit.length, QTextCursor::KeepAnchor);
        cursor.mergeCharFormat(edit.format);
    }

    cursor.endEditBlock();

    applyTableStyle(doc);

    outer.endEditBlock();

    doc->markContentsDirty(0, doc->characterCount());
}

QPoint MarkdownDocument::lineColumnAt(int position) const
{
    if (!m_document)
        return QPoint(1, 1);
    QTextDocument *doc = m_document->textDocument();
    if (!doc)
        return QPoint(1, 1);

    position = qBound(0, position, doc->characterCount() - 1);
    const QTextBlock block = doc->findBlock(position);
    if (!block.isValid())
        return QPoint(1, 1);

    return QPoint(block.blockNumber() + 1, position - block.position() + 1);
}

QString MarkdownDocument::preprocess(const QString &markdown) const
{
    static const QRegularExpression subscript(QStringLiteral("(?<!~)~(?!~)([^~\\s]+)~(?!~)"));
    static const QRegularExpression superscript(QStringLiteral("\\^([^\\^\\s]+)\\^"));
    static const QRegularExpression fencePattern(QStringLiteral("^ {0,3}(`{3,}|~{3,})"));

    static const QRegularExpression uncFileUrl(
        QStringLiteral("\\bfile://(?![/\\\\])"), QRegularExpression::CaseInsensitiveOption);

    const auto rewriteProse = [&](QString text) {
        text.replace(uncFileUrl, QStringLiteral("file:///"));

        for (const auto &pass : {std::pair{&subscript, &subscriptTable()},
                                 std::pair{&superscript, &superscriptTable()}}) {
            QString out;
            out.reserve(text.size());
            int copiedTo = 0;
            for (auto it = pass.first->globalMatch(text); it.hasNext();) {
                const QRegularExpressionMatch match = it.next();
                const QString translated = translateRun(match.captured(1), *pass.second);
                if (translated.isEmpty())
                    continue;
                out.append(QStringView{text}.mid(copiedTo,
                                                  match.capturedStart() - copiedTo));
                out.append(translated);
                copiedTo = int(match.capturedEnd());
            }
            out.append(QStringView{text}.mid(copiedTo));
            text = out;
        }
        return text;
    };

    const auto isEscaped = [](const QString &text, int position) {
        int backslashes = 0;
        for (int i = position - 1; i >= 0 && text.at(i) == QLatin1Char('\\'); --i)
            ++backslashes;
        return (backslashes % 2) == 1;
    };

    const auto rewriteOutsideCodeSpans = [&](const QString &line) {
        QString out;
        out.reserve(line.size());
        int copiedTo = 0;
        int searchFrom = 0;

        while (searchFrom < line.size()) {
            int open = line.indexOf(QLatin1Char('`'), searchFrom);
            while (open >= 0 && isEscaped(line, open))
                open = line.indexOf(QLatin1Char('`'), open + 1);
            if (open < 0)
                break;

            int openLength = 1;
            while (open + openLength < line.size()
                   && line.at(open + openLength) == QLatin1Char('`')) {
                ++openLength;
            }

            int close = open + openLength;
            int closeLength = 0;
            while (close < line.size()) {
                close = line.indexOf(QLatin1Char('`'), close);
                if (close < 0)
                    break;
                closeLength = 1;
                while (close + closeLength < line.size()
                       && line.at(close + closeLength) == QLatin1Char('`')) {
                    ++closeLength;
                }
                if (closeLength == openLength)
                    break;
                close += closeLength;
            }

            if (close < 0) {
                searchFrom = open + openLength;
                continue;
            }

            out.append(rewriteProse(line.mid(copiedTo, open - copiedTo)));
            const int literalEnd = close + closeLength;
            out.append(QStringView{line}.mid(open, literalEnd - open));
            copiedTo = literalEnd;
            searchFrom = literalEnd;
        }

        out.append(rewriteProse(line.mid(copiedTo)));
        return out;
    };

    QString result;
    result.reserve(markdown.size());
    QString prose;
    prose.reserve(markdown.size());
    QChar openFenceKind;
    int openFenceLength = 0;
    int lineStart = 0;

    const auto flushProse = [&]() {
        if (prose.isEmpty())
            return;
        result.append(rewriteOutsideCodeSpans(prose));
        prose.clear();
    };

    while (lineStart < markdown.size()) {
        const int newline = markdown.indexOf(QLatin1Char('\n'), lineStart);
        const int lineEnd = newline < 0 ? int(markdown.size()) : newline;
        const QString line = markdown.mid(lineStart, lineEnd - lineStart);
        const QRegularExpressionMatch fence = fencePattern.match(line);

        if (!openFenceKind.isNull()) {
            flushProse();
            if (fence.hasMatch()) {
                const QString run = fence.captured(1);
                const bool blankTail = line.mid(fence.capturedEnd(1)).trimmed().isEmpty();
                if (run.at(0) == openFenceKind && run.size() >= openFenceLength && blankTail) {
                    openFenceKind = QChar();
                    openFenceLength = 0;
                }
            }
            result.append(line);
        } else if (fence.hasMatch()) {
            flushProse();
            const QString run = fence.captured(1);
            openFenceKind = run.at(0);
            openFenceLength = int(run.size());
            result.append(line);
        } else {
            prose.append(line);
        }

        if (newline < 0) {
            break;
        }
        if (openFenceKind.isNull() && !fence.hasMatch())
            prose.append(QLatin1Char('\n'));
        else
            result.append(QLatin1Char('\n'));
        lineStart = newline + 1;
    }

    flushProse();
    return result;
}

void MarkdownDocument::applyExtendedSyntax(QTextDocument *doc)
{
    static const QRegularExpression highlight(QStringLiteral("==(?=\\S)(.+?)(?<=\\S)=="));
    static const QRegularExpression footnote(QStringLiteral("\\[\\^([^\\]\\s]+)\\]"));
    static const QRegularExpression emoji(QStringLiteral(":([a-z0-9_+-]{2,}):"));
    static const QRegularExpression headingId(QStringLiteral("\\s*\\{#([A-Za-z0-9_-]+)\\}\\s*$"));

    m_customAnchors.clear();

    struct Edit {
        int start;
        int length;
        QString replacement;
        QTextCharFormat format;
    };
    QList<Edit> edits;

    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        const QTextBlockFormat blockFormat = block.blockFormat();
        if (blockFormat.hasProperty(QTextFormat::BlockCodeFence)
            || blockFormat.hasProperty(QTextFormat::BlockCodeLanguage)
            || blockFormat.nonBreakableLines()) {
            continue;
        }

        const QString text = block.text();
        if (text.isEmpty())
            continue;
        const int base = block.position();

        QList<QPair<int, int>> codeRuns;
        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            const QTextFragment fragment = it.fragment();
            if (fragment.isValid() && fragment.charFormat().hasProperty(QTextFormat::FontFixedPitch)) {
                codeRuns.append({int(fragment.position() - base),
                                 int(fragment.position() + fragment.length() - base)});
            }
        }

        const auto insideCode = [&codeRuns](int offset) {
            for (const auto &run : std::as_const(codeRuns)) {
                if (offset >= run.first && offset < run.second)
                    return true;
            }
            return false;
        };

        const auto collect = [&](const QRegularExpression &pattern,
                                 const QTextCharFormat &format,
                                 bool useCapture) {
            for (auto it = pattern.globalMatch(text); it.hasNext();) {
                const QRegularExpressionMatch match = it.next();
                if (insideCode(match.capturedStart()))
                    continue;
                edits.append({int(base + match.capturedStart()), int(match.capturedLength()),
                              useCapture ? match.captured(1) : QString(), format});
            }
        };

        QTextCharFormat highlightFormat;
        if (m_highlightBackground.isValid())
            highlightFormat.setBackground(m_highlightBackground);

        QTextCharFormat footnoteFormat;
        footnoteFormat.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
        if (m_linkColor.isValid())
            footnoteFormat.setForeground(m_linkColor);

        collect(highlight, highlightFormat, true);
        collect(footnote, footnoteFormat, true);

        if (blockFormat.headingLevel() > 0) {
            const QRegularExpressionMatch match = headingId.match(text);
            if (match.hasMatch()) {
                m_customAnchors.insert(match.captured(1).toLower(), block.blockNumber());
                edits.append({base + int(match.capturedStart()), int(match.capturedLength()),
                              QString(), QTextCharFormat()});
            }
        }

        for (auto it = emoji.globalMatch(text); it.hasNext();) {
            const QRegularExpressionMatch match = it.next();
            const QString glyph = emojiTable().value(match.captured(1));
            if (glyph.isEmpty() || insideCode(match.capturedStart()))
                continue;
            edits.append({base + int(match.capturedStart()), int(match.capturedLength()),
                          glyph, QTextCharFormat()});
        }
    }

    if (edits.isEmpty())
        return;

    std::sort(edits.begin(), edits.end(),
              [](const Edit &a, const Edit &b) { return a.start < b.start; });

    QList<Edit> accepted;
    int consumedTo = -1;
    for (const Edit &edit : std::as_const(edits)) {
        if (edit.start < consumedTo)
            continue;
        accepted.append(edit);
        consumedTo = edit.start + edit.length;
    }

    QTextCursor cursor(doc);
    cursor.beginEditBlock();
    for (int i = accepted.size() - 1; i >= 0; --i) {
        const Edit &edit = accepted.at(i);
        cursor.setPosition(edit.start);
        cursor.setPosition(edit.start + edit.length, QTextCursor::KeepAnchor);
        QTextCharFormat format = cursor.charFormat();
        format.merge(edit.format);
        cursor.insertText(edit.replacement, format);
    }
    cursor.endEditBlock();
}

void MarkdownDocument::applyTaskListStyle(QTextDocument *doc)
{
    if (m_checkedGlyph.isEmpty() || m_uncheckedGlyph.isEmpty())
        return;

    struct Task {
        int position;
        int blockNumber;
        bool checked;
        int indent;
    };
    QList<Task> tasks;
    QList<int> leakedMarkerBlocks;
    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        const QTextBlockFormat format = block.blockFormat();
        const QTextList *list = block.textList();

        if (!list) {
            if (format.marker() != QTextBlockFormat::MarkerType::NoMarker)
                leakedMarkerBlocks.append(block.blockNumber());
            continue;
        }
        if (format.marker() == QTextBlockFormat::MarkerType::NoMarker)
            continue;
        tasks.append({block.position(), block.blockNumber(),
                      format.marker() == QTextBlockFormat::MarkerType::Checked,
                      list->format().indent()});
    }
    if (tasks.isEmpty() && leakedMarkerBlocks.isEmpty())
        return;

    QTextCharFormat glyphFormat;
    if (!m_checkboxFont.isEmpty())
        glyphFormat.setFontFamilies({m_checkboxFont});

    QTextCursor cursor(doc);
    cursor.beginEditBlock();

    for (int i = tasks.size() - 1; i >= 0; --i) {
        const Task &task = tasks.at(i);
        QTextCharFormat format = glyphFormat;
        const QColor color = task.checked ? m_checkedColor : m_uncheckedColor;
        if (color.isValid())
            format.setForeground(color);
        cursor.setPosition(task.position);
        cursor.insertText((task.checked ? m_checkedGlyph : m_uncheckedGlyph) + QLatin1Char(' '),
                          format);
    }

    for (const Task &task : std::as_const(tasks)) {
        const QTextBlock block = doc->findBlockByNumber(task.blockNumber);
        if (!block.isValid())
            continue;
        const QTextList *list = block.textList();
        QTextBlockFormat format = block.blockFormat();
        const int indent = list ? list->format().indent() : task.indent;
        format.setMarker(QTextBlockFormat::MarkerType::NoMarker);
        format.setObjectIndex(-1);
        format.setIndent(indent);
        cursor.setPosition(block.position());
        cursor.setBlockFormat(format);
    }


    for (int blockNumber : std::as_const(leakedMarkerBlocks)) {
        const QTextBlock block = doc->findBlockByNumber(blockNumber);
        if (!block.isValid())
            continue;
        QTextBlockFormat format = block.blockFormat();
        format.clearProperty(QTextFormat::BlockMarker);
        cursor.setPosition(block.position());
        cursor.setBlockFormat(format);
    }

    cursor.endEditBlock();
}


void MarkdownDocument::applyTableStyle(QTextDocument *doc)
{
    QList<QTextTable *> tables;
    const std::function<void(QTextFrame *)> collect = [&](QTextFrame *frame) {
        const auto children = frame->childFrames();
        for (QTextFrame *child : children) {
            if (auto *table = qobject_cast<QTextTable *>(child))
                tables.append(table);
            collect(child);
        }
    };
    collect(doc->rootFrame());

    for (QTextTable *table : std::as_const(tables)) {
        QTextTableFormat format = table->format();
        format.setHeaderRowCount(1);
        format.setCellPadding(9);
        format.setCellSpacing(0);
        format.setBorder(1);
        format.setBorderCollapse(true);
        format.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
        if (m_tableBorderColor.isValid())
            format.setBorderBrush(m_tableBorderColor);
        table->setFormat(format);

        for (int row = 0; row < table->rows(); ++row) {
            for (int column = 0; column < table->columns(); ++column) {
                QTextTableCell cell = table->cellAt(row, column);
                if (!cell.isValid())
                    continue;
                QTextTableCellFormat cellFormat = cell.format().toTableCellFormat();
                if (row == 0) {
                    if (m_tableHeaderBackground.isValid())
                        cellFormat.setBackground(m_tableHeaderBackground);
                } else if (row % 2 == 0 && m_tableStripeBackground.isValid()) {
                    cellFormat.setBackground(m_tableStripeBackground);
                } else {
                    cellFormat.clearBackground();
                }
                cell.setFormat(cellFormat);
            }
        }
    }
}


int MarkdownDocument::positionForLine(int line) const
{
    if (!m_document)
        return 0;
    QTextDocument *doc = m_document->textDocument();
    if (!doc)
        return 0;

    const QTextBlock block = doc->findBlockByNumber(qBound(0, line - 1, doc->blockCount() - 1));
    return block.isValid() ? block.position() : 0;
}

void MarkdownDocument::rebuildHeadingCache() const
{
    m_headingTops.clear();
    m_headingCacheDirty = false;

    if (!m_document)
        return;
    QTextDocument *doc = m_document->textDocument();
    if (!doc)
        return;
    QAbstractTextDocumentLayout *layout = doc->documentLayout();
    if (!layout)
        return;

    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        if (block.blockFormat().headingLevel() > 0)
            m_headingTops.append(layout->blockBoundingRect(block).top());
    }
}

qreal MarkdownDocument::headingTop(int index) const
{
    if (m_headingCacheDirty)
        rebuildHeadingCache();
    if (index < 0 || index >= m_headingTops.size())
        return -1;
    return m_headingTops.at(index);
}

int MarkdownDocument::headingIndexAt(qreal y) const
{
    if (m_headingCacheDirty)
        rebuildHeadingCache();

    int result = -1;
    for (int i = 0; i < m_headingTops.size(); ++i) {
        if (m_headingTops.at(i) > y)
            break;
        result = i;
    }
    return result;
}
