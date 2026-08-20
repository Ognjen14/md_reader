#pragma once

#include <QColor>
#include <QList>
#include <QString>
#include <QStringView>
namespace CodeSyntax {

enum class Language {
    Plain = 0,
    C,
    Cpp,
    CSharp,
    Python,
    JavaScript,
    PowerShell,
    Shell,
    Batch
};

enum class Role {
    Keyword,
    Type,
    String,
    Number,
    Comment,
    Function,
    Variable
};

struct Palette {
    QColor keyword;
    QColor type;
    QColor string;
    QColor number;
    QColor comment;
    QColor function;
    QColor variable;

    QColor color(Role role) const;
};

struct Span {
    int start = 0;
    int length = 0;
    Role role = Role::Keyword;
};

struct LineResult {
    QList<Span> spans;
    int state = 0;
};

Language languageFromInfo(QStringView info);


LineResult highlightLine(QStringView text, Language language, int previousState = 0);

}
