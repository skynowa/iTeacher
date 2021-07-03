/**
 * \file  Hint.h
 * \brief
 */


#pragma once

#include "../Config.h"
#include <QSqlDatabase>
//-------------------------------------------------------------------------------------------------
class Hint :
    public QObject
{
public:
    enum class Type
    {
        TrayIcon   = 1,
        MessageBox = 2,
        ToolTip    = 3
    };

    explicit Hint(QObject *parent, const Type type, const QSqlDatabase &database);

    void show() const;

signals:

public slots:

private:
    Q_OBJECT

    const Type          _type {};
    const QSqlDatabase &_database;
};
//-------------------------------------------------------------------------------------------------
