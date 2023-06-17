#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QUrl>
#include <QDebug>
#include <iostream>


#include "doc.h"
#include "luascript.h"

#define KXMLQLCScriptCommand QString("Command")

LuaScript::LuaScript(Doc* doc) : Function(doc, Function::LuaScriptType)
{

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
    std::cout << "CREATE COPY" << std::endl;

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
    std::cout << "COPY FROM" << std::endl;

    const LuaScript* script = qobject_cast<const LuaScript*> (function);
    if (script == NULL)
        return false;

    setData(script->data());

    return Function::copyFrom(function);
}

bool LuaScript::loadXML(QXmlStreamReader &root)
{
    std::cout << "LOAD XML" << std::endl;

    if (root.name() != KXMLQLCFunction)
    {
        qWarning() << Q_FUNC_INFO << "Function node not found";
        return false;
    }

    QXmlStreamAttributes attrs = root.attributes();

    if (attrs.value(KXMLQLCFunctionType).toString() != typeToString(Function::ScriptType))
    {
        qWarning() << Q_FUNC_INFO << root.attributes().value(KXMLQLCFunctionType).toString()
                   << "is not a script";
        return false;
    }

    /* Load script contents */
    while (root.readNextStartElement())
    {
        if (root.name() == KXMLQLCFunctionSpeed)
        {
            //loadXMLSpeed(root);
        }
        else if (root.name() == KXMLQLCFunctionDirection)
        {
            //loadXMLDirection(root);
        }
        else if (root.name() == KXMLQLCFunctionRunOrder)
        {
            //loadXMLRunOrder(root);
        }
        else if (root.name() == KXMLQLCScriptCommand)
        {
            m_data += root.readElementText().toUtf8();

        }
        else
        {
            qWarning() << Q_FUNC_INFO << "Unknown script tag:" << root.name();
            root.skipCurrentElement();
        }
    }

    return true;
}

bool LuaScript::saveXML(QXmlStreamWriter *doc)
{
    std::cout << "SAVE XML" << std::endl;

    doc->writeStartElement(KXMLQLCFunction);

    saveXMLCommon(doc);
    saveXMLSpeed(doc);
    saveXMLDirection(doc);
    saveXMLRunOrder(doc);

    doc->writeTextElement(KXMLQLCScriptCommand, m_data);
    doc->writeEndElement();
    return true;
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


void LuaScript::preRun(MasterTimer *timer)
{
    (void)timer;
    std::cout << "PRE RUN" << std::endl;
}

void LuaScript::write(MasterTimer *timer, QList<Universe*> universes)
{
    (void)timer;
    (void)universes;
    std::cout << "WRITE" << std::endl;
}

void LuaScript::postRun(MasterTimer *timer, QList<Universe*> universes)
{
    (void)timer;
    (void)universes;
    std::cout << "POSTRUN" << std::endl;
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
