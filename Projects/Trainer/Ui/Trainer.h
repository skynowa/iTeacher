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
class Trainer :
    public QMainWindow
{
public:
    Trainer(QWidget *parent, Qt::WindowFlags flags = Qt::Widget);
    ~Trainer() final;

    Q_DISABLE_COPY(Trainer)

signals:
    void closed();

protected:
    bool eventFilter(QObject *object, QEvent *event) final;

private:
    Q_OBJECT

    Ui::UiTrainer             _ui;
    std::unique_ptr<SqliteDb> _sqliteDb;

    void _initMain();
    void _initActions();

    // settings
    void _settingsLoad();
    void _settingsSave();

private Q_SLOTS:
    // group "Main"
    void randomRow() const;
    void learn() const;
    void mark() const;
    // void xxxxx() const;
    void exit();
};
//-------------------------------------------------------------------------------------------------
