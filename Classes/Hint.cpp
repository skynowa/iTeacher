/**
 * \file  Hint.cpp
 * \brief
 */


#include "Hint.h"

#include <xLib/Core/Core.h>
#include <xLib/Package/Application.h>
#include <xLib/Package/Translate.h>
#include <xLib/Fs/File.h>
#include <xLib/Fs/FileIO.h>

#include "../QtLib/Common.h"
#include "../QtLib/Utils.h"

#include <QtConcurrent>
#include <QToolTip>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "Ui/Main.h"
#include "Utils.h"

//-------------------------------------------------------------------------------------------------
Hint::Hint(
    QObject                                *a_parent,
    const Type                              a_type,
    const qtlib::SqlRelationalTableModelEx &a_model
) :
    QObject{a_parent},
    _type  {a_type},
    _model {a_model}
{
}
//-------------------------------------------------------------------------------------------------
/* static */
Hint
Hint::trayIcon(
    QObject                                *a_parent,
    const qtlib::SqlRelationalTableModelEx &a_model
)
{
    return Hint(a_parent, Type::TrayIcon, a_model);
}
//-------------------------------------------------------------------------------------------------
/* static */
Hint
Hint::messageBox(
    const qtlib::SqlRelationalTableModelEx &a_model
)
{
    return Hint(nullptr, Type::MessageBox, a_model);
}
//-------------------------------------------------------------------------------------------------
/* static */
Hint
Hint::toolTip(
    const qtlib::SqlRelationalTableModelEx &a_model
)
{
    return Hint(nullptr, Type::ToolTip, a_model);
}
//-------------------------------------------------------------------------------------------------
void
Hint::show() const
{
    bool bRv {};

    // QMessageBox - only one instance
    QSharedMemory locker;
    {
        cQString dlgGuid = qS2QS(xl::package::Application::info().name) + "_hint_guid";

        locker.setKey(dlgGuid);

        bRv = locker.attach();
        qCHECK_DO(bRv, return);

        bRv = locker.create(1);
        qCHECK_DO(!bRv, return);
    }

    QString term;
    QString valueBrief;
    QString langCodeFrom;
    QString langCodeTo;
    {
        term = qtlib::Utils::clipBoardSelecrtionOrClipboard();

        // auto detect languages
        xl::package::Translate translate;

        std::tstring_t textToBrief;
        std::tstring_t textToDetail;
        std::tstring_t textToRaw;
        std::tstring_t langFrom;
        std::tstring_t langTo;
        translate.run(term.toStdString(), &textToBrief, &textToDetail, &textToRaw, &langFrom, &langTo);
        qTEST(!textToBrief.empty());
        qTEST(!textToDetail.empty());
        qTEST(!textToRaw.empty());
        qTEST(!langFrom.empty());
        qTEST(!langTo.empty());

        // [out]
        {
            valueBrief   = QString::fromStdString(textToBrief);
            Q_UNUSED(textToDetail);
            Q_UNUSED(textToRaw);
            langCodeFrom = QString::fromStdString(langFrom);
            langCodeTo   = QString::fromStdString(langTo);

            qDebug() << qTRACE_VAR(langCodeFrom) << " -> " << qTRACE_VAR(langCodeTo);

            // TODO: xl::package::Translate::_langsDetect() - fix
            if (langCodeFrom == "auto" &&
                langCodeTo   == "auto")
            {
                langCodeFrom = "ru";
                langCodeTo   = "en";

                qDebug() << qTRACE_VAR(langCodeFrom) << " -> " << qTRACE_VAR(langCodeTo);
            }
        }
    }

    // text (is term exists) - format
    QString isTermExistsStr;
    bool    isTermExists {};
    QString tagName;
    {
        QSqlDatabase db = _model.database();
        SqliteDb     sqliteDb(nullptr, &db, _model);

        // isTermExists
        isTermExists    = sqliteDb.isTerminExists(term);
        isTermExistsStr = isTermExists ? QString(tr("Exists")) : QString(tr("New"));

        // tagID
        const QSqlRecord recordTerm = sqliteDb.findByField(_model.tableName(), DB_F_MAIN_TERM, term);
        cQString tagID = recordTerm.value(DB_F_MAIN_TAG).toString();

        // tagName
        const QSqlRecord recordTag = sqliteDb.findByField(DB_T_TAGS, DB_F_TAGS_ID, tagID);
        tagName = recordTag.value(DB_F_TAGS_NAME).toString();
        if ( tagName.isEmpty() ) {
            tagName = "-";
        }
    }

    QString title;
    {
        cQString appName = qS2QS(xl::package::Application::info().name);
        cQString dbName  = QFileInfo( _model.database().databaseName() ).fileName();

        switch (_type) {
        case Type::TrayIcon:
        case Type::MessageBox:
            title = QString("%1 - %2 [%3 -> %4] %5")
                        .arg(appName)
                        .arg(dbName)
                        .arg(langCodeFrom)
                        .arg(langCodeTo)
                        .arg(isTermExistsStr);
            break;
        case Type::ToolTip:
            title = QString("<b>%1 - %2</b> [<b>%3 -> %4</b>] <b class='term_exists'>%5</b>")
                        .arg(appName)
                        .arg(dbName)
                        .arg(langCodeFrom)
                        .arg(langCodeTo)
                        .arg(isTermExistsStr);
            break;
        }
    }

    // text - format
    QString text;
    {
        switch (_type) {
        case Type::TrayIcon:
            // TODO: QSystemTrayIcon doesn't support HTML ???
            text = QString(
                        "%1\n\n"
                        "%2\n\n"
                        "%3\n")
                        .arg(term)
                        .arg(valueBrief)
                        .arg(tagName);
        case Type::MessageBox:
            {
                constexpr std::size_t termFontSize  {24};
                constexpr std::size_t valueFontSize {termFontSize - 2};

                text = QString(
                            "<style>"
                                "h3 {"
                                    "color: blue;"
                                    "text-align: center;"
                                    "font-size: %1px;"
                                "}"
                                "h4 {"
                                    "color: red;"
                                    "text-align: center;"
                                    "font-size: %2px;"
                                "}"
                                "h5 {"
                                    "color: black;"
                                    "text-align: center;"
                                    "font-size: %2px;"
                                "}"
                                ".term_exists {"
                                    "color: green;"
                                "}"
                            "</style>"
                            "<h3>%3</h3>"   // title
                            "<h4>%4</h4>"   // msg
                            "<h5>%5</h5>"   // tagName
                            "<h5> </h5>")   // force EOL
                            .arg(termFontSize)
                            .arg(valueFontSize)
                            .arg(term)
                            .arg(valueBrief)
                            .arg(tagName);
            }
            break;
        case Type::ToolTip:
            {
                constexpr std::size_t termFontSize  {24};
                constexpr std::size_t valueFontSize {termFontSize - 2};

                text = QString(
                            "<style>"
                                "h3 {"
                                    "color: blue;"
                                    "text-align: center;"
                                    "font-size: %1px;"
                                "}"
                                "h4 {"
                                    "color: red;"
                                    "text-align: center;"
                                    "font-size: %2px;"
                                "}"
                                ".term_exists {"
                                    "color: green;"
                                "}"
                            "</style>"
                            "%3"            // title
                            "<hr/>"
                            "<h3>%4</h3>"
                            "<h4>%5</h4>"   // msg
                            "<h5>%6</h5>"   // tagName
                            "<h5> </h5>")   // force EOL
                            .arg(termFontSize)
                            .arg(valueFontSize)
                            .arg(title)
                            .arg(term)
                            .arg(valueBrief)
                            .arg(tagName);
            }
            break;
        }
    }

    // Speech
    {
        QStringList audioFiles;
        {
            cQString &tempDirPath = Application::tempDirPath().c_str();

            // Download as file (langCodeTo)
            cQString audioPathTo = QString("%1/audio_to.mp3").arg(tempDirPath);
            _audioFileDownload(valueBrief, langCodeTo, audioPathTo);

            // Download as file (langCodeFrom)
            cQString audioPathFrom = QString("%1/audio_from.mp3").arg(tempDirPath);
            _audioFileDownload(term, langCodeFrom, audioPathFrom);

            audioFiles << audioPathTo << audioPathFrom;
        }

        // Play file
        QMediaPlayer player;

        if ( player.isAudioAvailable() ) {
            qDebug() << qTRACE_VAR(player.isAudioAvailable());

            QMediaPlaylist playList;

            for (const auto &it_audioFile : audioFiles) {
                playList.addMedia( QUrl::fromLocalFile(it_audioFile) );
            }

            player.setPlaylist(&playList);
            player.setVolume(50);
            player.play();

            if (player.error() != QMediaPlayer::Error::NoError) {
                qDebug() << qTRACE_VAR(player.error());
                qDebug() << qTRACE_VAR(player.errorString());
            }
        } else {
            cQString mplayerBin = "mplayer";

            QStringList args;
            args << "-volume" << "100";
            args << audioFiles;

            QProcess *proc = new QProcess();

            connect(proc, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
                    proc, &QProcess::deleteLater);
            proc->start(mplayerBin, args);
        }
    }

    // show message
    switch (_type) {
    case Type::TrayIcon:
        {
            if ( !QSystemTrayIcon::supportsMessages() ) {
                qWarning() << qTRACE_VAR(QSystemTrayIcon::supportsMessages());
            }

            auto *mainWnd = static_cast<Main *>( this->parent() );
            qTEST_PTR(mainWnd);

            mainWnd->trayIcon().showMessage(title, text, QSystemTrayIcon::Information, _timeoutMs);
        }
        break;
    case Type::MessageBox:
        {
            QMessageBox msgBox;
            msgBox.setParent( static_cast<QWidget *>(nullptr) );

            // Icon
            {
                const auto icon = isTermExists ? QMessageBox::Information : QMessageBox::Critical;
                msgBox.setIcon(icon);
            }

            // [HACK] Title width - set min value
            {
                const int widthMin {400};

                auto *horizontalSpacer = new QSpacerItem(widthMin, 0,
                    QSizePolicy::Minimum, QSizePolicy::Expanding);
                auto *layout           = static_cast<QGridLayout *>(msgBox.layout());

                layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
            }

            msgBox.setWindowTitle(title);
            msgBox.setText(text);
            msgBox.setStandardButtons(QMessageBox::Ok);

            QTimer::singleShot(_timeoutMs, &msgBox, SLOT(close()));
            msgBox.exec();
        }
        break;
    case Type::ToolTip:
        QToolTip::showText(QCursor::pos(), text, nullptr, QRect(), _timeoutMs);
        break;
    }
}
//-------------------------------------------------------------------------------------------------
void
Hint::_audioFileDownload(
    cQString &a_text,       ///< source text for audio file
    cQString &a_langCode,   ///< language code
    cQString &a_audioPath   ///< audio file path
) const
{
    using namespace xl;
    using namespace xl::package::curl;
    using namespace xl::fs;

    cbool_t isDebug {false};

    HttpClient http(isDebug);

    DataIn dataIn;
    dataIn.url     = xT("https://translate.google.com.vn/translate_tts");
    dataIn.request = Format::str("ie={}&q={}&tl={}&client={}",
                        "UTF-8",
                        http.escape(a_text.toStdString()),
                        a_langCode.toStdString(),
                        "tw-ob");
    Cout() << qTRACE_VAR(dataIn.url);
    Cout() << qTRACE_VAR(dataIn.request);

    DataOut dataOut;

    bool_t bRv = http.get(dataIn, &dataOut);
    xTEST(bRv);
    xTEST(!dataOut.headers.empty());
    xTEST(!dataOut.body.empty());

    File file( a_audioPath.toStdString() );
    file.binWrite({dataOut.body.cbegin(), dataOut.body.cend()}, FileIO::OpenMode::BinWrite);
}
//-------------------------------------------------------------------------------------------------
