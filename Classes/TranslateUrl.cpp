/**
 * \file  TranslateUrl.cpp
 * \brief Translate URL
 */


#include "TranslateUrl.h"


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
TranslateUrl::TranslateUrl(
    const Type  a_type,
    cQString   &a_text,
    cQString   &a_langFrom,
    cQString   &a_langTo
) :
    _type    {a_type},
    _text    {a_text},
    _langFrom{a_langFrom},
    _langTo  {a_langTo}
{
}
//-------------------------------------------------------------------------------------------------
QUrl
TranslateUrl::get() const
{
    QString url;

    switch (_type) {
    case Type::Unknown:
        url = "";
        break;
    case Type::Google:
        {
            cQString operation = "translate";

            url = QString("https://translate.google.com/?sl=%1&tl=%2&op=%3&text=%4")
                        .arg(_langFrom)
                        .arg(_langTo)
                        .arg(operation)
                        .arg(_text);
        }
        break;
    case Type::Deepl:
        {
            url = QString("https://www.deepl.com/translator#%1/%2/%3")
                    .arg(_langFrom)
                    .arg(_langTo)
                    .arg(_text);
        }
        break;
    }

    return QUrl(url);
}
//-------------------------------------------------------------------------------------------------
void
TranslateUrl::desktopOpen() const
{
    QDesktopServices::openUrl( get() );
}
//-------------------------------------------------------------------------------------------------
