/**
 * \file   CCenteredCheckBox.cpp
 * \brief
 */


#include "CCenteredCheckBox.h"

#include <QtGui>
#include <QCheckBox>
#include <QHBoxLayout>

//-------------------------------------------------------------------------------------------------
CCenteredCheckBox::CCenteredCheckBox(
    QWidget *a_parent
) :
    QWidget  (a_parent),
    _checkBox(Q_NULLPTR)
{
    setContentsMargins(0, 0, 0, 0);
    setAutoFillBackground(true);

    _checkBox = new QCheckBox();

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(_checkBox);
    layout->setAlignment(_checkBox, Qt::AlignHCenter | Qt::AlignVCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);
    setFocusProxy(_checkBox);
}
//-------------------------------------------------------------------------------------------------
QCheckBox *
CCenteredCheckBox::checkBox() const
{
    return _checkBox;
}
//-------------------------------------------------------------------------------------------------
