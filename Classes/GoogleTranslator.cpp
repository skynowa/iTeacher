/**
 * \file   GoogleTranslator.cpp
 * \brief
 */


#include "GoogleTranslator.h"

#include <QDomDocument>
#include <QMediaPlayer>


/**************************************************************************************************
*   private, consts
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
namespace {

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

        qDebug() << qDEBUG_VAR(countEn);
        qDebug() << qDEBUG_VAR(countRu);

        qTEST(false);
    }
}
//-------------------------------------------------------------------------------------------------
void
GoogleTranslator::execute(
    cQString &a_textFrom,       ///< source text
    cQString &a_langFrom,       ///< source text language
    cQString &a_langTo,         ///< target text language
    QString  *a_textToBrief,    ///< [out] target brief translate
    QString  *a_textToDetail,   ///< [out] target detail translate
    QString  *a_textToRaw       ///< [out] target raw translate (HTML) (maybe Q_NULLPTR)
) const
{
    qTEST(!a_textFrom.isEmpty());
    qTEST(!a_langFrom.isEmpty());
    qTEST(!a_langTo.isEmpty());
    qTEST(a_textToBrief  != Q_NULLPTR);
    qTEST(a_textToDetail != Q_NULLPTR);
    qTEST_NA(a_textToDetail);

    QString textToBrief;
    QString textToDetail;
    QString textToRaw;

    // request to Google
    QString response;
    bool    isDictionaryText = false;
    {
        cQString host = QString("https://translate.google.com");
        cQString url  = QString("%1/m?text=%2&sl=%3&tl=%4")
                            .arg(host)
                            .arg(a_textFrom)
                            .arg(a_langFrom)
                            .arg(a_langTo);

        QNetworkAccessManager manager;
        manager.connectToHost(host, 80);

        QNetworkRequest request(url);

        QNetworkReply *reply = manager.get(request);
        qTEST_PTR(reply);

    #if 0
        cQVariant httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if ( !httpStatusCode.isValid() ) {
            qDebug() << qDEBUG_VAR(httpStatusCode);
            return;
        }

        int status = httpStatusCode.toInt();
        if (status != 200) {
            cQString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
            qDebug() << qDEBUG_VAR(reason);
        }
    #endif

        if (reply->error() != QNetworkReply::NoError) {
            *a_textToBrief  = reply->errorString();
            *a_textToDetail = reply->errorString();

            if (a_textToRaw != Q_NULLPTR) {
                *a_textToRaw = reply->errorString();
            }

            reply->close();
            qPTR_DELETE(reply);

            return;
        }

        for ( ; ; ) {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

            qCHECK_DO(reply->isFinished(), break);
        }

        response = QString::fromUtf8(reply->readAll());
        qTEST(!response.isEmpty());

        reply->close();
        qPTR_DELETE(reply);

        textToRaw        = response;
        isDictionaryText = response.contains("Dictionary:");
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

        // qDebug() << qDEBUG_VAR(*a_textToBrief);
        // qDebug() << qDEBUG_VAR(*a_textToDetail);
        // qDebug() << qDEBUG_VAR(*a_textToRaw);
    }
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
        const QUrl            url(urlStr);
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
    {
    #if 0
        Phonon::MediaObject *player = Phonon::createPlayer(
                                            Phonon::MusicCategory,
                                            Phonon::MediaSource(a_filePath));
        qTEST_PTR(player);

        // for signal slot mechanism
        // connect(player, SIGNAL( finished() ),
        //         player, SLOT  ( deleteLater() ));

        player->play();

        // wait for finish
        for (bool bRv = true; bRv; ) {
            Phonon::State state = player->state();
            if (Phonon::LoadingState == state ||
                Phonon::PlayingState == state)
            {
                bRv = true;
            } else {
                qTEST(Phonon::PausedState == state || Phonon::StoppedState == state);

                bRv = false;
            }

            ::Utils::sleep(100);

            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }

        qPTR_DELETE(player);
    #else
        QMediaPlayer player;
        qCHECK_DO(!player.isAudioAvailable(),
            qDebug() << "QMediaPlayer: audio is not available"; return);

        player.setMedia(QUrl::fromLocalFile(a_filePath));
        player.setVolume(35);
        player.play();
    #endif
    }
}
//-------------------------------------------------------------------------------------------------