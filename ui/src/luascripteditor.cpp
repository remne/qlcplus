#include "luascripteditor.h"
#include <iostream>

#include <QPainter>
#include <QTextBlock>
#include <QCompleter>
#include <QStringListModel>
#include <QAbstractItemView>
#include <QScrollBar>

#include "luascript.h"

LuaScriptEditor::LuaScriptEditor(QWidget *parent, LuaScript *luaScript, Doc* doc) : QPlainTextEdit(parent)
{
   (void)luaScript;
   (void)doc;

    m_lineNumberArea = new LineNumberArea(this);

    connect(this, &LuaScriptEditor::blockCountChanged, this, &LuaScriptEditor::updateLineNumberAreaWidth);
    connect(this, &LuaScriptEditor::updateRequest, this, &LuaScriptEditor::updateLineNumberArea);
    connect(this, &LuaScriptEditor::cursorPositionChanged, this, &LuaScriptEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    setupCodeCompleter();

    this->setPlainText(luaScript->data());
}

LuaScriptEditor::~LuaScriptEditor()
{

}

void LuaScriptEditor::setupCodeCompleter()
{

    QStringList words = {"function", "func"};

    auto completer = new QCompleter(this);
    // completer->setModel(modelFromFile(":/resources/wordlist.txt"));
    auto list = new QStringListModel(words, completer);
    completer->setModel(static_cast<QAbstractItemModel *>(list));
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);

    setCompleter(completer);
}

int LuaScriptEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void LuaScriptEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void LuaScriptEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void LuaScriptEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void LuaScriptEditor::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Tab && m_completer && !m_completer->popup()->isVisible())
    {
        this->insertPlainText("  ");
        e->accept();
        return;
    }

    if (m_completer && m_completer->popup()->isVisible())
    {
        // The following keys are forwarded by the completer to the widget
        switch (e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
        default:
            break;
        }
    }

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_Space); // CTRL+space
    if (!m_completer || !isShortcut)                                                                     // do not process the shortcut when we have a completer
        QPlainTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) ||
                             e->modifiers().testFlag(Qt::ShiftModifier);
    if (!m_completer || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 3 || eow.contains(e->text().right(1))))
    {
        m_completer->popup()->hide();
        return;
    }

    if (completionPrefix != m_completer->completionPrefix())
    {
        m_completer->setCompletionPrefix(completionPrefix);
        m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(m_completer->popup()->sizeHintForColumn(0) + m_completer->popup()->verticalScrollBar()->sizeHint().width());
    m_completer->complete(cr); // popup it up!
}

void LuaScriptEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::blue).lighter(190);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void LuaScriptEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, m_lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void LuaScriptEditor::setCompleter(QCompleter *completer)
{
    if (m_completer)
        m_completer->disconnect(this);

    m_completer = completer;

    if (!m_completer)
        return;

    m_completer->setWidget(this);
    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(m_completer, QOverload<const QString &>::of(&QCompleter::activated),
                     this, &LuaScriptEditor::insertCompletion);
}

QCompleter *LuaScriptEditor::completer() const
{
    return m_completer;
}

void LuaScriptEditor::insertCompletion(const QString &completion)
{
    if (m_completer->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - m_completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString LuaScriptEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void LuaScriptEditor::focusInEvent(QFocusEvent *e)
{
    if (m_completer)
        m_completer->setWidget(this);

    QPlainTextEdit::focusInEvent(e);
}
