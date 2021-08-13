/**
 * \file  Hint.h
 * \brief
 */


#pragma once

#include "../Config.h"
#include <QSqlDatabase>
#include <QSize>
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

///@name ctors, dtor
///@{
    Hint() = delete;
    explicit Hint(QObject *parent, const Type type, const QSqlDatabase &database);

    Q_DISABLE_COPY(Hint);

    static Hint trayIcon(QObject *parent, const QSqlDatabase &database);
    static Hint messageBox(QObject *parent, const QSqlDatabase &database);
    static Hint toolTip(QObject *parent, const QSqlDatabase &database);
///@}

    void show() const;

signals:

public slots:

private:
    Q_OBJECT

    const int           _timeoutMs {3000};

    const Type          _type {};
    const QSqlDatabase &_database;
};
//-------------------------------------------------------------------------------------------------
