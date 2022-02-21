/**
 * \file  Hint.h
 * \brief
 */


#pragma once

#include "Config.h"
#include <QtLib/Application.h>
#include <QtLib/Db/SqlRelationalTableModelEx.h>
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
    explicit Hint(QObject *parent, const Type type, const QSqlDatabase *db,
                const qtlib::SqlRelationalTableModelEx &model);

    Q_DISABLE_COPY(Hint)

    static Hint trayIcon(QObject *parent, QSqlDatabase *db, const qtlib::SqlRelationalTableModelEx &model);
    static Hint messageBox( QSqlDatabase *db, const qtlib::SqlRelationalTableModelEx &model);
    static Hint toolTip( QSqlDatabase *db, const qtlib::SqlRelationalTableModelEx &model);
///@}

    void show() const;

private:
    Q_OBJECT

    static constexpr int _timeoutMs {3 * 1000};

    const Type                              _type {};
    const QSqlDatabase                     *_db {};
    const qtlib::SqlRelationalTableModelEx &_model;
};
//-------------------------------------------------------------------------------------------------
