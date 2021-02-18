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
    Q_OBJECT
    Q_DISABLE_COPY(CenteredCheckBox)

public:
    explicit   CenteredCheckBox(QWidget *parent = nullptr);

    QCheckBox *checkBox() const;

private:
    QCheckBox *_checkBox;
};
//-------------------------------------------------------------------------------------------------
