#pragma once

#include <QtCore/QMetaObject>
#include <QtCore/QString>

#include "function.h"

class MasterTimer;
class Universe;
class Doc;

/**
 * The LuaScript class inherites from legacy script class
 * to keep most of ui parts the same.
 * The engine is exchanged to a Lua based one, and is executed
 * by a separate thread for each processed script
*/
class LuaScript : public Function
{
    Q_OBJECT

public:
    LuaScript(Doc* doc);
    virtual ~LuaScript();

    virtual QIcon getIcon() const override;
    // quint32 totalDuration()

    Function* createCopy(Doc* doc, bool addToDoc = true) override;
    bool copyFrom(const Function* function) override;
    bool loadXML(QXmlStreamReader &root) override;
    bool saveXML(QXmlStreamWriter *doc) override;

/*
    bool setData(const QString& str);
    bool appendData(const QString& str);
    QString data() const;
*/

public:
    QString data() const;
private:
    QString m_data;

private:
    bool setData(const QString &str);
};
