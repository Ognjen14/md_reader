#include "appsettings.h"

#include <QFileInfo>
#include <QGuiApplication>
#include <QRect>
#include <QScreen>

AppSettings::AppSettings(QObject *parent)
    : QObject(parent)
{
    m_darkTheme      = m_settings.value(QStringLiteral("ui/darkTheme"), m_darkTheme).toBool();
    m_editMode       = m_settings.value(QStringLiteral("ui/editMode"), m_editMode).toBool();
    m_outlineVisible = m_settings.value(QStringLiteral("ui/outlineVisible"), m_outlineVisible).toBool();
    m_outlineWidth   = m_settings.value(QStringLiteral("ui/outlineWidth"), m_outlineWidth).toInt();
    m_splitView      = m_settings.value(QStringLiteral("ui/splitView"), m_splitView).toBool();
    m_splitRatio     = m_settings.value(QStringLiteral("ui/splitRatio"), m_splitRatio).toInt();
    m_syncScroll     = m_settings.value(QStringLiteral("ui/syncScroll"), m_syncScroll).toBool();
    m_viewFontSize   = m_settings.value(QStringLiteral("ui/viewFontSize"), m_viewFontSize).toInt();
    m_editorFontSize = m_settings.value(QStringLiteral("editor/fontPointSize"), m_editorFontSize).toInt();
    m_tabSpaces      = m_settings.value(QStringLiteral("editor/tabSpaces"), m_tabSpaces).toInt();
    m_viewFontFamily = m_settings.value(QStringLiteral("ui/viewFontFamily")).toString();
    m_editorFontFamily = m_settings.value(QStringLiteral("editor/fontFamily")).toString();
    m_openFiles      = m_settings.value(QStringLiteral("files/open")).toStringList();
    m_currentTab     = m_settings.value(QStringLiteral("files/currentTab"), 0).toInt();
    m_recentFiles    = m_settings.value(QStringLiteral("files/recent")).toStringList();

    m_outlineWidth   = qBound(140, m_outlineWidth, 640);
    m_splitRatio     = qBound(20, m_splitRatio, 80);
    m_viewFontSize   = qBound(8, m_viewFontSize, 48);
    m_editorFontSize = qBound(6, m_editorFontSize, 40);
    m_tabSpaces      = qBound(1, m_tabSpaces, 8);

    const auto keepExisting = [](const QStringList &paths) {
        QStringList surviving;
        surviving.reserve(paths.size());
        for (const QString &path : paths) {
            if (QFileInfo::exists(path))
                surviving.append(path);
        }
        return surviving;
    };
    m_recentFiles = keepExisting(m_recentFiles);
    m_openFiles = keepExisting(m_openFiles);
    m_currentTab = qBound(0, m_currentTab, qMax(0, int(m_openFiles.size()) - 1));

    m_windowMaximized = m_settings.value(QStringLiteral("window/maximized"), m_windowMaximized).toBool();
    m_windowWidth     = m_settings.value(QStringLiteral("window/width"), m_windowWidth).toInt();
    m_windowHeight    = m_settings.value(QStringLiteral("window/height"), m_windowHeight).toInt();
    const bool hasSavedPosition = m_settings.contains(QStringLiteral("window/x"))
                                  && m_settings.contains(QStringLiteral("window/y"));
    m_windowX         = m_settings.value(QStringLiteral("window/x"), -1).toInt();
    m_windowY         = m_settings.value(QStringLiteral("window/y"), -1).toInt();
    clampGeometryToScreens(hasSavedPosition);
}

void AppSettings::clampGeometryToScreens(bool hasSavedPosition)
{
    m_windowWidth  = qBound(400, m_windowWidth, 10000);
    m_windowHeight = qBound(300, m_windowHeight, 10000);

    const QRect saved(m_windowX, m_windowY, m_windowWidth, m_windowHeight);
    bool visible = false;
    const auto screens = QGuiApplication::screens();
    for (const QScreen *screen : screens) {
        if (screen->availableGeometry().intersects(saved)) {
            visible = true;
            break;
        }
    }

    if (hasSavedPosition && visible)
        return;

    const QScreen *primary = QGuiApplication::primaryScreen();
    const QRect area = primary ? primary->availableGeometry() : QRect(0, 0, 1920, 1080);
    m_windowWidth  = qMin(m_windowWidth, area.width());
    m_windowHeight = qMin(m_windowHeight, area.height());
    m_windowX = area.x() + (area.width() - m_windowWidth) / 2;
    m_windowY = area.y() + (area.height() - m_windowHeight) / 2;
}

void AppSettings::setDarkTheme(bool v)
{
    if (m_darkTheme == v)
        return;
    m_darkTheme = v;
    m_settings.setValue(QStringLiteral("ui/darkTheme"), v);
    emit darkThemeChanged();
}

void AppSettings::setEditMode(bool v)
{
    if (m_editMode == v)
        return;
    m_editMode = v;
    m_settings.setValue(QStringLiteral("ui/editMode"), v);
    emit editModeChanged();
}

void AppSettings::setOutlineVisible(bool v)
{
    if (m_outlineVisible == v)
        return;
    m_outlineVisible = v;
    m_settings.setValue(QStringLiteral("ui/outlineVisible"), v);
    emit outlineVisibleChanged();
}

void AppSettings::setOutlineWidth(int v)
{
    v = qBound(140, v, 640);
    if (m_outlineWidth == v)
        return;
    m_outlineWidth = v;
    m_settings.setValue(QStringLiteral("ui/outlineWidth"), v);
    emit outlineWidthChanged();
}

void AppSettings::setSplitView(bool v)
{
    if (m_splitView == v)
        return;
    m_splitView = v;
    m_settings.setValue(QStringLiteral("ui/splitView"), v);
    emit splitViewChanged();
}

void AppSettings::setSplitRatio(int v)
{
    v = qBound(20, v, 80);
    if (m_splitRatio == v)
        return;
    m_splitRatio = v;
    m_settings.setValue(QStringLiteral("ui/splitRatio"), v);
    emit splitRatioChanged();
}

void AppSettings::setSyncScroll(bool v)
{
    if (m_syncScroll == v)
        return;
    m_syncScroll = v;
    m_settings.setValue(QStringLiteral("ui/syncScroll"), v);
    emit syncScrollChanged();
}

void AppSettings::setViewFontSize(int v)
{
    v = qBound(8, v, 48);
    if (m_viewFontSize == v)
        return;
    m_viewFontSize = v;
    m_settings.setValue(QStringLiteral("ui/viewFontSize"), v);
    emit viewFontSizeChanged();
}

void AppSettings::setEditorFontSize(int v)
{
    v = qBound(6, v, 40);
    if (m_editorFontSize == v)
        return;
    m_editorFontSize = v;
    m_settings.setValue(QStringLiteral("editor/fontPointSize"), v);
    emit editorFontSizeChanged();
}

void AppSettings::setTabSpaces(int v)
{
    v = qBound(1, v, 8);
    if (m_tabSpaces == v)
        return;
    m_tabSpaces = v;
    m_settings.setValue(QStringLiteral("editor/tabSpaces"), v);
    emit tabSpacesChanged();
}


void AppSettings::setViewFontFamily(const QString &v)
{
    if (m_viewFontFamily == v)
        return;
    m_viewFontFamily = v;
    m_settings.setValue(QStringLiteral("ui/viewFontFamily"), v);
    emit viewFontFamilyChanged();
}

void AppSettings::setEditorFontFamily(const QString &v)
{
    if (m_editorFontFamily == v)
        return;
    m_editorFontFamily = v;
    m_settings.setValue(QStringLiteral("editor/fontFamily"), v);
    emit editorFontFamilyChanged();
}

void AppSettings::setOpenFiles(const QStringList &v)
{
    if (m_openFiles == v)
        return;
    m_openFiles = v;
    m_settings.setValue(QStringLiteral("files/open"), v);
    emit openFilesChanged();
}

void AppSettings::setCurrentTab(int v)
{
    v = qMax(0, v);
    if (m_currentTab == v)
        return;
    m_currentTab = v;
    m_settings.setValue(QStringLiteral("files/currentTab"), v);
    emit currentTabChanged();
}

void AppSettings::saveGeometry(int x, int y, int w, int h, bool maximized)
{
    m_settings.setValue(QStringLiteral("window/maximized"), maximized);
    if (!maximized) {
        m_settings.setValue(QStringLiteral("window/x"), x);
        m_settings.setValue(QStringLiteral("window/y"), y);
        m_settings.setValue(QStringLiteral("window/width"), w);
        m_settings.setValue(QStringLiteral("window/height"), h);
    }
    m_settings.sync();
}

void AppSettings::addRecentFile(const QString &path)
{
    if (path.isEmpty())
        return;
    const QString canonical = QFileInfo(path).absoluteFilePath();
    m_recentFiles.removeAll(canonical);
    m_recentFiles.prepend(canonical);
    while (m_recentFiles.size() > MaxRecentFiles)
        m_recentFiles.removeLast();
    m_settings.setValue(QStringLiteral("files/recent"), m_recentFiles);
    emit recentFilesChanged();
}

void AppSettings::clearRecentFiles()
{
    if (m_recentFiles.isEmpty())
        return;
    m_recentFiles.clear();
    m_settings.setValue(QStringLiteral("files/recent"), m_recentFiles);
    emit recentFilesChanged();
}
