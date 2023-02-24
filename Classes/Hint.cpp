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
        QString data;
        {
            QClipboard *clipboard = QApplication::clipboard();
            if (clipboard == nullptr) {
                qDebug() << __FUNCTION__ << "clipboard - return";
                return;
            }

            // TODO: option or new method
            qDebug() << qTRACE_VAR(clipboard->supportsSelection());

            const auto mode = clipboard->supportsSelection() ?
                QClipboard::Mode::Selection : QClipboard::Mode::Clipboard;

            data = clipboard->text(mode).trimmed();
        }

        term = data;

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
        }
    }

    // text (is term exists) - format
    QString isTermExists;
    {
        QSqlDatabase db = _model.database();

        SqliteDb sqliteDb(nullptr, &db, _model);
        bRv = sqliteDb.isTerminExists(term);

        isTermExists = (bRv) ? QString(tr("Exists")) : QString(tr("New"));
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
                        .arg(isTermExists);
            break;
        case Type::ToolTip:
            title = QString("<b>%1 - %2</b> [<b>%3 -> %4</b>] <b class='term_exists'>%5</b>")
                        .arg(appName)
                        .arg(dbName)
                        .arg(langCodeFrom)
                        .arg(langCodeTo)
                        .arg(isTermExists);
            break;
        }
    }

    // text - format
    QString text;
    {
        switch (_type) {
        case Type::TrayIcon:
        case Type::MessageBox:
            // TODO: QSystemTrayIcon doesn't support HTML ???
            text = QString(
                        "%1\n\n"
                        "%2")
                        .arg(term, valueBrief);
            break;
        case Type::ToolTip:
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
                        "%3"
                        "<hr/>"
                        "<h3>%4</h3>"
                        "<h4>%5</h4>"
                        "<h5> </h5>")   // force EOL
                        .arg(termFontSize)
                        .arg(valueFontSize)
                        .arg(title)
                        .arg(term)
                        .arg(valueBrief);
            break;
        }
    }

    // Speech
    {
        cQString audioPath = QString("%1/audio.mp3").arg(Application::tempDirPath().c_str());

        // Download as file
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
                                http.escape(valueBrief.toStdString()),
                                langCodeTo.toStdString(),
                                "tw-ob");

            DataOut dataOut;

            bool_t bRv = http.get(dataIn, &dataOut);
            xTEST(bRv);
            xTEST(!dataOut.headers.empty());
            xTEST(!dataOut.body.empty());

            File file( audioPath.toStdString() );
            file.binWrite({dataOut.body.cbegin(), dataOut.body.cend()}, FileIO::OpenMode::BinWrite);
        }

        // Play file
        QMediaPlayer player;

        qDebug() << qTRACE_VAR(player.isAudioAvailable());

        if ( player.isAudioAvailable() ) {
            player.setMedia(QUrl::fromLocalFile(audioPath));
            player.setVolume(50);
            player.play();

            if (player.error() != QMediaPlayer::Error::NoError) {
                qDebug() << qTRACE_VAR(player.error());
                qDebug() << qTRACE_VAR(player.errorString());
            }
        } else {
            cQString mplayerBin = "mplayer";

            QStringList args;
            args << audioPath;

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
            msgBox.setIcon(QMessageBox::Information);
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
