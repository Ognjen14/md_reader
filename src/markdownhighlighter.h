#pragma once

#include "codesyntax.h"

#include <QColor>
#include <QQuickTextDocument>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QtQml/qqmlregistration.h>

class MarkdownHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QQuickTextDocument *document READ quickDocument WRITE setQuickDocument NOTIFY documentChanged)

    Q_PROPERTY(qreal baseFontSize READ baseFontSize WRITE setBaseFontSize NOTIFY paletteChanged)

    Q_PROPERTY(QColor headingColor READ headingColor WRITE setHeadingColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor boldColor READ boldColor WRITE setBoldColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor italicColor READ italicColor WRITE setItalicColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor codeColor READ codeColor WRITE setCodeColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor inlineCodeColor READ inlineCodeColor WRITE setInlineCodeColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor inlineCodeBackground READ inlineCodeBackground WRITE setInlineCodeBackground NOTIFY paletteChanged)
    Q_PROPERTY(QColor codeBackground READ codeBackground WRITE setCodeBackground NOTIFY paletteChanged)
    Q_PROPERTY(QColor linkColor READ linkColor WRITE setLinkColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor markerColor READ markerColor WRITE setMarkerColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor quoteColor READ quoteColor WRITE setQuoteColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor delimiterColor READ delimiterColor WRITE setDelimiterColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor ruleColor READ ruleColor WRITE setRuleColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor highlightBackground READ highlightBackground WRITE setHighlightBackground NOTIFY paletteChanged)
    Q_PROPERTY(QColor codeKeywordColor READ codeKeywordColor WRITE setCodeKeywordColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor codeTypeColor READ codeTypeColor WRITE setCodeTypeColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor codeStringColor READ codeStringColor WRITE setCodeStringColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor codeNumberColor READ codeNumberColor WRITE setCodeNumberColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor codeCommentColor READ codeCommentColor WRITE setCodeCommentColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor codeFunctionColor READ codeFunctionColor WRITE setCodeFunctionColor NOTIFY paletteChanged)
    Q_PROPERTY(QColor codeVariableColor READ codeVariableColor WRITE setCodeVariableColor NOTIFY paletteChanged)

public:
    explicit MarkdownHighlighter(QObject *parent = nullptr);

    QQuickTextDocument *quickDocument() const { return m_quickDocument; }
    void setQuickDocument(QQuickTextDocument *document);

    qreal baseFontSize() const { return m_baseFontSize; }
    void setBaseFontSize(qreal size);

    QColor headingColor() const { return m_headingColor; }
    void setHeadingColor(const QColor &color);
    QColor boldColor() const { return m_boldColor; }
    void setBoldColor(const QColor &color);
    QColor italicColor() const { return m_italicColor; }
    void setItalicColor(const QColor &color);
    QColor codeColor() const { return m_codeColor; }
    void setCodeColor(const QColor &color);
    QColor inlineCodeColor() const { return m_inlineCodeColor; }
    void setInlineCodeColor(const QColor &color);
    QColor inlineCodeBackground() const { return m_inlineCodeBackground; }
    void setInlineCodeBackground(const QColor &color);
    QColor codeBackground() const { return m_codeBackground; }
    void setCodeBackground(const QColor &color);
    QColor linkColor() const { return m_linkColor; }
    void setLinkColor(const QColor &color);
    QColor markerColor() const { return m_markerColor; }
    void setMarkerColor(const QColor &color);
    QColor quoteColor() const { return m_quoteColor; }
    void setQuoteColor(const QColor &color);
    QColor delimiterColor() const { return m_delimiterColor; }
    void setDelimiterColor(const QColor &color);
    QColor ruleColor() const { return m_ruleColor; }
    void setRuleColor(const QColor &color);
    QColor highlightBackground() const { return m_highlightBackground; }
    void setHighlightBackground(const QColor &color);
    QColor codeKeywordColor() const { return m_codePalette.keyword; }
    void setCodeKeywordColor(const QColor &color);
    QColor codeTypeColor() const { return m_codePalette.type; }
    void setCodeTypeColor(const QColor &color);
    QColor codeStringColor() const { return m_codePalette.string; }
    void setCodeStringColor(const QColor &color);
    QColor codeNumberColor() const { return m_codePalette.number; }
    void setCodeNumberColor(const QColor &color);
    QColor codeCommentColor() const { return m_codePalette.comment; }
    void setCodeCommentColor(const QColor &color);
    QColor codeFunctionColor() const { return m_codePalette.function; }
    void setCodeFunctionColor(const QColor &color);
    QColor codeVariableColor() const { return m_codePalette.variable; }
    void setCodeVariableColor(const QColor &color);

signals:
    void documentChanged();
    void paletteChanged();

protected:
    void highlightBlock(const QString &text) override;

private:
    enum FenceKind { NoFence = 0, BacktickFence = 1, TildeFence = 2 };

    struct Fence {
        int kind = NoFence;
        int length = 0;
        int end = 0;
        bool canClose = false;
    };

    static Fence fenceAt(const QString &text);
    static int encodeFence(const Fence &fence, CodeSyntax::Language language,
                           int syntaxState = 0)
    {
        return fence.kind | (qMin(fence.length, 0xFFF) << 4)
               | (int(language) << 16) | (syntaxState << 20);
    }
    static int fenceKindOf(int state) { return state & 0xF; }
    static int fenceLengthOf(int state) { return (state >> 4) & 0xFFF; }
    static CodeSyntax::Language fenceLanguageOf(int state)
    {
        return CodeSyntax::Language((state >> 16) & 0xF);
    }
    static int syntaxStateOf(int state) { return (state >> 20) & 0xF; }

    void rebuildFormats();
    void schedulePaletteUpdate();

    void applySpan(int start, int count, const QTextCharFormat &format);

    void highlightInline(const QString &text, int from);
    bool isMasked(int start, int count) const;
    void mask(int start, int count);

    QTextCharFormat headingFormat(int level) const;

    QQuickTextDocument *m_quickDocument = nullptr;
    qreal m_baseFontSize = 11.0;

    QColor m_headingColor;
    QColor m_boldColor;
    QColor m_italicColor;
    QColor m_codeColor;
    QColor m_codeBackground;
    QColor m_inlineCodeColor;
    QColor m_inlineCodeBackground;
    QColor m_linkColor;
    QColor m_markerColor;
    QColor m_quoteColor;
    QColor m_delimiterColor;
    QColor m_ruleColor;
    QColor m_highlightBackground;
    CodeSyntax::Palette m_codePalette;

    QTextCharFormat m_boldFormat;
    QTextCharFormat m_italicFormat;
    QTextCharFormat m_codeFormat;
    QTextCharFormat m_codeBlockFormat;
    QTextCharFormat m_fenceDelimiterFormat;
    QTextCharFormat m_linkTextFormat;
    QTextCharFormat m_urlFormat;
    QTextCharFormat m_markerFormat;
    QTextCharFormat m_quoteFormat;
    QTextCharFormat m_delimiterFormat;
    QTextCharFormat m_ruleFormat;
    QTextCharFormat m_strikeFormat;
    QTextCharFormat m_highlightFormat;
    QTextCharFormat m_footnoteFormat;
    QTextCharFormat m_subscriptFormat;
    QTextCharFormat m_superscriptFormat;

    QList<bool> m_mask;
    bool m_paletteUpdateQueued = false;
};
