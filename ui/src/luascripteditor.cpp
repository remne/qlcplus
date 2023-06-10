#include "luascripteditor.h"

#include <QLabel>

LuaScriptEditor::LuaScriptEditor(QWidget *parent, LuaScript *luaScript, Doc* doc)
{
    (void)parent;
    (void)luaScript;
    (void)doc;

    auto label = new QLabel(this);
    label->setText("HEJSAN!");
}

LuaScriptEditor::~LuaScriptEditor()
{

}

