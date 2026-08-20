#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QtQml/qqmlregistration.h>

class AppSettings : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool darkTheme READ darkTheme WRITE setDarkTheme NOTIFY darkThemeChanged)
    Q_PROPERTY(bool editMode READ editMode WRITE setEditMode NOTIFY editModeChanged)
    Q_PROPERTY(bool outlineVisible READ outlineVisible WRITE setOutlineVisible NOTIFY outlineVisibleChanged)
    Q_PROPERTY(int outlineWidth READ outlineWidth WRITE setOutlineWidth NOTIFY outlineWidthChanged)
    Q_PROPERTY(bool splitView READ splitView WRITE setSplitView NOTIFY splitViewChanged)
    Q_PROPERTY(int splitRatio READ splitRatio WRITE setSplitRatio NOTIFY splitRatioChanged)
    Q_PROPERTY(bool syncScroll READ syncScroll WRITE setSyncScroll NOTIFY syncScrollChanged)
    Q_PROPERTY(int viewFontSize READ viewFontSize WRITE setViewFontSize NOTIFY viewFontSizeChanged)
    Q_PROPERTY(int editorFontSize READ editorFontSize WRITE setEditorFontSize NOTIFY editorFontSizeChanged)
    Q_PROPERTY(int tabSpaces READ tabSpaces WRITE setTabSpaces NOTIFY tabSpacesChanged)
    Q_PROPERTY(QString viewFontFamily READ viewFontFamily WRITE setViewFontFamily NOTIFY viewFontFamilyChanged)
    Q_PROPERTY(QString editorFontFamily READ editorFontFamily WRITE setEditorFontFamily NOTIFY editorFontFamilyChanged)
    Q_PROPERTY(QStringList openFiles READ openFiles WRITE setOpenFiles NOTIFY openFilesChanged)
    Q_PROPERTY(int currentTab READ currentTab WRITE setCurrentTab NOTIFY currentTabChanged)
    Q_PROPERTY(QStringList recentFiles READ recentFiles NOTIFY recentFilesChanged)

    Q_PROPERTY(int windowX READ windowX CONSTANT)
    Q_PROPERTY(int windowY READ windowY CONSTANT)
    Q_PROPERTY(int windowWidth READ windowWidth CONSTANT)
    Q_PROPERTY(int windowHeight READ windowHeight CONSTANT)
    Q_PROPERTY(bool windowMaximized READ windowMaximized CONSTANT)

public:
    explicit AppSettings(QObject *parent = nullptr);

    bool darkTheme() const { return m_darkTheme; }
    void setDarkTheme(bool v);

    bool editMode() const { return m_editMode; }
    void setEditMode(bool v);

    bool outlineVisible() const { return m_outlineVisible; }
    void setOutlineVisible(bool v);

    int outlineWidth() const { return m_outlineWidth; }
    void setOutlineWidth(int v);

    bool splitView() const { return m_splitView; }
    void setSplitView(bool v);

    int splitRatio() const { return m_splitRatio; }
    void setSplitRatio(int v);

    bool syncScroll() const { return m_syncScroll; }
    void setSyncScroll(bool v);

    int viewFontSize() const { return m_viewFontSize; }
    void setViewFontSize(int v);

    int editorFontSize() const { return m_editorFontSize; }
    void setEditorFontSize(int v);

    int tabSpaces() const { return m_tabSpaces; }
    void setTabSpaces(int v);

    QString viewFontFamily() const { return m_viewFontFamily; }
    void setViewFontFamily(const QString &v);

    QString editorFontFamily() const { return m_editorFontFamily; }
    void setEditorFontFamily(const QString &v);

    QStringList openFiles() const { return m_openFiles; }
    void setOpenFiles(const QStringList &v);

    int currentTab() const { return m_currentTab; }
    void setCurrentTab(int v);

    QStringList recentFiles() const { return m_recentFiles; }

    int windowX() const { return m_windowX; }
    int windowY() const { return m_windowY; }
    int windowWidth() const { return m_windowWidth; }
    int windowHeight() const { return m_windowHeight; }
    bool windowMaximized() const { return m_windowMaximized; }

    Q_INVOKABLE void saveGeometry(int x, int y, int w, int h, bool maximized);
    Q_INVOKABLE void addRecentFile(const QString &path);
    Q_INVOKABLE void clearRecentFiles();

signals:
    void darkThemeChanged();
    void editModeChanged();
    void outlineVisibleChanged();
    void outlineWidthChanged();
    void splitViewChanged();
    void splitRatioChanged();
    void syncScrollChanged();
    void viewFontSizeChanged();
    void editorFontSizeChanged();
    void tabSpacesChanged();
    void viewFontFamilyChanged();
    void editorFontFamilyChanged();
    void openFilesChanged();
    void currentTabChanged();
    void recentFilesChanged();

private:
    void clampGeometryToScreens(bool hasSavedPosition);

    static constexpr int MaxRecentFiles = 10;

    QSettings m_settings;

    bool m_darkTheme = true;
    bool m_editMode = false;
    bool m_outlineVisible = true;
    int m_outlineWidth = 260;
    bool m_splitView = false;
    int m_splitRatio = 50;
    bool m_syncScroll = true;
    int m_viewFontSize = 15;
    int m_editorFontSize = 11;
    int m_tabSpaces = 2;
    QString m_viewFontFamily;
    QString m_editorFontFamily;
    QStringList m_openFiles;
    int m_currentTab = 0;
    QStringList m_recentFiles;

    int m_windowX = 0;
    int m_windowY = 0;
    int m_windowWidth = 1100;
    int m_windowHeight = 760;
    bool m_windowMaximized = false;
};
