/**
 * \file   GoogleTranslator.h
 * \brief
 */


#pragma once

#include "../QtLib/Common.h"
#include "../QtLib/Utils.h"
//-------------------------------------------------------------------------------------------------
class GoogleTranslator
    /// Google translator
{
public:
    enum Language
        ///< detected language
    {
        lgUnknown,
        lgEn,
        lgRu
    };

    enum HttpRequestType
        ///< HTTP request type
    {
        hrUnknown,
        hrGet,
        hrPost
    };
    typedef const HttpRequestType cHttpRequestType;

             GoogleTranslator();
        ///< constructor
    virtual ~GoogleTranslator();
        ///< destructor

    void     languagesDetect(cQString &text, Language *langFrom,
                 Language *langTo, QString *langCodeFrom, QString *langCodeTo) const;
        ///< detect languages
    void     execute(cHttpRequestType &httpRequestType, cQString &textFrom, cQString &langFrom,
                 cQString &langTo, QString *textToBrief, QString *textToDetail, QString *textToRaw)
                 const;
        ///< translate text by Google API

    // audio
    void     speech(cQString &text, cQString &lang, cQString &filePath);

private:
    void     _replyParse(QNetworkReply *reply, QString *textToBrief, QString *textToDetail,
                 QString *textToRaw) const;
        ///< parse response
};
//-------------------------------------------------------------------------------------------------
