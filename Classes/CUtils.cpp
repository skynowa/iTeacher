/**
 * \file   CUtils.cpp
 * \brief
 */


#include "CUtils.h"


#if defined(Q_WS_WIN)
    #include <windows.h>
#else

#endif


/****************************************************************************
*   public
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
    HWND hWnd = ::FindWindowW(a_className.toStdWString().c_str(), a_windowName.toStdWString().c_str());
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

        for (int i = 0; i < a_fieldNames.size(); ++ i) {
            srRecord.append(QSqlField(a_fieldNames.at(i)));
            srRecord.setValue(a_fieldNames.at(i), cslRow.at(i));
        }

        bRv = a_sqlTableModel->insertRecord(iTargetRow, srRecord);
        Q_ASSERT(true == bRv);

        bRv = a_sqlTableModel->submitAll();
        Q_ASSERT(true == bRv);
    }
}
//---------------------------------------------------------------------------
QString
CUtils::googleTranslate(
    const QString &textFrom,
    const QString &langTo
)
{
    // cache, uses Google to translate text
    typedef std::map< QString, std::pair<QString, QString> > container_t;

    static container_t g_mspTranslations;

    // locate the translation in the map
    container_t::iterator it = g_mspTranslations.find(textFrom);
    if (it != g_mspTranslations.end()) {
        if (langTo == it->second.first) {
            return it->second.second;
        }
    }

    // translate URL
    const QString csUrl = QString("http://translate.google.com/translate_a/t?client=t&text=%0&hl=%1&sl=auto&tl=%1&multires=1&prev=enter&oc=2&ssel=0&tsel=0&uptl=%1&sc=1")
                                .arg(textFrom)
                                .arg(langTo);

    QNetworkAccessManager nmManager;
    QNetworkRequest       nrRequest(csUrl);

    // get reply from Google
    QNetworkReply *nrReply = nmManager.get(nrRequest);
    do {
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    while (! nrReply->isFinished());

    // convert to string
    QString sTranslation( QString::fromUtf8(nrReply->readAll()) );

    // free memory
    nrReply->close();

    delete nrReply; nrReply = NULL;

    // remove [[[" from the beginning
    sTranslation = sTranslation.replace("[[[\"", "");

    // extract final translated string
    sTranslation = sTranslation.mid(0, sTranslation.indexOf(",\"") - 1);

    // add the sTranslation to the map so we don't need to make another web request for a sTranslation
    g_mspTranslations[textFrom] = std::pair<QString, QString>(langTo, sTranslation);

    return sTranslation;
}
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
