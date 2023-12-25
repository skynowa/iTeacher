/**
 * \file  TranslateUrl.h
 * \brief Translate URL
 */


#pragma once

#include "../QtLib/Common.h"
//-------------------------------------------------------------------------------------------------
class TranslateUrl
    /// Translate URL
{
public:
///\name ctors, dtor
///\{
    enum class Type
        /// Translate type
    {
        Unknown = 0, // Unknown
        Google  = 1, // https://translate.google.com
        Deepl   = 2  // https://www.deepl.com/translator
    };

             TranslateUrl() = delete;
             TranslateUrl(const Type type, cQString &text, cQString &langFrom, cQString &langTo);
    virtual ~TranslateUrl() = default;

    Q_DISABLE_COPY(TranslateUrl)
///\}

    QUrl get() const;
            ///< Get URL object
    void open() const;
            ///< Open by QDesktopServices

private:
    const Type _type {};
    cQString   _text;
    cQString   _langFrom;
    cQString   _langTo;
};
//-------------------------------------------------------------------------------------------------
