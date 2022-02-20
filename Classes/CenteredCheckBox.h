/**
 * \file   CenteredCheckBox.h
 * \brief
 */


#pragma once

#include <QWidget>
//-------------------------------------------------------------------------------------------------
class QCheckBox;

class CenteredCheckBox :
    public QWidget
{
public:
///@name ctors, dtor
///@{
    explicit CenteredCheckBox(QWidget *parent);

    Q_DISABLE_COPY(CenteredCheckBox);
///@}

    QCheckBox *checkBox() const;

private:
    Q_OBJECT

    QCheckBox *_checkBox {};
};
//-------------------------------------------------------------------------------------------------
