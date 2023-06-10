
#include "doc.h"
#include "luascript.h"

LuaScript::LuaScript(Doc* doc) : Function(doc, Function::LuaScriptType)
{
    (void)doc;


}

LuaScript::~LuaScript()
{

}

QIcon LuaScript::getIcon() const
{
    return QIcon(":/script.png");
}

Function* LuaScript::createCopy(Doc* doc, bool addToDoc)
{
    Function *copy = new LuaScript(doc);
    if (copy->copyFrom(this) == false)
    {
        delete copy;
        copy = NULL;
    }
    if (addToDoc == true && doc->addFunction(copy) == false)
    {
        delete copy;
        copy = NULL;
    }

    return copy;
}

bool LuaScript::copyFrom(const Function* function)
{
    const LuaScript* script = qobject_cast<const LuaScript*> (function);
    if (script == NULL)
        return false;

    setData(script->data());

    return Function::copyFrom(function);

}
bool LuaScript::loadXML(QXmlStreamReader &root)
{
    (void)root;
    return false;
}

bool LuaScript::saveXML(QXmlStreamWriter *doc)
{
    (void)doc;
    return false;
}

QString LuaScript::data() const
{
    return m_data;
}

bool LuaScript::setData(const QString &str)
{
    if (str.isEmpty())
    {
        return false;
    }

    m_data = str;
    return true;
}


/*
bool LuaScript::setData(const QString& str)
{

}
QString LuaScript::data() const
{

}
bool LuaScript::appendData(const QString& str)
{

}

*/