/**
 * \file   CCenteredCheckBox.cpp
 * \brief
 */


#include "CCenteredCheckBox.h"

#include <QtGui>


//---------------------------------------------------------------------------
CCenteredCheckBox::CCenteredCheckBox(
    QWidget *parent
) :
    QWidget    (parent),
    _m_checkBox(NULL)
{
    setContentsMargins(0, 0, 0, 0);
    setAutoFillBackground(true);

    _m_checkBox = new QCheckBox();

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(_m_checkBox);
    layout->setAlignment(_m_checkBox, Qt::AlignHCenter | Qt::AlignVCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);
    setFocusProxy(_m_checkBox);
}
//---------------------------------------------------------------------------
QCheckBox *
CCenteredCheckBox::checkBox() const {
    return _m_checkBox;
}
//---------------------------------------------------------------------------
