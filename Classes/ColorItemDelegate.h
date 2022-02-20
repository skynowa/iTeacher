/**
 * \file  ColortemDelegate.h
 * \brief
 *
 * \see https://stackoverflow.com/questions/56735853/painting-the-background-of-a-qtableview-with-a-custom-qstyleditemdelegate
 */


#pragma once

#include <QStyledItemDelegate>
#include "../QtLib/Common.h"
//-------------------------------------------------------------------------------------------------
class ColorItemDelegate :
    public QStyledItemDelegate
{
public:
///@name ctors, dtor
///@{
    ColorItemDelegate(QObject *parent, const Qt::GlobalColor highlightColor,
                        const Qt::GlobalColor highlightedTextColor,
                        const Qt::GlobalColor backgroundColor);
    virtual ~ColorItemDelegate() = default;

    Q_DISABLE_COPY(ColorItemDelegate);
///@}

    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const final;

private:
    Q_OBJECT

    const Qt::GlobalColor _highlightColor {};
    const Qt::GlobalColor _highlightedTextColor {};
    const Qt::GlobalColor _backgroundColor {};
};
//-------------------------------------------------------------------------------------------------
class BlackWhiteItemDelegate :
    public ColorItemDelegate
{
public:
///@name ctors, dtor
///@{
    explicit  BlackWhiteItemDelegate(QObject *parent);
    virtual  ~BlackWhiteItemDelegate() = default;
///@}

private:
    Q_OBJECT
};
//-------------------------------------------------------------------------------------------------
