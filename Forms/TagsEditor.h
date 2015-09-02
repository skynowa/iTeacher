/**
 * \file   TagsEditor.h
 * \brief
 */


#pragma once

//-------------------------------------------------------------------------------------------------
#include "ui_TagsEditor.h"

#include "../Config.h"
#include "../QtLib/Common.h"

#include "Main.h"
//-------------------------------------------------------------------------------------------------
class TagsEditor :
    public QDialog
{
public:
    Ui::UiTagsEditor ui;

                     TagsEditor(QWidget *parent, const QSqlDatabase &db);
    virtual         ~TagsEditor();

private:
    const QSqlDatabase &_db;
    QSqlTableModel      _model;
    qtlib::SqlNavigator _sqlNavigator;

    void             _construct();
    void             _destruct();

    Q_OBJECT
    Q_DISABLE_COPY(TagsEditor)

private Q_SLOTS:
    void             Insert();
    void             Remove();
};
//-------------------------------------------------------------------------------------------------
