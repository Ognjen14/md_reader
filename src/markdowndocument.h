#pragma once

#include "codesyntax.h"

#include <QColor>
#include <QHash>
#include <QList>
#include <QObject>
#include <QPoint>
#include <QQuickTextDocument>
#include <QUrl>
#include <QtQml/qqmlregistration.h>


class MarkdownDocument : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QQuickTextDocument *document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(QUrl baseUrl READ baseUrl WRITE setBaseUrl NOTIFY baseUrlChanged)
    Q_PROPERTY(qreal lineHeight READ lineHeight WRITE setLineHeight NOTIFY styleChanged)
    Q_PROPERTY(QColor codeBackground READ codeBackground WRITE setCodeBackground NOTIFY styleChanged)
    Q_PROPERTY(QColor headingColor READ headingColor WRITE setHeadingColor NOTIFY styleChanged)
    Q_PROPERTY(QColor codeColor READ codeColor WRITE setCodeColor NOTIFY styleChanged)
    Q_PROPERTY(QColor inlineCodeColor READ inlineCodeColor WRITE setInlineCodeColor NOTIFY styleChanged)
    Q_PROPERTY(QColor inlineCodeBackground READ inlineCodeBackground WRITE setInlineCodeBackground NOTIFY styleChanged)
    Q_PROPERTY(QColor linkColor READ linkColor WRITE setLinkColor NOTIFY styleChanged)
    Q_PROPERTY(QColor quoteColor READ quoteColor WRITE setQuoteColor NOTIFY styleChanged)
    Q_PROPERTY(QColor boldColor READ boldColor WRITE setBoldColor NOTIFY styleChanged)
    Q_PROPERTY(QColor italicColor READ italicColor WRITE setItalicColor NOTIFY styleChanged)
    Q_PROPERTY(QColor markerColor READ markerColor WRITE setMarkerColor NOTIFY styleChanged)
    Q_PROPERTY(QColor ruleColor READ ruleColor WRITE setRuleColor NOTIFY styleChanged)
    Q_PROPERTY(QColor highlightBackground READ highlightBackground WRITE setHighlightBackground NOTIFY styleChanged)
    Q_PROPERTY(QColor codeKeywordColor READ codeKeywordColor WRITE setCodeKeywordColor NOTIFY styleChanged)
    Q_PROPERTY(QColor codeTypeColor READ codeTypeColor WRITE setCodeTypeColor NOTIFY styleChanged)
    Q_PROPERTY(QColor codeStringColor READ codeStringColor WRITE setCodeStringColor NOTIFY styleChanged)
    Q_PROPERTY(QColor codeNumberColor READ codeNumberColor WRITE setCodeNumberColor NOTIFY styleChanged)
    Q_PROPERTY(QColor codeCommentColor READ codeCommentColor WRITE setCodeCommentColor NOTIFY styleChanged)
    Q_PROPERTY(QColor codeFunctionColor READ codeFunctionColor WRITE setCodeFunctionColor NOTIFY styleChanged)
    Q_PROPERTY(QColor codeVariableColor READ codeVariableColor WRITE setCodeVariableColor NOTIFY styleChanged)
    Q_PROPERTY(QString codeFont READ codeFont WRITE setCodeFont NOTIFY styleChanged)
    Q_PROPERTY(QString checkboxFont READ checkboxFont WRITE setCheckboxFont NOTIFY styleChanged)
    Q_PROPERTY(QString checkedGlyph READ checkedGlyph WRITE setCheckedGlyph NOTIFY styleChanged)
    Q_PROPERTY(QString uncheckedGlyph READ uncheckedGlyph WRITE setUncheckedGlyph NOTIFY styleChanged)
    Q_PROPERTY(QColor checkedColor READ checkedColor WRITE setCheckedColor NOTIFY styleChanged)
    Q_PROPERTY(QColor uncheckedColor READ uncheckedColor WRITE setUncheckedColor NOTIFY styleChanged)
    Q_PROPERTY(QVariantList codeRegions READ codeRegions NOTIFY codeRegionsChanged)
    Q_PROPERTY(QColor tableBorderColor READ tableBorderColor WRITE setTableBorderColor NOTIFY styleChanged)
    Q_PROPERTY(QColor tableHeaderBackground READ tableHeaderBackground WRITE setTableHeaderBackground NOTIFY styleChanged)
    Q_PROPERTY(QColor tableStripeBackground READ tableStripeBackground WRITE setTableStripeBackground NOTIFY styleChanged)

public:
    explicit MarkdownDocument(QObject *parent = nullptr);

    QQuickTextDocument *document() const { return m_document; }
    void setDocument(QQuickTextDocument *document);

    qreal lineHeight() const { return m_lineHeight; }
    void setLineHeight(qreal percent);

    QColor codeBackground() const { return m_codeBackground; }
    void setCodeBackground(const QColor &color);
    QColor headingColor() const { return m_headingColor; }
    void setHeadingColor(const QColor &color);
    QColor codeColor() const { return m_codeColor; }
    void setCodeColor(const QColor &color);
    QColor inlineCodeColor() const { return m_inlineCodeColor; }
    void setInlineCodeColor(const QColor &color);
    QColor inlineCodeBackground() const { return m_inlineCodeBackground; }
    void setInlineCodeBackground(const QColor &color);
    QColor linkColor() const { return m_linkColor; }
    void setLinkColor(const QColor &color);
    QColor quoteColor() const { return m_quoteColor; }
    void setQuoteColor(const QColor &color);
    QColor boldColor() const { return m_boldColor; }
    void setBoldColor(const QColor &color);
    QColor italicColor() const { return m_italicColor; }
    void setItalicColor(const QColor &color);
    QColor markerColor() const { return m_markerColor; }
    void setMarkerColor(const QColor &color);
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
    QString codeFont() const { return m_codeFont; }
    void setCodeFont(const QString &family);
    QUrl baseUrl() const { return m_baseUrl; }
    void setBaseUrl(const QUrl &url);

    QString checkboxFont() const { return m_checkboxFont; }
    void setCheckboxFont(const QString &family);
    QString checkedGlyph() const { return m_checkedGlyph; }
    void setCheckedGlyph(const QString &glyph);
    QString uncheckedGlyph() const { return m_uncheckedGlyph; }
    void setUncheckedGlyph(const QString &glyph);
    QColor checkedColor() const { return m_checkedColor; }
    void setCheckedColor(const QColor &color);
    QColor uncheckedColor() const { return m_uncheckedColor; }
    void setUncheckedColor(const QColor &color);

    QColor tableBorderColor() const { return m_tableBorderColor; }
    void setTableBorderColor(const QColor &color);
    QColor tableHeaderBackground() const { return m_tableHeaderBackground; }
    void setTableHeaderBackground(const QColor &color);
    QColor tableStripeBackground() const { return m_tableStripeBackground; }
    void setTableStripeBackground(const QColor &color);

    Q_INVOKABLE void restyle();

    void applyStyle(QTextDocument *doc);

    Q_INVOKABLE QString preprocess(const QString &markdown) const;

    Q_INVOKABLE QPoint lineColumnAt(int position) const;
    Q_INVOKABLE int positionForLine(int line) const;

    Q_INVOKABLE qreal headingTop(int index) const;
    Q_INVOKABLE int headingIndexAt(qreal y) const;

    QVariantList codeRegions() const;

    Q_INVOKABLE qreal anchorTop(const QString &anchor) const;

    Q_INVOKABLE void copyToClipboard(const QString &text) const;

    Q_INVOKABLE void refreshViewportCulling() { disableViewportCulling(); }

signals:
    void documentChanged();
    void baseUrlChanged();
    void styleChanged();
    void codeRegionsChanged();

private:
    void applyExtendedSyntax(QTextDocument *doc);
    void applyTaskListStyle(QTextDocument *doc);
    void applyTableStyle(QTextDocument *doc);

    void rebuildHeadingCache() const;
    void invalidateHeadingCache();
    void disableViewportCulling();

    static bool isCodeBlock(const QTextBlock &block);
    void rebuildCodeRegions() const;

    QQuickTextDocument *m_document = nullptr;
    QUrl m_baseUrl;
    qreal m_lineHeight = 150.0;
    QColor m_codeBackground;
    QColor m_headingColor;
    QColor m_codeColor;
    QColor m_inlineCodeColor;
    QColor m_inlineCodeBackground;
    QColor m_linkColor;
    QColor m_quoteColor;
    QColor m_boldColor;
    QColor m_italicColor;
    QColor m_markerColor;
    QColor m_ruleColor;
    QColor m_highlightBackground;
    CodeSyntax::Palette m_codePalette;

    QString m_codeFont;
    QString m_checkboxFont;
    QString m_checkedGlyph;
    QString m_uncheckedGlyph;
    QColor m_checkedColor;
    QColor m_uncheckedColor;

    QColor m_tableBorderColor;
    QColor m_tableHeaderBackground;
    QColor m_tableStripeBackground;

    bool m_applying = false;

    mutable QVariantList m_codeRegions;
    mutable bool m_codeRegionsDirty = true;

    QHash<QString, int> m_customAnchors;

    QMetaObject::Connection m_contentsConnection;
    QMetaObject::Connection m_layoutConnection;
    mutable QList<qreal> m_headingTops;
    mutable bool m_headingCacheDirty = true;
};
