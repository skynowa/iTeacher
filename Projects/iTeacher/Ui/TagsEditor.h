/**
 * \file   TagsEditor.h
 * \brief
 */


#pragma once

//-------------------------------------------------------------------------------------------------
#include "ui_TagsEditor.h"

#include "../Config.h"
#include "../QtLib/Common.h"
#include "SqlRelationalTableModelEx.h"

#include "Main.h"
//-------------------------------------------------------------------------------------------------
class TagsEditor :
    public QDialog
{
public:
    Ui::UiTagsEditor ui;

    TagsEditor(QWidget *parent, const QSqlDatabase &db);
    ~TagsEditor() final;

    Q_DISABLE_COPY(TagsEditor)

private:
    Q_OBJECT

    const QSqlDatabase               &_db;
    qtlib::SqlRelationalTableModelEx  _model;
    qtlib::SqlNavigator               _sqlNavigator;

    void _construct();
    void _destruct();

private Q_SLOTS:
    void insert();
    void remove();
};
//-------------------------------------------------------------------------------------------------
