/**
 * \file  Trainer.h
 * \brief
 */


#pragma once

#include "../Config.h"
#include <xLib/Core/Core.h>
#include "../QtLib/Common.h"
#include <Classes/SqliteDb.h>

#include "ui_Trainer.h"
//-------------------------------------------------------------------------------------------------
class WordEditor;

class Trainer :
    public QMainWindow
{
public:
             Trainer(QWidget *parent, Qt::WindowFlags flags = Qt::Widget);
    virtual ~Trainer() = default;

private:
    Q_OBJECT
    Q_DISABLE_COPY(Trainer)

    Ui::UiTrainer             _ui;
    std::unique_ptr<SqliteDb> _db;

    void _initMain();
    void _initActions();

private Q_SLOTS:
    // group "Main"
    void randomRow() const;
    void exit();
};
//-------------------------------------------------------------------------------------------------
