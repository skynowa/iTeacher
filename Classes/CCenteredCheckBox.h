/**
 * \file   CCenteredCheckBox.h
 * \brief
 */


#ifndef iTeacher_CCenteredCheckBox_H
#define iTeacher_CCenteredCheckBox_H
//-------------------------------------------------------------------------------------------------
#include <QWidget>
//-------------------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------------------
#endif // iTeacher_CCenteredCheckBox_H
