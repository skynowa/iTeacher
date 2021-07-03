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
        cQString dlgGuid = qS2QS(xl::package::Application::info().name) + "_OnTranslateClipboard_guid";

        locker.setKey(dlgGuid);

        bRv = locker.attach();
        qCHECK_DO(bRv, return);

        bRv = locker.create(1);
        qCHECK_DO(!bRv, return);
    }

    QString title;
    {
        QFileInfo info( _database.databaseName() );

        title = QString("%1 - %2")
                    .arg( qS2QS(xl::package::Application::info().name) )
                    .arg( info.fileName() );
    }

    QString term;
    QString text;
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

        QString valueBrief;
        QString valueDetail;
        QString valueRaw;

        QString langCodeFrom;
        QString langCodeTo;
        {
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
                valueDetail  = QString::fromStdString(textToDetail);
                valueRaw     = QString::fromStdString(textToRaw);

                langCodeFrom = QString::fromStdString(langFrom);
                langCodeTo   = QString::fromStdString(langTo);

            #if 0
                qDebug() << qTRACE_VAR(valueBrief);
                qDebug() << qTRACE_VAR(valueDetail);
                qDebug() << qTRACE_VAR(valueRaw);
                qDebug() << qTRACE_VAR(langCodeFrom);
                qDebug() << qTRACE_VAR(langCodeTo);
            #endif

                Q_UNUSED(valueDetail);
            }
        }

        if (_type == Type::TrayIcon) {
            // QSystemTrayIcon doesn't support HTML ???
            text = QString(
                        "%1 [%2 -> %3]\n\n"
                        "%4\n\n"
                        "%5")
                        .arg(title, langCodeFrom, langCodeTo, term, valueBrief);
        } else {
            text = QString(
                        "<style>"
                            "h3 {"
                                "color: blue"
                            "}"
                            "h4 {"
                                "color: red"
                            "}"
                        "</style>"
                        "<b>%1</b> [<b>%2 -> %3</b>]"
                        "<hr/>"
                        "<h3>%4</h3>"
                        "<h4>%5</h4>")
                        .arg(title, langCodeFrom, langCodeTo, term, valueBrief);
        }
    }

    // is term exists
    bRv = iteacher::Utils::isTerminExists(_database, term);
    if (bRv) {
        // term already exists
        if (_type == Type::TrayIcon) {
            text += QString(tr("\n\nTerm already exists"));
        } else {
            text += QString(tr("<br />Term already exists"));
        }
    } else {
        // ok, term is a new
        if (_type == Type::TrayIcon) {
            text += QString(tr("\n\nTerm is a new"));
        } else {
            text += QString(tr("<br />Term is a new"));
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
        QToolTip::showText(QCursor::pos(), text);
        break;
    }
}
//-------------------------------------------------------------------------------------------------
