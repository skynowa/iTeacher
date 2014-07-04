/**
 * \file   CenteredCheckBox.h
 * \brief
 */


#ifndef iTeacher_CenteredCheckBox_H
#define iTeacher_CenteredCheckBox_H
//-------------------------------------------------------------------------------------------------
#include <QWidget>
//-------------------------------------------------------------------------------------------------
class QCheckBox;

class CenteredCheckBox :
    public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(CenteredCheckBox)

public:
    explicit   CenteredCheckBox(QWidget *parent = Q_NULLPTR);

    QCheckBox *checkBox() const;

private:
    QCheckBox *_checkBox;
};
//-------------------------------------------------------------------------------------------------
#endif // iTeacher_CenteredCheckBox_H
