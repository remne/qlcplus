#pragma once

#include <QWidget>

class Doc;
class LuaScript;

class LuaScriptEditor : public QWidget
{
    Q_OBJECT

public:
    LuaScriptEditor(QWidget *parent, LuaScript *luaScript, Doc* doc);
    virtual ~LuaScriptEditor();


private:
    LuaScript *m_script;
    Doc *m_doc;
};
