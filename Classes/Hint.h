/**
 * \file  Hint.h
 * \brief
 */


#pragma once

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
    explicit Hint(QObject *parent, const Type type, const qtlib::SqlRelationalTableModelEx &model);

    Q_DISABLE_COPY(Hint)

    static Hint trayIcon(QObject *parent, const qtlib::SqlRelationalTableModelEx &model);
    static Hint messageBox(const qtlib::SqlRelationalTableModelEx &model);
    static Hint toolTip(const qtlib::SqlRelationalTableModelEx &model);
///@}

    void show() const;

private:
    Q_OBJECT

    static constexpr int _timeoutMs {30 * 1000};

    const Type                              _type {};
    const qtlib::SqlRelationalTableModelEx &_model;

    void _audioFileDownload(cQString &text, cQString &langCode, cQString &audioPath) const;
            ///< Download as file
};
//-------------------------------------------------------------------------------------------------

/// Вы просматривали
