/**
 * \file   WordFinder.h
 * \brief
 */


#pragma once

#include "ui_WordFinder.h"

#include "../Config.h"
#include "../QtLib/Common.h"
#include "SqlRelationalTableModelEx.h"
//-------------------------------------------------------------------------------------------------
class WordFinder :
    public QDialog
{
public:
    Ui::UiWordFinder ui;

                    WordFinder(QWidget *parent, qtlib::SqlRelationalTableModelEx *tableModel);
    virtual        ~WordFinder();

private:
    qtlib::SqlRelationalTableModelEx *_model;

    void            _construct();
    void            _destruct();

    void            _resetAll();
    void            _saveAll();

    Q_OBJECT
    Q_DISABLE_COPY(WordFinder)

private Q_SLOTS:
    void            bbxButtons_OnClicked(QAbstractButton *button);
};
//-------------------------------------------------------------------------------------------------
