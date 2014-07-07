/**
 * \file   WordFinder.h
 * \brief
 */


#ifndef iTeacher_CWordFinderH
#define iTeacher_CWordFinderH
//-------------------------------------------------------------------------------------------------
#include "ui_WordFinder.h"

#include "../Config.h"
#include "../QtLib/Common.h"
//-------------------------------------------------------------------------------------------------
class WordFinder :
    public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY(WordFinder)

public:
    Ui::UiWordFinder m_Ui;

                    WordFinder(QWidget *parent, QSqlTableModel *tableModel);
    virtual        ~WordFinder();

private:
    QSqlTableModel *_model;

    void            _construct();
    void            _destruct();

    void            _resetAll();
    void            _saveAll();

private Q_SLOTS:
    void            slot_bbxButtons_OnClicked(QAbstractButton *button);
};
//-------------------------------------------------------------------------------------------------
#endif // iTeacher_CWordFinderH
