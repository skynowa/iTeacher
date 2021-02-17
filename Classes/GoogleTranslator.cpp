/**
 * \file   GoogleTranslator.cpp
 * \brief  Google translator
 */


#include "GoogleTranslator.h"

#include <QDomDocument>

#if OPTION_QMEDIA_PLAYER
    #include <QMediaPlayer>
#endif


/**************************************************************************************************
*   private, consts
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
namespace
{

cQString langEn = "en";
cQString langRu = "ru";

}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
GoogleTranslator::GoogleTranslator()
{
}
//-------------------------------------------------------------------------------------------------
/* virtual */
GoogleTranslator::~GoogleTranslator()
{
}
//-------------------------------------------------------------------------------------------------
void
GoogleTranslator::languagesDetect(
    cQString                   &a_text,
    GoogleTranslator::Language *a_langFrom,
    GoogleTranslator::Language *a_langTo,
    QString                    *a_langCodeFrom,
    QString                    *a_langCodeTo
) const
{
    qTEST(!a_text.isEmpty());
    qTEST(a_langFrom     != Q_NULLPTR);
    qTEST(a_langTo       != Q_NULLPTR);
    qTEST(a_langCodeFrom != Q_NULLPTR);
    qTEST(a_langCodeTo   != Q_NULLPTR);

    cQString lettersEn = QString::fromUtf8("abcdefghijklmnopqrstuvwxyz");
    cQString lettersRu = QString::fromUtf8("абвгдеёжзийклмнопрстуфхцчшщъыьэюя");

    uint     countEn = 0;
    uint     countRu = 0;

    for (int i = 0; i < a_text.size(); ++ i) {
        cQChar letter = a_text.at(i).toLower();
        qCHECK_DO(!letter.isLetter(), continue);

        qCHECK_DO(lettersEn.contains(letter), ++ countEn);
        qCHECK_DO(lettersRu.contains(letter), ++ countRu);
    }

    cbool isEn      = (countEn != 0 && countRu == 0);
    cbool isRu      = (countEn == 0 && countRu != 0);
    cbool isMixed   = (countEn != 0 && countRu != 0);
    cbool isUnknown = (countEn == 0 && countRu == 0);

    if      (isEn) {
        *a_langFrom     = GoogleTranslator::lgEn;
        *a_langTo       = GoogleTranslator::lgRu;

        *a_langCodeFrom = ::langEn;
        *a_langCodeTo   = ::langRu;

        qDebug() << "Langs: en-ru\n";
    }
    else if (isRu) {
        *a_langFrom     = GoogleTranslator::lgRu;
        *a_langTo       = GoogleTranslator::lgEn;

        *a_langCodeFrom = ::langRu;
        *a_langCodeTo   = ::langEn;

        qDebug() << "Langs: ru-en\n";
    }
    else if (isMixed) {
        qDebug() << "Langs: mixed-mixed\n";

        cbool isPreferEn = (countEn >= countRu);
        cbool isPreferRu = (countRu >  countEn);

        if      (isPreferEn) {
            *a_langFrom     = GoogleTranslator::lgEn;
            *a_langTo       = GoogleTranslator::lgRu;

            *a_langCodeFrom = ::langEn;
            *a_langCodeTo   = ::langRu;

            qDebug() << "Langs (prefer): en-ru\n";
        }
        else if (isPreferRu) {
            *a_langFrom     = GoogleTranslator::lgRu;
            *a_langTo       = GoogleTranslator::lgEn;

            *a_langCodeFrom = ::langRu;
            *a_langCodeTo   = ::langEn;

            qDebug() << "Langs (prefer): ru-en\n";
        }
        else {
            qTEST(false);
        }
    }
    else if (isUnknown) {
        *a_langFrom     = GoogleTranslator::lgUnknown;
        *a_langTo       = GoogleTranslator::lgUnknown;

        *a_langCodeFrom = "";
        *a_langCodeTo   = "";

        qDebug() << "Langs: unknown-unknown\n";
    }
    else {
        *a_langFrom     = GoogleTranslator::lgUnknown;
        *a_langTo       = GoogleTranslator::lgUnknown;

        *a_langCodeFrom = "";
        *a_langCodeTo   = "";

        qDebug() << qTRACE_VAR(countEn);
        qDebug() << qTRACE_VAR(countRu);

        qTEST(false);
    }
}
//-------------------------------------------------------------------------------------------------
void
GoogleTranslator::execute(
    cHttpRequestType &a_httpRequestType,///< HTTP request type
    cQString         &a_textFrom,       ///< source text
    cQString         &a_langFrom,       ///< source text language
    cQString         &a_langTo,         ///< target text language
    QString          *a_textToBrief,    ///< [out] target brief translate
    QString          *a_textToDetail,   ///< [out] target detail translate
    QString          *a_textToRaw       ///< [out] target raw translate (HTML) (maybe Q_NULLPTR)
) const
{
    qTEST(a_httpRequestType == hrGet || a_httpRequestType == hrPost);
    qTEST(!a_textFrom.isEmpty());
    qTEST(!a_langFrom.isEmpty());
    qTEST(!a_langTo.isEmpty());
    qTEST(a_textToBrief  != Q_NULLPTR);
    qTEST(a_textToDetail != Q_NULLPTR);
    qTEST_NA(a_textToRaw);

    cQString              host  = QString("https://translate.google.com");
    QNetworkAccessManager manager;
    QNetworkRequest       request;
    QNetworkReply        *reply = Q_NULLPTR;

    switch (a_httpRequestType) {
    case hrGet: {
           /**
            * HTTP GET request:
            *
            * https://translate.google.com/m?text=cat&sl=en&tl=ru
            */

            cQUrl url  = QString("%1/m?text=%2&sl=%3&tl=%4")
                                .arg(host)
                                .arg(a_textFrom)
                                .arg(a_langFrom)
                                .arg(a_langTo);
            url.toEncoded();

            request.setUrl(url);

            reply = manager.get(request);
            qTEST_PTR(reply);
        }
        break;
    case hrPost: {
           /**
            * HTTP POST request:
            *
            * <form action="/m" class="">
            *     ...
            *     <input type="hidden" name="hl" value="ru"/>
            *     <input type="hidden" name="sl" value="auto"/>
            *     <input type="hidden" name="tl" value="ru"/>
            *     <input type="hidden" name="ie" value="UTF-8"/>
            *     <input type="hidden" name="prev" value="_m"/>
            *     <input type="text" name="q" style="width:65%" maxlength="2048" value=""/><br>
            *     <input type="submit" value="Перевести"/>
            * </form>
            */

            cQUrl url = QString("%1/m").arg(host);
            url.toEncoded();

            QUrlQuery query;
            query.addQueryItem("h1", a_langFrom);
            query.addQueryItem("tl", a_langTo);
            query.addQueryItem("ie", "UTF-8");
            query.addQueryItem("q",  a_textFrom);

            request.setUrl(url);
            request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

            reply = manager.post(request, query.toString(QUrl::FullyEncoded).toUtf8());
            qTEST_PTR(reply);
        }
        break;
    case hrUnknown:
    default:
        qTEST(false);
        break;
    }

     _replyParse(reply, a_textToBrief, a_textToDetail, a_textToRaw);

    reply->close();
    qPTR_DELETE(reply);
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   audio
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
GoogleTranslator::speech(
    cQString &a_text,
    cQString &a_lang,
    cQString &a_filePath
)
{
    bool bRv = false;

    // request to Google
    {
        cQString              urlStr = "http://translate.google.ru/translate_tts?&q=" +
                                       a_text + "&tl=" + a_lang;
        cQUrl                 url(urlStr);
        QNetworkAccessManager manager;
        const QNetworkRequest request(url);

        QNetworkReply *reply = manager.get(request);
        qTEST_PTR(reply);

        for ( ; ; ) {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

            qCHECK_DO( reply->isFinished(), break);
        }

        // write to audio file
        {
            QFile file(a_filePath);
            bRv = file.open(QIODevice::WriteOnly);
            qTEST(bRv);

            file.write(reply->readAll());
        }

        reply->close();
        qPTR_DELETE(reply);
    }

    // play audio file
    // TODO_VER: GoogleTranslator::speech - QSound ???
    {
    #if OPTION_QMEDIA_PLAYER
        QMediaPlayer player;
        if (!player.isAudioAvailable()) {
            cQString msg = QString(QObject::tr("QMediaPlayer: audio is not available."));
            QMessageBox::warning(Q_NULLPTR, qApp->applicationName(), msg);

            return;
        }

        player.setMedia(QUrl::fromLocalFile(a_filePath));
        player.setVolume(35);
        player.play();
    #endif
    }
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
GoogleTranslator::_replyParse(
    QNetworkReply *a_reply,
    QString       *a_textToBrief,
    QString       *a_textToDetail,
    QString       *a_textToRaw
) const
{
    QString textToBrief;
    QString textToDetail;
    QString textToRaw;

    QString response;
    bool    isDictionaryText = false;
    {
    #if 1
        cQVariant httpStatusCode = a_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if ( !httpStatusCode.isValid() ) {
            qDebug() << qTRACE_VAR(httpStatusCode);
            return;
        }

        int status = httpStatusCode.toInt();
        if (status != 200) {
            cQString reason = a_reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
            qDebug() << qTRACE_VAR(reason);
        }
    #endif

        if (a_reply->error() != QNetworkReply::NoError) {
            *a_textToBrief  = a_reply->errorString();
            *a_textToDetail = a_reply->errorString();

            if (a_textToRaw != Q_NULLPTR) {
                *a_textToRaw = a_reply->errorString();
            }

            return;
        }

        for ( ; ; ) {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

            qCHECK_DO(a_reply->isFinished(), break);
        }

        response = QString::fromUtf8(a_reply->readAll());
        qTEST(!response.isEmpty());

        textToRaw        = response;
        isDictionaryText = response.contains("Dictionary:");

        // qDebug() << qTRACE_VAR(response);
    }

    // proccess response

    {
        response.replace("Dictionary:", "\n");
        response.replace("<br>", "\n");
    }

    // parse response
    {
        QDomDocument document;
        document.setContent(response);

        QDomNodeList docList = document.elementsByTagName("div");
        qTEST(docList.count() >= 3);

        // out - textToBrief
        textToBrief = docList.at(2).toElement().text();
        qTEST(!textToBrief.isEmpty());

        // out - textToDetail
        if (isDictionaryText) {
            textToDetail = docList.at(5).toElement().text();
            qTEST(!textToDetail.isEmpty());
        } else {
            textToDetail = QObject::tr("n/a");
        }
    }

    // out
    {
        a_textToBrief->swap(textToBrief);
        a_textToDetail->swap(textToDetail);

        if (a_textToRaw != Q_NULLPTR) {
            a_textToRaw->swap(textToRaw);
        }

        // qDebug() << qTRACE_VAR(*a_textToBrief);
        // qDebug() << qTRACE_VAR(*a_textToDetail);
        // qDebug() << qTRACE_VAR(*a_textToRaw);
    }
}
//-------------------------------------------------------------------------------------------------
