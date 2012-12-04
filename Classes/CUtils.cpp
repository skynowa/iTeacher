/**
 * \file   CUtils.cpp
 * \brief
 */


#include "CUtils.h"


#if defined(Q_WS_WIN)
    #include <windows.h>
#else

#endif

#include <QDomDocument>
#include <QTextStream>


/****************************************************************************
*   public
*
*****************************************************************************/


/****************************************************************************
*   GUI
*
*****************************************************************************/

//---------------------------------------------------------------------------
/* static */
bool
CUtils::setApplicationSingle(
    const QString &a_applicationGuid
)
{
    bool bRv = false;

    static QSharedMemory smLocker(a_applicationGuid);

    bRv = smLocker.attach();
    qCHECK_RET(true == bRv, false);

    bRv = smLocker.create(1);
    qCHECK_RET(false == bRv, false);

    return true;
}
//---------------------------------------------------------------------------
/* static */
void
CUtils::widgetAlignCenter(
    QWidget *a_widget
)
{
    Q_ASSERT(NULL != a_widget);

#if 0
    QDesktopWidget *desktop = QApplication::desktop();
    Q_ASSERT(NULL != desktop);

    const int x = (desktop->width()  - a_widget->width())  / 2;
    const int y = (desktop->height() - a_widget->height()) / 2;

    a_widget->setGeometry(x, y, a_widget->width(), a_widget->height());
#endif

    QRect rcRect = QStyle::alignedRect(
                        Qt::LeftToRight, Qt::AlignCenter, a_widget->size(),
                        qApp->desktop()->availableGeometry());

    a_widget->setGeometry(rcRect);
}
//---------------------------------------------------------------------------
/* static */
void
CUtils::widgetAlignTopCenter(
    QWidget *a_widget
)
{
    Q_ASSERT(NULL != a_widget);

    QDesktopWidget *desktop = QApplication::desktop();
    Q_ASSERT(NULL != desktop);

    const int x = (desktop->width() - a_widget->width()) / 2;
    const int y = 0;

    a_widget->setGeometry(x, y, a_widget->width(), a_widget->height());
}
//---------------------------------------------------------------------------
/* static */
void
CUtils::applicationActivate(
    const QString &a_className,
    const QString &a_windowName
)
{
#if defined(Q_WS_WIN)
    HWND hWnd = ::FindWindow(qQS2S(a_className).c_str(), qQS2S(a_windowName).c_str());
    if (NULL != hWnd) {
        BOOL blRv = ::SetForegroundWindow(hWnd);
        Q_ASSERT((BOOL)FALSE != blRv);

        ::Beep(400, 400);
    }
#else
    // TODO: activation application window
#endif
}
//---------------------------------------------------------------------------


/****************************************************************************
*   DB
*
*****************************************************************************/

//---------------------------------------------------------------------------
int
CUtils::sqlTableModelRowCount(
    QSqlTableModel *model
)
{
    Q_ASSERT(NULL != model);

    for ( ; model->canFetchMore(); ) {
        model->fetchMore();
    }

    return model->rowCount();
}
//---------------------------------------------------------------------------
/* static */
void
CUtils::importCsv(
    const QString          &a_filePath,
    QSqlTableModel         *a_sqlTableModel,
    const QVector<QString> &a_fieldNames,
    const QString          &a_columnSeparator
)
{
    bool bRv = false;

    // read file
    QStringList slFile;

    {
        QFile fileCSV(a_filePath);

        bRv = fileCSV.open(QFile::ReadOnly);
        Q_ASSERT(true == bRv);

        QString data = fileCSV.readAll();
        slFile = data.split("\n");

        fileCSV.close();

        qCHECK_DO(true == slFile.isEmpty(), return);
    }

    // file -> DB
    for (int i = 0; i < slFile.size(); ++ i) {
        const QStringList cslRow = slFile.at(i).split(a_columnSeparator);

        // iTargetRow
        int iTargetRow = CUtils::sqlTableModelRowCount(a_sqlTableModel) - 1;

        // srRecord
        QSqlRecord srRecord;

        for (int x = 0; x < a_fieldNames.size(); ++ x) {
            srRecord.append(QSqlField(a_fieldNames.at(x)));
            srRecord.setValue(a_fieldNames.at(x), cslRow.at(x));
        }

        bRv = a_sqlTableModel->insertRecord(iTargetRow, srRecord);
        Q_ASSERT(true == bRv);

        bRv = a_sqlTableModel->submitAll();
        Q_ASSERT(true == bRv);
    }
}
//---------------------------------------------------------------------------
/* static */
void
CUtils::exportCsv(
    const QString          &a_filePath,
    QSqlTableModel         *a_sqlTableModel,
    const QVector<QString> &a_fieldNames,
    const QString          &a_columnSeparator
)
{
    // DB -> text
    QString sCsv;

    // DB fields -> CSV header
    for (int x = 0; x < a_fieldNames.size(); ++ x) {
        sCsv.push_back( a_fieldNames.at(x) );
        sCsv.push_back( a_columnSeparator );
    }
    sCsv.push_back( "\n" );

    // DB -> file
    {
        int iRealRowCount = CUtils::sqlTableModelRowCount(a_sqlTableModel);

        for (int i = 0; i < iRealRowCount; ++ i) {
            for (int x = 0; x < a_fieldNames.size(); ++ x) {
                sCsv.push_back( a_sqlTableModel->record(i).value( a_fieldNames.at(x) ).toString() );
                sCsv.push_back( a_columnSeparator );
            }

            sCsv.push_back( "\n" );
        }
    }

    // write to file
    {
        QFile fileCSV(a_filePath);

        bool bRv = fileCSV.open(QFile::WriteOnly | QIODevice::Text);
        Q_ASSERT(true == bRv);

        QTextStream stream(&fileCSV);

        stream.setCodec("UTF-8");
        stream << sCsv;
    }
}
//---------------------------------------------------------------------------


/****************************************************************************
*   web
*
*****************************************************************************/

//---------------------------------------------------------------------------
QString
CUtils::googleTranslate(
    const QString &textFrom,
    const QString &langFrom,
    const QString &langTo
)
{
    QString sRv;

    // request to Google
    QString sReply;

    {
        const QString csUrl =
            "http://translate.google.com/m?translate_a/t?client=t&text="
            + textFrom +
            "&sl="
            + langFrom +
            "&tl="
            + langTo;

        QNetworkAccessManager nmManager;
        QNetworkRequest       nrRequest(csUrl);

        QNetworkReply *nrReply = nmManager.get(nrRequest);
        Q_ASSERT(NULL != nrReply);

        do {
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        }
        while (! nrReply->isFinished());

        sReply = QString::fromUtf8(nrReply->readAll());

        nrReply->close();
        delete nrReply; nrReply = NULL;
    }

    // parse reply
    QStringList lstReply;

    {
        Q_ASSERT(true == sReply.contains(QObject::tr("Словарь:")));

        QString text = sReply;
        text.replace("<br>", "~");
        text.replace("~~", "*");
        text.replace(QObject::tr("Словарь:"), QObject::tr(""));

        // qDebug() << text;

        QDomDocument document;
        document.setContent(text);

        QDomNodeList docList = document.elementsByTagName("div");
        for (int i = 0; i < docList.count(); ++ i) {
            lstReply.append(docList.at(i).toElement().text());
        }

        QString str = lstReply.at(4);
        if (false == str.contains(QObject::tr("Google"))) {
            str.replace("~", "\n    - ");
            str.replace("*", "\n\n");
            str.remove(str.count() - 2, 2);
        } else {
            str.clear();
        }

        lstReply.append(str);

        //
        sRv = lstReply.at(2);
        sRv += "\n\n" + lstReply.last();
    }

    return sRv;
}
//---------------------------------------------------------------------------


/****************************************************************************
*   converters
*
*****************************************************************************/

//---------------------------------------------------------------------------
std::wstring
CUtils::toStdWString(
    const QString &str
)
{
#ifdef _MSC_VER
    return std::wstring(reinterpret_cast<const wchar_t *>( str.utf16() ));
#else
    return str.toStdWString();
#endif
}
//---------------------------------------------------------------------------
QString
CUtils::fromStdWString(
    const std::wstring &str
)
{
#ifdef _MSC_VER
    return QString::fromUtf16(reinterpret_cast<const ushort *>( str.c_str() ));
#else
    return QString::fromStdWString(str);
#endif
}
//---------------------------------------------------------------------------


/****************************************************************************
*   private
*
*****************************************************************************/

//---------------------------------------------------------------------------
CUtils::CUtils() {

}
//---------------------------------------------------------------------------
CUtils::~CUtils() {

}
//---------------------------------------------------------------------------
