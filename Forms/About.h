/**
 * \file   About.h
 * \brief  about widget
 */


#ifndef QtLib_AboutH
#define QtLib_AboutH
//-------------------------------------------------------------------------------------------------
#include "ui_About.h"
//-------------------------------------------------------------------------------------------------
class About :
    public QDialog
{
    Q_OBJECT

public:
    Ui::UiAbout ui;

    explicit    About(QWidget *parent);

private:
    void        _construct();
    void        _destruct();
    void        _initMain();

    // slots
    void        slot_bbxButtons_onClicked(QAbstractButton *button);
};
//-------------------------------------------------------------------------------------------------
#endif // QtLib_AboutH
