#pragma once

#include <QPlainTextEdit>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class LineNumberArea;
class QCompleter;

// QLC classes
class Doc;
class LuaScript;

class LuaScriptEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    LuaScriptEditor(QWidget *parent, LuaScript *luaScript, Doc* doc);
    virtual ~LuaScriptEditor();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();


    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

    void insertCompletion(const QString &completion);

private:
    QString textUnderCursor() const;
    void setupCodeCompleter();

private:
    QCompleter *m_completer = nullptr;
    QWidget *m_lineNumberArea;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(LuaScriptEditor *editor) : QWidget(editor), luaScriptEditor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(luaScriptEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        luaScriptEditor->lineNumberAreaPaintEvent(event);
    }

private:
    LuaScriptEditor *luaScriptEditor;
};
