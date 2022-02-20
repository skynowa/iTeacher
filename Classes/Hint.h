/**
 * \file  Hint.h
 * \brief
 */


#pragma once

#include "Config.h"
#include <QtLib/Application.h>
#include <QSqlTableModel>
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
    explicit Hint(QObject *parent, const Type type, cQSqlTableModel &model);

    Q_DISABLE_COPY(Hint)

    static Hint trayIcon(QObject *parent, cQSqlTableModel &model);
    static Hint messageBox(cQSqlTableModel &model);
    static Hint toolTip(cQSqlTableModel &model);
///@}

    void show() const;

private:
    Q_OBJECT

    static constexpr int _timeoutMs {3 * 1000};

    const Type       _type {};
    cQSqlTableModel &_model;
};
//-------------------------------------------------------------------------------------------------
