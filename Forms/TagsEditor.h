/**
 * \file   TagsEditor.h
 * \brief
 */


#ifndef iTeacher_TagsEditorH
#define iTeacher_TagsEditorH
//-------------------------------------------------------------------------------------------------
#include "ui_TagsEditor.h"

#include "../Config.h"
#include "../QtLib/Common.h"

#include "Main.h"
//-------------------------------------------------------------------------------------------------
class TagsEditor :
    public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(TagsEditor)

public:
    Ui::UiTagsEditor ui;

                     TagsEditor(QWidget *parent, const QSqlDatabase &db);
    virtual         ~TagsEditor();

private:
    const QSqlDatabase &_db;
    QSqlTableModel   _model;

    void             _construct();
    void             _destruct();

private Q_SLOTS:

};
//-------------------------------------------------------------------------------------------------
#endif // iTeacher_TagsEditorH
