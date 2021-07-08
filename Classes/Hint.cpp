/**
 * \file  Hint.cpp
 * \brief
 */


#include "Hint.h"

#include <xLib/Core/Core.h>
#include <xLib/Package/Application.h>
#include <xLib/Package/Translate.h>
#include "../QtLib/Common.h"
#include "../QtLib/Utils.h"
#include <QtConcurrent>
#include <QToolTip>
#include "Forms/Main.h"
#include "Utils.h"

//-------------------------------------------------------------------------------------------------
Hint::Hint(
    QObject            *a_parent,
    const Type          a_type,
    const QSqlDatabase &a_database
) :
    QObject  {a_parent},
    _type    {a_type},
    _database{a_database}
{
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
        QClipboard *clipboard = QApplication::clipboard();
        if (clipboard == nullptr) {
            qDebug() << __FUNCTION__ << "clipboard - return";
            return;
        }

        // TODO: option or new method
        const auto mode = clipboard->supportsSelection() ?
            QClipboard::Mode::Selection : QClipboard::Mode::Clipboard;

        term = clipboard->text(mode).trimmed();

        // auto detect languages
        xl::package::Translate translate;

        std::tstring_t textToBrief;
        std::tstring_t textToDetail;
        std::tstring_t textToRaw;
        std::tstring_t langFrom;
        std::tstring_t langTo;
        translate.execute(term.toStdString(), &textToBrief, &textToDetail,
            &textToRaw, &langFrom, &langTo);
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
        bRv = iteacher::Utils::isTerminExists(_database, term);
        isTermExists = (bRv) ? QString(tr("Exists")) : QString(tr("New"));
    }

    QString title;
    {
        cQString appName = qS2QS(xl::package::Application::info().name);
        cQString dbName  = QFileInfo( _database.databaseName() ).fileName();

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
            text = QString(
                        "<style>"
                            "h3 {"
                                "color: blue;"
                                "text-align: center;"
                                "font-size: 18px;"
                            "}"
                            "h4 {"
                                "color: red;"
                                "text-align: center;"
                                "font-size: 16px;"
                            "}"
                            ".term_exists {"
                                "color: green;"
                            "}"
                        "</style>"
                        "%1"
                        "<hr/>"
                        "<h3>%2</h3>"
                        "<h4>%3</h4>"
                        "<h5> </h5>")   // force EOL
                        .arg(title, term, valueBrief);
            break;
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

            mainWnd->trayIcon().showMessage(title, text, QSystemTrayIcon::Information,
                QSYSTEM_TRAYICON_MESSAGE_TIMEOUT_MSEC);
        }
        break;
    case Type::MessageBox:
        QMessageBox::information(nullptr, title, text);
        break;
    case Type::ToolTip:
        QToolTip::showText(QCursor::pos(), text, nullptr, QRect(),
            QSYSTEM_TRAYICON_MESSAGE_TIMEOUT_MSEC);
        break;
    }
}
//-------------------------------------------------------------------------------------------------
