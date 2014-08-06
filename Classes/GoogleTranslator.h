/**
 * \file   GoogleTranslator.h
 * \brief
 */


#ifndef iTeacher__GoogleTranslatorH
#define iTeacher__GoogleTranslatorH
//-------------------------------------------------------------------------------------------------
#include "../QtLib/Common.h"
#include "../QtLib/Utils.h"
//-------------------------------------------------------------------------------------------------
class GoogleTranslator
    /// application
{
public:
             GoogleTranslator();
        ///< constructor
    virtual ~GoogleTranslator();
        ///< destructor

    enum Language
        ///< detected language
    {
        lgUnknown,
        lgEn,
        lgRu
    };

    void     languagesDetect(cQString &text, Language *langFrom,
                 Language *langTo, QString *langCodeFrom, QString *langCodeTo) const;
        ///< detect languages
    void     execute(cQString &textFrom, cQString &langFrom, cQString &langTo, QString *textToBrief,
                 QString *textToDetail, QString *textToRaw) const;
        ///< translate text by Google API

    // audio
    void     speech(cQString &text, cQString &lang, cQString &filePath);

private:

};
//-------------------------------------------------------------------------------------------------
#endif // iTeacher__GoogleTranslatorH
