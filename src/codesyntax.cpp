#include "codesyntax.h"

#include <QSet>

namespace CodeSyntax {
namespace {

enum LexerState {
    NormalState = 0,
    CBlockCommentState,
    PythonTripleSingleState,
    PythonTripleDoubleState,
    JavaScriptTemplateState,
    PowerShellBlockCommentState,
    PowerShellHereSingleState,
    PowerShellHereDoubleState,
    ShellSingleQuoteState,
    ShellDoubleQuoteState
};

void appendSpan(LineResult &result, int start, int end, Role role)
{
    if (end <= start)
        return;
    if (!result.spans.isEmpty()) {
        Span &last = result.spans.last();
        if (last.role == role && last.start + last.length == start) {
            last.length += end - start;
            return;
        }
    }
    result.spans.append({start, end - start, role});
}

bool isIdentifierStart(QChar ch)
{
    return ch == QLatin1Char('_') || ch.isLetter();
}

bool isIdentifierPart(QChar ch)
{
    return ch == QLatin1Char('_') || ch.isLetterOrNumber();
}

int identifierEnd(QStringView text, int start)
{
    int i = start + 1;
    while (i < text.size() && isIdentifierPart(text.at(i)))
        ++i;
    return i;
}

int nextNonSpace(QStringView text, int start)
{
    int i = start;
    while (i < text.size() && text.at(i).isSpace())
        ++i;
    return i;
}

int quotedEnd(QStringView text, int start, QChar quote, bool doubledQuoteEscapes = false)
{
    int i = start + 1;
    while (i < text.size()) {
        if (!doubledQuoteEscapes && text.at(i) == QLatin1Char('\\')) {
            i += 2;
            continue;
        }
        if (text.at(i) == quote) {
            if (doubledQuoteEscapes && i + 1 < text.size() && text.at(i + 1) == quote) {
                i += 2;
                continue;
            }
            return i + 1;
        }
        ++i;
    }
    return text.size();
}

int numberEnd(QStringView text, int start)
{
    int i = start;
    if (i + 1 < text.size() && text.at(i) == QLatin1Char('0')
        && QStringView(u"xXbBoO").contains(text.at(i + 1))) {
        i += 2;
        while (i < text.size() && (text.at(i).isLetterOrNumber() || text.at(i) == QLatin1Char('_')))
            ++i;
        return i;
    }

    bool exponentSeen = false;
    while (i < text.size()) {
        const QChar ch = text.at(i);
        if (ch.isDigit() || ch == QLatin1Char('_') || ch == QLatin1Char('.')) {
            ++i;
        } else if (!exponentSeen && (ch == QLatin1Char('e') || ch == QLatin1Char('E'))) {
            exponentSeen = true;
            ++i;
            if (i < text.size() && (text.at(i) == QLatin1Char('+') || text.at(i) == QLatin1Char('-')))
                ++i;
        } else if (ch.isLetter()) {
            ++i;
        } else {
            break;
        }
    }
    return i;
}

bool contains(const QSet<QString> &set, QStringView token, Qt::CaseSensitivity sensitivity = Qt::CaseSensitive)
{
    const QString value = token.toString();
    return set.contains(sensitivity == Qt::CaseInsensitive ? value.toLower() : value);
}

const QSet<QString> &cKeywords()
{
    static const QSet<QString> values = {
        QStringLiteral("auto"), QStringLiteral("break"), QStringLiteral("case"),
        QStringLiteral("const"), QStringLiteral("continue"), QStringLiteral("default"),
        QStringLiteral("do"), QStringLiteral("else"), QStringLiteral("enum"),
        QStringLiteral("extern"), QStringLiteral("for"), QStringLiteral("goto"),
        QStringLiteral("if"), QStringLiteral("inline"), QStringLiteral("register"),
        QStringLiteral("restrict"), QStringLiteral("return"), QStringLiteral("sizeof"),
        QStringLiteral("static"), QStringLiteral("struct"), QStringLiteral("switch"),
        QStringLiteral("typedef"), QStringLiteral("union"), QStringLiteral("volatile"),
        QStringLiteral("while"), QStringLiteral("true"), QStringLiteral("false"),
        QStringLiteral("_Alignas"), QStringLiteral("_Alignof"),
        QStringLiteral("_Atomic"), QStringLiteral("_Generic"), QStringLiteral("_Noreturn"),
        QStringLiteral("_Static_assert"), QStringLiteral("_Thread_local")
    };
    return values;
}

const QSet<QString> &cppKeywords()
{
    static QSet<QString> values = [] {
        QSet<QString> result = cKeywords();
        const QSet<QString> additions = {
            QStringLiteral("alignas"), QStringLiteral("alignof"), QStringLiteral("and"),
            QStringLiteral("and_eq"), QStringLiteral("asm"), QStringLiteral("bitand"),
            QStringLiteral("bitor"), QStringLiteral("catch"), QStringLiteral("class"),
            QStringLiteral("compl"), QStringLiteral("concept"), QStringLiteral("consteval"),
            QStringLiteral("constexpr"), QStringLiteral("constinit"), QStringLiteral("const_cast"),
            QStringLiteral("co_await"), QStringLiteral("co_return"), QStringLiteral("co_yield"),
            QStringLiteral("decltype"), QStringLiteral("delete"), QStringLiteral("dynamic_cast"),
            QStringLiteral("explicit"), QStringLiteral("export"), QStringLiteral("friend"),
            QStringLiteral("mutable"), QStringLiteral("namespace"), QStringLiteral("new"),
            QStringLiteral("noexcept"), QStringLiteral("not"), QStringLiteral("not_eq"),
            QStringLiteral("operator"), QStringLiteral("or"), QStringLiteral("or_eq"),
            QStringLiteral("private"), QStringLiteral("protected"), QStringLiteral("public"),
            QStringLiteral("reflexpr"), QStringLiteral("reinterpret_cast"), QStringLiteral("requires"),
            QStringLiteral("static_assert"), QStringLiteral("static_cast"), QStringLiteral("template"),
            QStringLiteral("this"), QStringLiteral("thread_local"), QStringLiteral("throw"),
            QStringLiteral("try"), QStringLiteral("typeid"), QStringLiteral("typename"),
            QStringLiteral("using"), QStringLiteral("virtual"), QStringLiteral("nullptr"),
            QStringLiteral("xor"),
            QStringLiteral("xor_eq")
        };
        result.unite(additions);
        return result;
    }();
    return values;
}

const QSet<QString> &cSharpKeywords()
{
    static const QSet<QString> values = {
        QStringLiteral("abstract"), QStringLiteral("as"), QStringLiteral("base"),
        QStringLiteral("break"), QStringLiteral("case"), QStringLiteral("catch"),
        QStringLiteral("checked"), QStringLiteral("class"), QStringLiteral("const"),
        QStringLiteral("continue"), QStringLiteral("default"), QStringLiteral("delegate"),
        QStringLiteral("do"), QStringLiteral("else"), QStringLiteral("enum"),
        QStringLiteral("event"), QStringLiteral("explicit"), QStringLiteral("extern"),
        QStringLiteral("finally"), QStringLiteral("fixed"), QStringLiteral("for"),
        QStringLiteral("foreach"), QStringLiteral("goto"), QStringLiteral("if"),
        QStringLiteral("implicit"), QStringLiteral("in"), QStringLiteral("interface"),
        QStringLiteral("internal"), QStringLiteral("is"), QStringLiteral("lock"),
        QStringLiteral("namespace"), QStringLiteral("new"), QStringLiteral("operator"),
        QStringLiteral("out"), QStringLiteral("override"), QStringLiteral("params"),
        QStringLiteral("private"), QStringLiteral("protected"), QStringLiteral("public"),
        QStringLiteral("readonly"), QStringLiteral("record"), QStringLiteral("ref"),
        QStringLiteral("return"), QStringLiteral("sealed"), QStringLiteral("sizeof"),
        QStringLiteral("stackalloc"), QStringLiteral("static"), QStringLiteral("struct"),
        QStringLiteral("switch"), QStringLiteral("this"), QStringLiteral("throw"),
        QStringLiteral("try"), QStringLiteral("typeof"), QStringLiteral("unchecked"),
        QStringLiteral("unsafe"), QStringLiteral("using"), QStringLiteral("virtual"),
        QStringLiteral("volatile"), QStringLiteral("while"), QStringLiteral("async"),
        QStringLiteral("await"), QStringLiteral("get"), QStringLiteral("init"),
        QStringLiteral("set"), QStringLiteral("value"), QStringLiteral("var"),
        QStringLiteral("when"), QStringLiteral("where"), QStringLiteral("yield"),
        QStringLiteral("true"), QStringLiteral("false"), QStringLiteral("null")
    };
    return values;
}

const QSet<QString> &cFamilyTypes()
{
    static const QSet<QString> values = {
        QStringLiteral("bool"), QStringLiteral("byte"), QStringLiteral("char"),
        QStringLiteral("char8_t"), QStringLiteral("char16_t"), QStringLiteral("char32_t"),
        QStringLiteral("decimal"), QStringLiteral("double"), QStringLiteral("dynamic"),
        QStringLiteral("float"), QStringLiteral("int"), QStringLiteral("int8_t"),
        QStringLiteral("int16_t"), QStringLiteral("int32_t"), QStringLiteral("int64_t"),
        QStringLiteral("long"), QStringLiteral("nint"), QStringLiteral("nuint"),
        QStringLiteral("object"), QStringLiteral("sbyte"), QStringLiteral("short"),
        QStringLiteral("signed"), QStringLiteral("size_t"), QStringLiteral("string"),
        QStringLiteral("uint"), QStringLiteral("uint8_t"), QStringLiteral("uint16_t"),
        QStringLiteral("uint32_t"), QStringLiteral("uint64_t"), QStringLiteral("ulong"),
        QStringLiteral("ushort"), QStringLiteral("unsigned"), QStringLiteral("void"),
        QStringLiteral("_Bool"),
        QStringLiteral("wchar_t")
    };
    return values;
}

const QSet<QString> &pythonKeywords()
{
    static const QSet<QString> values = {
        QStringLiteral("and"), QStringLiteral("as"), QStringLiteral("assert"),
        QStringLiteral("async"), QStringLiteral("await"), QStringLiteral("break"),
        QStringLiteral("case"), QStringLiteral("class"), QStringLiteral("continue"),
        QStringLiteral("def"), QStringLiteral("del"), QStringLiteral("elif"),
        QStringLiteral("else"), QStringLiteral("except"), QStringLiteral("finally"),
        QStringLiteral("for"), QStringLiteral("from"), QStringLiteral("global"),
        QStringLiteral("if"), QStringLiteral("import"), QStringLiteral("in"),
        QStringLiteral("is"), QStringLiteral("lambda"), QStringLiteral("match"),
        QStringLiteral("nonlocal"), QStringLiteral("not"), QStringLiteral("or"),
        QStringLiteral("pass"), QStringLiteral("raise"), QStringLiteral("return"),
        QStringLiteral("try"), QStringLiteral("while"), QStringLiteral("with"),
        QStringLiteral("yield")
    };
    return values;
}

const QSet<QString> &pythonBuiltins()
{
    static const QSet<QString> values = {
        QStringLiteral("bool"), QStringLiteral("bytes"), QStringLiteral("dict"),
        QStringLiteral("float"), QStringLiteral("frozenset"), QStringLiteral("int"),
        QStringLiteral("list"), QStringLiteral("object"), QStringLiteral("set"),
        QStringLiteral("str"), QStringLiteral("tuple"), QStringLiteral("type"),
        QStringLiteral("None"), QStringLiteral("True"), QStringLiteral("False")
    };
    return values;
}

const QSet<QString> &javaScriptKeywords()
{
    static const QSet<QString> values = {
        QStringLiteral("async"), QStringLiteral("await"), QStringLiteral("break"),
        QStringLiteral("case"), QStringLiteral("catch"), QStringLiteral("class"),
        QStringLiteral("const"), QStringLiteral("continue"), QStringLiteral("debugger"),
        QStringLiteral("default"), QStringLiteral("delete"), QStringLiteral("do"),
        QStringLiteral("else"), QStringLiteral("export"), QStringLiteral("extends"),
        QStringLiteral("finally"), QStringLiteral("for"), QStringLiteral("from"),
        QStringLiteral("function"), QStringLiteral("get"), QStringLiteral("if"),
        QStringLiteral("import"), QStringLiteral("in"), QStringLiteral("instanceof"),
        QStringLiteral("let"), QStringLiteral("new"), QStringLiteral("of"),
        QStringLiteral("return"), QStringLiteral("set"), QStringLiteral("static"),
        QStringLiteral("super"), QStringLiteral("switch"), QStringLiteral("this"),
        QStringLiteral("throw"), QStringLiteral("try"), QStringLiteral("typeof"),
        QStringLiteral("var"), QStringLiteral("void"), QStringLiteral("while"),
        QStringLiteral("with"), QStringLiteral("yield")
    };
    return values;
}

const QSet<QString> &javaScriptTypes()
{
    static const QSet<QString> values = {
        QStringLiteral("Array"), QStringLiteral("BigInt"), QStringLiteral("Boolean"),
        QStringLiteral("Date"), QStringLiteral("Error"), QStringLiteral("Map"),
        QStringLiteral("Math"), QStringLiteral("Number"), QStringLiteral("Object"),
        QStringLiteral("Promise"), QStringLiteral("RegExp"), QStringLiteral("Set"),
        QStringLiteral("String"), QStringLiteral("Symbol"), QStringLiteral("WeakMap"),
        QStringLiteral("WeakSet"), QStringLiteral("null"), QStringLiteral("true"),
        QStringLiteral("false"), QStringLiteral("undefined")
    };
    return values;
}

const QSet<QString> &powerShellKeywords()
{
    static const QSet<QString> values = {
        QStringLiteral("begin"), QStringLiteral("break"), QStringLiteral("catch"),
        QStringLiteral("class"), QStringLiteral("continue"), QStringLiteral("data"),
        QStringLiteral("do"), QStringLiteral("dynamicparam"), QStringLiteral("else"),
        QStringLiteral("elseif"), QStringLiteral("end"), QStringLiteral("enum"),
        QStringLiteral("exit"), QStringLiteral("filter"), QStringLiteral("finally"),
        QStringLiteral("for"), QStringLiteral("foreach"), QStringLiteral("from"),
        QStringLiteral("function"), QStringLiteral("if"), QStringLiteral("in"),
        QStringLiteral("param"), QStringLiteral("process"), QStringLiteral("return"),
        QStringLiteral("switch"), QStringLiteral("throw"), QStringLiteral("trap"),
        QStringLiteral("try"), QStringLiteral("until"), QStringLiteral("using"),
        QStringLiteral("while"), QStringLiteral("workflow")
    };
    return values;
}

const QSet<QString> &shellKeywords()
{
    static const QSet<QString> values = {
        QStringLiteral("case"), QStringLiteral("do"), QStringLiteral("done"),
        QStringLiteral("elif"), QStringLiteral("else"), QStringLiteral("esac"),
        QStringLiteral("fi"), QStringLiteral("for"), QStringLiteral("function"),
        QStringLiteral("if"), QStringLiteral("in"), QStringLiteral("select"),
        QStringLiteral("then"), QStringLiteral("time"), QStringLiteral("until"),
        QStringLiteral("while"), QStringLiteral("break"), QStringLiteral("continue"),
        QStringLiteral("return"), QStringLiteral("export"), QStringLiteral("local"),
        QStringLiteral("readonly"), QStringLiteral("declare"), QStringLiteral("typeset")
    };
    return values;
}

const QSet<QString> &batchKeywords()
{
    static const QSet<QString> values = {
        QStringLiteral("call"), QStringLiteral("choice"), QStringLiteral("do"),
        QStringLiteral("else"), QStringLiteral("endlocal"), QStringLiteral("exit"),
        QStringLiteral("for"), QStringLiteral("goto"), QStringLiteral("if"),
        QStringLiteral("in"), QStringLiteral("not"), QStringLiteral("set"),
        QStringLiteral("setlocal"), QStringLiteral("shift"), QStringLiteral("start")
    };
    return values;
}

LineResult highlightCFamily(QStringView text, Language language, int previousState)
{
    LineResult result;
    result.state = NormalState;
    int i = 0;
    bool expectFunction = false;
    bool expectType = false;

    if (previousState == CBlockCommentState) {
        const int close = text.indexOf(QStringView(u"*/"));
        if (close < 0) {
            appendSpan(result, 0, text.size(), Role::Comment);
            result.state = CBlockCommentState;
            return result;
        }
        appendSpan(result, 0, close + 2, Role::Comment);
        i = close + 2;
    } else if (language == Language::JavaScript && previousState == JavaScriptTemplateState) {
        int close = i;
        while ((close = text.indexOf(QLatin1Char('`'), close)) >= 0) {
            int slashes = 0;
            for (int j = close - 1; j >= 0 && text.at(j) == QLatin1Char('\\'); --j)
                ++slashes;
            if ((slashes % 2) == 0)
                break;
            ++close;
        }
        if (close < 0) {
            appendSpan(result, 0, text.size(), Role::String);
            result.state = JavaScriptTemplateState;
            return result;
        }
        appendSpan(result, 0, close + 1, Role::String);
        i = close + 1;
    }

    const QSet<QString> &keywords = language == Language::C ? cKeywords()
                                      : language == Language::Cpp ? cppKeywords()
                                      : language == Language::CSharp ? cSharpKeywords()
                                                                     : javaScriptKeywords();

    while (i < text.size()) {
        if (i + 1 < text.size() && text.at(i) == QLatin1Char('/')
            && text.at(i + 1) == QLatin1Char('/')) {
            appendSpan(result, i, text.size(), Role::Comment);
            break;
        }
        if (i + 1 < text.size() && text.at(i) == QLatin1Char('/')
            && text.at(i + 1) == QLatin1Char('*')) {
            const int close = text.indexOf(QStringView(u"*/"), i + 2);
            if (close < 0) {
                appendSpan(result, i, text.size(), Role::Comment);
                result.state = CBlockCommentState;
                break;
            }
            appendSpan(result, i, close + 2, Role::Comment);
            i = close + 2;
            continue;
        }

        if (language != Language::JavaScript && text.at(i) == QLatin1Char('#')
            && text.left(i).trimmed().isEmpty()) {
            int directiveEnd = i + 1;
            while (directiveEnd < text.size() && isIdentifierPart(text.at(directiveEnd)))
                ++directiveEnd;
            appendSpan(result, i, directiveEnd, Role::Keyword);
            i = directiveEnd;
            continue;
        }

        if (language == Language::CSharp && i + 1 < text.size()
            && text.at(i) == QLatin1Char('@') && text.at(i + 1) == QLatin1Char('"')) {
            const int end = quotedEnd(text, i + 1, QLatin1Char('"'), true);
            appendSpan(result, i, end, Role::String);
            i = end;
            continue;
        }

        const QChar ch = text.at(i);
        if (ch == QLatin1Char('"') || ch == QLatin1Char('\'')) {
            const int end = quotedEnd(text, i, ch);
            appendSpan(result, i, end, Role::String);
            i = end;
            continue;
        }
        if (language == Language::JavaScript && ch == QLatin1Char('`')) {
            int end = quotedEnd(text, i, ch);
            appendSpan(result, i, end, Role::String);
            if (end == text.size() && (text.isEmpty() || text.back() != QLatin1Char('`')))
                result.state = JavaScriptTemplateState;
            i = end;
            continue;
        }
        if (ch.isDigit() || (ch == QLatin1Char('.') && i + 1 < text.size() && text.at(i + 1).isDigit())) {
            const int end = numberEnd(text, i);
            appendSpan(result, i, end, Role::Number);
            i = end;
            continue;
        }
        if (isIdentifierStart(ch)) {
            const int end = identifierEnd(text, i);
            const QStringView token = text.mid(i, end - i);
            Role role = Role::Variable;
            bool styled = false;
            if (contains(keywords, token)) {
                role = Role::Keyword;
                styled = true;
                expectFunction = token == QStringView(u"function");
                expectType = token == QStringView(u"class") || token == QStringView(u"struct")
                             || token == QStringView(u"interface") || token == QStringView(u"enum")
                             || token == QStringView(u"record");
            } else if (contains(cFamilyTypes(), token)
                       || (language == Language::JavaScript && contains(javaScriptTypes(), token))) {
                role = Role::Type;
                styled = true;
            } else if (expectFunction) {
                role = Role::Function;
                styled = true;
                expectFunction = false;
            } else if (expectType || (!token.isEmpty() && token.at(0).isUpper())) {
                role = Role::Type;
                styled = true;
                expectType = false;
            } else if (nextNonSpace(text, end) < text.size()
                       && text.at(nextNonSpace(text, end)) == QLatin1Char('(')) {
                role = Role::Function;
                styled = true;
            }
            if (styled)
                appendSpan(result, i, end, role);
            i = end;
            continue;
        }
        ++i;
    }
    return result;
}

bool isPythonStringPrefix(QStringView token)
{
    if (token.isEmpty() || token.size() > 3)
        return false;
    for (QChar ch : token) {
        const QChar lower = ch.toLower();
        if (lower != QLatin1Char('r') && lower != QLatin1Char('u')
            && lower != QLatin1Char('b') && lower != QLatin1Char('f')) {
            return false;
        }
    }
    return true;
}

LineResult highlightPython(QStringView text, int previousState)
{
    LineResult result;
    result.state = NormalState;
    int i = 0;
    bool expectFunction = false;
    bool expectType = false;

    const auto consumeTriple = [&](int start, QChar quote, int state,
                                   LineResult &target) -> int {
        const QString delimiter(3, quote);
        const int close = text.indexOf(delimiter, start);
        if (close < 0) {
            appendSpan(target, i, text.size(), Role::String);
            target.state = state;
            return int(text.size());
        }
        appendSpan(target, i, close + 3, Role::String);
        return close + 3;
    };

    if (previousState == PythonTripleSingleState)
        i = consumeTriple(0, QLatin1Char('\''), PythonTripleSingleState, result);
    else if (previousState == PythonTripleDoubleState)
        i = consumeTriple(0, QLatin1Char('"'), PythonTripleDoubleState, result);
    if (result.state != NormalState)
        return result;

    while (i < text.size()) {
        if (text.at(i) == QLatin1Char('#')) {
            appendSpan(result, i, text.size(), Role::Comment);
            break;
        }
        if (text.at(i) == QLatin1Char('@') && i + 1 < text.size() && isIdentifierStart(text.at(i + 1))) {
            const int end = identifierEnd(text, i + 1);
            appendSpan(result, i, end, Role::Function);
            i = end;
            continue;
        }

        int stringStart = i;
        QChar quote = text.at(i);
        if (isIdentifierStart(text.at(i))) {
            const int prefixEnd = identifierEnd(text, i);
            const QStringView prefix = text.mid(i, prefixEnd - i);
            if (prefixEnd < text.size() && isPythonStringPrefix(prefix)
                && (text.at(prefixEnd) == QLatin1Char('\'') || text.at(prefixEnd) == QLatin1Char('"'))) {
                quote = text.at(prefixEnd);
                stringStart = prefixEnd;
            }
        }
        if (quote == QLatin1Char('\'') || quote == QLatin1Char('"')) {
            const bool triple = stringStart + 2 < text.size()
                                && text.at(stringStart + 1) == quote
                                && text.at(stringStart + 2) == quote;
            if (triple) {
                const QString delimiter(3, quote);
                const int close = text.indexOf(delimiter, stringStart + 3);
                if (close < 0) {
                    appendSpan(result, i, text.size(), Role::String);
                    result.state = quote == QLatin1Char('\'') ? PythonTripleSingleState
                                                               : PythonTripleDoubleState;
                    break;
                }
                appendSpan(result, i, close + 3, Role::String);
                i = close + 3;
            } else {
                const int end = quotedEnd(text, stringStart, quote);
                appendSpan(result, i, end, Role::String);
                i = end;
            }
            continue;
        }
        if (text.at(i).isDigit() || (text.at(i) == QLatin1Char('.')
                                     && i + 1 < text.size() && text.at(i + 1).isDigit())) {
            const int end = numberEnd(text, i);
            appendSpan(result, i, end, Role::Number);
            i = end;
            continue;
        }
        if (isIdentifierStart(text.at(i))) {
            const int end = identifierEnd(text, i);
            const QStringView token = text.mid(i, end - i);
            if (contains(pythonKeywords(), token)) {
                appendSpan(result, i, end, Role::Keyword);
                expectFunction = token == QStringView(u"def");
                expectType = token == QStringView(u"class");
            } else if (contains(pythonBuiltins(), token)) {
                appendSpan(result, i, end, Role::Type);
            } else if (expectFunction) {
                appendSpan(result, i, end, Role::Function);
                expectFunction = false;
            } else if (expectType) {
                appendSpan(result, i, end, Role::Type);
                expectType = false;
            } else if (nextNonSpace(text, end) < text.size()
                       && text.at(nextNonSpace(text, end)) == QLatin1Char('(')) {
                appendSpan(result, i, end, Role::Function);
            }
            i = end;
            continue;
        }
        ++i;
    }
    return result;
}

int variableEnd(QStringView text, int start)
{
    if (start + 1 >= text.size())
        return start + 1;
    if (text.at(start + 1) == QLatin1Char('{')) {
        const int close = text.indexOf(QLatin1Char('}'), start + 2);
        return close < 0 ? text.size() : close + 1;
    }
    int i = start + 1;
    while (i < text.size()) {
        const QChar ch = text.at(i);
        if (isIdentifierPart(ch) || ch == QLatin1Char(':') || ch == QLatin1Char('?')
            || ch == QLatin1Char('#') || ch == QLatin1Char('@') || ch == QLatin1Char('*')) {
            ++i;
        } else {
            break;
        }
    }
    return i;
}

LineResult highlightPowerShell(QStringView text, int previousState)
{
    LineResult result;
    result.state = NormalState;
    int i = 0;

    if (previousState == PowerShellBlockCommentState) {
        const int close = text.indexOf(QStringView(u"#>"));
        if (close < 0) {
            appendSpan(result, 0, text.size(), Role::Comment);
            result.state = PowerShellBlockCommentState;
            return result;
        }
        appendSpan(result, 0, close + 2, Role::Comment);
        i = close + 2;
    } else if (previousState == PowerShellHereSingleState
               || previousState == PowerShellHereDoubleState) {
        const QStringView close = previousState == PowerShellHereSingleState
                                      ? QStringView(u"'@") : QStringView(u"\"@");
        appendSpan(result, 0, text.size(), Role::String);
        if (!text.trimmed().startsWith(close))
            result.state = previousState;
        return result;
    }

    bool commandPosition = true;
    while (i < text.size()) {
        if (i + 1 < text.size() && text.at(i) == QLatin1Char('<')
            && text.at(i + 1) == QLatin1Char('#')) {
            const int close = text.indexOf(QStringView(u"#>"), i + 2);
            if (close < 0) {
                appendSpan(result, i, text.size(), Role::Comment);
                result.state = PowerShellBlockCommentState;
                break;
            }
            appendSpan(result, i, close + 2, Role::Comment);
            i = close + 2;
            continue;
        }
        if (text.at(i) == QLatin1Char('#')) {
            appendSpan(result, i, text.size(), Role::Comment);
            break;
        }
        if (i + 1 < text.size() && text.at(i) == QLatin1Char('@')
            && (text.at(i + 1) == QLatin1Char('\'') || text.at(i + 1) == QLatin1Char('"'))
            && text.mid(i + 2).trimmed().isEmpty()) {
            appendSpan(result, i, text.size(), Role::String);
            result.state = text.at(i + 1) == QLatin1Char('\'') ? PowerShellHereSingleState
                                                                : PowerShellHereDoubleState;
            break;
        }
        if (text.at(i) == QLatin1Char('\'') || text.at(i) == QLatin1Char('"')) {
            const int end = quotedEnd(text, i, text.at(i), text.at(i) == QLatin1Char('\''));
            appendSpan(result, i, end, Role::String);
            i = end;
            commandPosition = false;
            continue;
        }
        if (text.at(i) == QLatin1Char('$')) {
            const int end = variableEnd(text, i);
            appendSpan(result, i, end, Role::Variable);
            i = end;
            commandPosition = false;
            continue;
        }
        if (text.at(i) == QLatin1Char('[') && i + 1 < text.size()
            && isIdentifierStart(text.at(i + 1))) {
            const int nameEnd = identifierEnd(text, i + 1);
            if (nameEnd < text.size() && text.at(nameEnd) == QLatin1Char(']')) {
                appendSpan(result, i, nameEnd + 1, Role::Type);
                i = nameEnd + 1;
                continue;
            }
        }
        if (text.at(i).isDigit()) {
            const int end = numberEnd(text, i);
            appendSpan(result, i, end, Role::Number);
            i = end;
            commandPosition = false;
            continue;
        }
        if (text.at(i) == QLatin1Char('-') && i + 1 < text.size() && text.at(i + 1).isLetter()) {
            int end = i + 2;
            while (end < text.size() && (isIdentifierPart(text.at(end)) || text.at(end) == QLatin1Char('-')))
                ++end;
            appendSpan(result, i, end, Role::Variable);
            i = end;
            continue;
        }
        if (isIdentifierStart(text.at(i))) {
            int end = i + 1;
            while (end < text.size() && (isIdentifierPart(text.at(end)) || text.at(end) == QLatin1Char('-')))
                ++end;
            const QStringView token = text.mid(i, end - i);
            if (contains(powerShellKeywords(), token, Qt::CaseInsensitive))
                appendSpan(result, i, end, Role::Keyword);
            else if (token.contains(QLatin1Char('-')) || commandPosition)
                appendSpan(result, i, end, Role::Function);
            commandPosition = token.compare(QStringView(u"function"), Qt::CaseInsensitive) == 0;
            i = end;
            continue;
        }
        if (text.at(i) == QLatin1Char('|') || text.at(i) == QLatin1Char(';')
            || text.at(i) == QLatin1Char('&'))
            commandPosition = true;
        ++i;
    }
    return result;
}

LineResult highlightShell(QStringView text, int previousState)
{
    LineResult result;
    result.state = NormalState;
    int i = 0;
    if (previousState == ShellSingleQuoteState || previousState == ShellDoubleQuoteState) {
        const QChar quote = previousState == ShellSingleQuoteState ? QLatin1Char('\'') : QLatin1Char('"');
        const int end = quotedEnd(text, -1, quote);
        appendSpan(result, 0, end, Role::String);
        if (end == text.size() && (text.isEmpty() || text.back() != quote)) {
            result.state = previousState;
            return result;
        }
        i = end;
    }

    bool commandPosition = true;
    while (i < text.size()) {
        const QChar ch = text.at(i);
        if (ch == QLatin1Char('#')
            && (i == 0 || text.at(i - 1).isSpace() || text.at(i - 1) == QLatin1Char(';'))) {
            appendSpan(result, i, text.size(), Role::Comment);
            break;
        }
        if (ch == QLatin1Char('\'') || ch == QLatin1Char('"')) {
            const int end = quotedEnd(text, i, ch);
            appendSpan(result, i, end, Role::String);
            if (end == text.size() && (text.isEmpty() || text.back() != ch))
                result.state = ch == QLatin1Char('\'') ? ShellSingleQuoteState : ShellDoubleQuoteState;
            i = end;
            commandPosition = false;
            continue;
        }
        if (ch == QLatin1Char('$')) {
            const int end = variableEnd(text, i);
            appendSpan(result, i, end, Role::Variable);
            i = end;
            commandPosition = false;
            continue;
        }
        if (ch.isDigit()) {
            const int end = numberEnd(text, i);
            appendSpan(result, i, end, Role::Number);
            i = end;
            commandPosition = false;
            continue;
        }
        if (ch == QLatin1Char('-') && i + 1 < text.size() && text.at(i + 1) != QLatin1Char(' ')) {
            int end = i + 1;
            while (end < text.size() && !text.at(end).isSpace() && text.at(end) != QLatin1Char('|')
                   && text.at(end) != QLatin1Char(';'))
                ++end;
            appendSpan(result, i, end, Role::Variable);
            i = end;
            continue;
        }
        if (isIdentifierStart(ch)) {
            int end = i + 1;
            while (end < text.size() && (isIdentifierPart(text.at(end)) || text.at(end) == QLatin1Char('-')))
                ++end;
            const QStringView token = text.mid(i, end - i);
            if (contains(shellKeywords(), token))
                appendSpan(result, i, end, Role::Keyword);
            else if (commandPosition)
                appendSpan(result, i, end, Role::Function);
            const QString lower = token.toString().toLower();
            commandPosition = lower == QStringLiteral("sudo") || lower == QStringLiteral("env")
                              || lower == QStringLiteral("command") || lower == QStringLiteral("nohup");
            i = end;
            continue;
        }
        if (ch == QLatin1Char('|') || ch == QLatin1Char(';') || ch == QLatin1Char('&'))
            commandPosition = true;
        ++i;
    }
    return result;
}

LineResult highlightBatch(QStringView text)
{
    LineResult result;
    const QString trimmed = text.trimmed().toString();
    if (trimmed.startsWith(QStringLiteral("::"))
        || trimmed.startsWith(QStringLiteral("rem "), Qt::CaseInsensitive)
        || trimmed.compare(QStringLiteral("rem"), Qt::CaseInsensitive) == 0) {
        appendSpan(result, 0, text.size(), Role::Comment);
        return result;
    }

    int i = 0;
    bool commandPosition = true;
    while (i < text.size()) {
        if (text.at(i) == QLatin1Char('%')) {
            int end = text.indexOf(QLatin1Char('%'), i + 1);
            if (end < 0) {
                end = i + 1;
                while (end < text.size() && !text.at(end).isSpace())
                    ++end;
            } else {
                ++end;
            }
            appendSpan(result, i, end, Role::Variable);
            i = end;
            continue;
        }
        if (text.at(i) == QLatin1Char('!')) {
            const int close = text.indexOf(QLatin1Char('!'), i + 1);
            if (close > i) {
                appendSpan(result, i, close + 1, Role::Variable);
                i = close + 1;
                continue;
            }
        }
        if (text.at(i) == QLatin1Char('"')) {
            const int end = quotedEnd(text, i, QLatin1Char('"'));
            appendSpan(result, i, end, Role::String);
            i = end;
            commandPosition = false;
            continue;
        }
        if (text.at(i) == QLatin1Char('/') && i + 1 < text.size()
            && !text.at(i + 1).isSpace()) {
            int end = i + 1;
            while (end < text.size() && !text.at(end).isSpace()
                   && text.at(end) != QLatin1Char('|') && text.at(end) != QLatin1Char('&')) {
                ++end;
            }
            appendSpan(result, i, end, Role::Variable);
            i = end;
            continue;
        }
        if (text.at(i).isDigit()) {
            const int end = numberEnd(text, i);
            appendSpan(result, i, end, Role::Number);
            i = end;
            continue;
        }
        if (isIdentifierStart(text.at(i))) {
            int end = i + 1;
            while (end < text.size() && (isIdentifierPart(text.at(end)) || text.at(end) == QLatin1Char('-')))
                ++end;
            const QStringView token = text.mid(i, end - i);
            if (contains(batchKeywords(), token, Qt::CaseInsensitive))
                appendSpan(result, i, end, Role::Keyword);
            else if (commandPosition)
                appendSpan(result, i, end, Role::Function);
            commandPosition = false;
            i = end;
            continue;
        }
        if (text.at(i) == QLatin1Char('|') || text.at(i) == QLatin1Char('&'))
            commandPosition = true;
        ++i;
    }
    return result;
}

}

QColor Palette::color(Role role) const
{
    switch (role) {
    case Role::Keyword: return keyword;
    case Role::Type: return type;
    case Role::String: return string;
    case Role::Number: return number;
    case Role::Comment: return comment;
    case Role::Function: return function;
    case Role::Variable: return variable;
    }
    return {};
}

Language languageFromInfo(QStringView info)
{
    const QStringView trimmed = info.trimmed();
    qsizetype end = 0;
    while (end < trimmed.size() && !trimmed.at(end).isSpace())
        ++end;
    QString name = trimmed.left(end).toString().toLower();
    if (name.startsWith(QStringLiteral("{.")) && name.endsWith(QLatin1Char('}')))
        name = name.mid(2, name.size() - 3);
    else if (name.startsWith(QLatin1Char('.')))
        name.remove(0, 1);

    if (name == QStringLiteral("c"))
        return Language::C;
    if (name == QStringLiteral("cpp") || name == QStringLiteral("c++")
        || name == QStringLiteral("cc") || name == QStringLiteral("cxx")
        || name == QStringLiteral("hpp") || name == QStringLiteral("hxx"))
        return Language::Cpp;
    if (name == QStringLiteral("csharp") || name == QStringLiteral("cs")
        || name == QStringLiteral("c#"))
        return Language::CSharp;
    if (name == QStringLiteral("python") || name == QStringLiteral("py"))
        return Language::Python;
    if (name == QStringLiteral("javascript") || name == QStringLiteral("js")
        || name == QStringLiteral("jsx") || name == QStringLiteral("node"))
        return Language::JavaScript;
    if (name == QStringLiteral("powershell") || name == QStringLiteral("ps1")
        || name == QStringLiteral("pwsh"))
        return Language::PowerShell;
    if (name == QStringLiteral("shell") || name == QStringLiteral("sh")
        || name == QStringLiteral("bash") || name == QStringLiteral("zsh")
        || name == QStringLiteral("linux") || name == QStringLiteral("linux-terminal"))
        return Language::Shell;
    if (name == QStringLiteral("terminal") || name == QStringLiteral("console")
        || name == QStringLiteral("cmd") || name == QStringLiteral("bat")
        || name == QStringLiteral("batch") || name == QStringLiteral("dos")
        || name == QStringLiteral("windows-terminal"))
        return Language::Batch;
    return Language::Plain;
}

LineResult highlightLine(QStringView text, Language language, int previousState)
{
    switch (language) {
    case Language::C:
    case Language::Cpp:
    case Language::CSharp:
    case Language::JavaScript:
        return highlightCFamily(text, language, previousState);
    case Language::Python:
        return highlightPython(text, previousState);
    case Language::PowerShell:
        return highlightPowerShell(text, previousState);
    case Language::Shell:
        return highlightShell(text, previousState);
    case Language::Batch:
        return highlightBatch(text);
    case Language::Plain:
        return {};
    }
    return {};
}

}
