/**
 * \file  ColortemDelegate.cpp
 * \brief
 */


#include "ColorItemDelegate.h"
//-------------------------------------------------------------------------------------------------
ColorItemDelegate::ColorItemDelegate(
    QObject               *a_parent,
    const Qt::GlobalColor  a_highlightColor,
    const Qt::GlobalColor  a_highlightedTextColor,
    const Qt::GlobalColor  a_backgroundColor
) :
    QStyledItemDelegate  (a_parent),
    _highlightColor      {a_highlightColor},
    _highlightedTextColor{a_highlightedTextColor},
    _backgroundColor     {a_backgroundColor}
{
}
//-------------------------------------------------------------------------------------------------
void
ColorItemDelegate::initStyleOption(
    QStyleOptionViewItem *out_option,
    const QModelIndex    &a_index
) const
{
    QStyledItemDelegate::initStyleOption(out_option, a_index);

    out_option->palette.setBrush(QPalette::ColorGroup::Normal, QPalette::ColorRole::Highlight,
        _highlightColor);
    out_option->palette.setBrush(QPalette::ColorGroup::Normal, QPalette::ColorRole::HighlightedText,
        _highlightedTextColor);
    out_option->backgroundBrush = _backgroundColor;
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   BlackWhiteItemDelegate
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
BlackWhiteItemDelegate::BlackWhiteItemDelegate(
    QObject *a_parent
) :
    ColorItemDelegate(a_parent, Qt::black, Qt::white, Qt::darkGray)
{
}
//-------------------------------------------------------------------------------------------------
