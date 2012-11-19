/**
 * \file   CCenteredCheckBox.h
 * \brief
 */


#ifndef CCenteredCheckBox_H
#define CCenteredCheckBox_H
//---------------------------------------------------------------------------
#include <QWidget>
//---------------------------------------------------------------------------
class QCheckBox;

class CCenteredCheckBox :
    public QWidget
{
        Q_OBJECT

    public:
        explicit   CCenteredCheckBox(QWidget *parent = 0);

        QCheckBox *checkBox         () const;

    private:
        QCheckBox *_m_checkBox;
};
//---------------------------------------------------------------------------
#endif // CCenteredCheckBox_H
