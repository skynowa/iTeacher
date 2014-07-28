/**
 * \file   WordEditor.cpp
 * \brief
 */


#include "WordEditor.h"

#include "../QtLib/Utils.h"

#include <xLib/Core/Application.h>
#include <QDomDocument>


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
WordEditor::WordEditor(
    QWidget             *a_parent,
    QSqlTableModel      *a_tableModel,
    qtlib::SqlNavigator *a_sqlNavigator,
    cbool               &a_insertMode,
    cQString            &a_newTerm /* = QString() */
) :
    QDialog       (a_parent),
    _isConstructed(false),
    _model        (a_tableModel),
    _sqlNavigator (a_sqlNavigator),
    _currentRow   (a_sqlNavigator->view()->currentIndex().row()),
    _insertMode   (a_insertMode),
    _termNew      (a_newTerm.trimmed()),
    _plInfoDefault()
{
    qTEST_PTR(a_parent);
    qTEST_PTR(a_tableModel);
    qTEST_PTR(a_sqlNavigator);
    qTEST_NA(_currentRow);

    _construct();
}
//-------------------------------------------------------------------------------------------------
/* virtual */
WordEditor::~WordEditor()
{
    _destruct();
}
//-------------------------------------------------------------------------------------------------
bool
WordEditor::isConstructed() const
{
    return _isConstructed;
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
WordEditor::_construct()
{
    // _isConstructed
    if (_currentRow < 0) {
        _isConstructed = false;
        return;
    }

    ui.setupUi(this);

    // signals
    {
        connect(ui.pbtnLangsSwap,     SIGNAL( clicked() ),
                this,                 SLOT  ( slot_pbtnLangsSwap_OnClicked() ));

        connect(ui.pbtnTermTranslate, SIGNAL( clicked() ),
                this,                 SLOT  ( slot_termTranslate() ));

        connect(ui.pbtnTermCheck,     SIGNAL( clicked() ),
                this,                 SLOT  ( slot_termCheck() ));

        connect(ui.bbxButtons,        SIGNAL( clicked(QAbstractButton *) ),
                this,                 SLOT  ( slot_bbxButtons_OnClicked(QAbstractButton *) ));

        connect(ui.tedtWordTerm,      SIGNAL( textChanged() ),
                this,                 SLOT  ( slot_WordTermOrValue_OnTextChanged() ));

        connect(ui.tedtWordBriefValue, SIGNAL( textChanged() ),
                this,                  SLOT  ( slot_WordTermOrValue_OnTextChanged() ));
    }

    // shortcuts
    {
        // term translate (Ctrl+Return)
        QShortcut *shortcut = new QShortcut(this);
        shortcut->setKey(QKeySequence(Qt::CTRL + Qt::Key_Return));

        connect(shortcut, SIGNAL( activated() ),
                this,     SLOT  ( slot_termTranslate() ));
    }

    // UI
    {
        // title
        {
            QFileInfo info( _sqlNavigator->model()->database().databaseName() );

            cQString title = QString("%1 - %2")
                                .arg( APP_NAME )
                                .arg( info.fileName() );

            setWindowTitle(title);
        }

        QSqlRecord record = _model->record(_currentRow);

        // term, value
        {
            ui.tedtWordTerm->setText( record.value(DB_F_MAIN_TERM).toString() );
            ui.tedtWordBriefValue->setText( record.value(DB_F_MAIN_VALUE).toString() );
            ui.tedtWordDetailValue->setText("");
        }

        // tags
        {
            QSqlQuery query;
            query.prepare("SELECT " DB_F_TAGS_NAME " FROM " DB_T_TAGS ";");
            bool rv = query.exec();
            qCHECK_REF(rv, query);

            for ( ; query.next(); ) {
                ui.cboTags->addItem( query.value(0).toString() );
            }

            cint index = ui.cboTags->findText( record.value(DB_F_MAIN_TAG).toString() );
            ui.cboTags->setCurrentIndex(index);
        }

        // checkboxes
        ui.chkWordIsLearned->setChecked( record.value(DB_F_MAIN_IS_LEARNED).toBool() );
        ui.chkWordIsMarked->setChecked ( record.value(DB_F_MAIN_IS_MARKED).toBool() );

        // ui.lblInfo
        {
            // ui.gridLayout->addWidget(ui.lblInfo);

            _plInfoDefault = ui.lblInfo->palette();
        }

        // check word term
        if (!_termNew.isEmpty()) {
            ui.tedtWordTerm->setText(_termNew);
            slot_termCheck();
            slot_termTranslate();
        }

        // ui.bbxButtons
        {
            ui.bbxButtons->setMinimumWidth(BUTTON_BOX_WIDTH);

            QList<QAbstractButton *> buttons = ui.bbxButtons->buttons();

            while ( !buttons.empty() ) {
                buttons.takeFirst()->setMinimumWidth( ui.bbxButtons->minimumWidth() );
            }
        }

        // set focus
        {
            ui.tedtWordTerm->selectAll();
            ui.tedtWordTerm->setFocus();
        }
    }

    _settingsLoad();

    // _isConstructed
    _isConstructed = true;
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::_destruct()
{
    _settingsSave();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
WordEditor::_resetAll()
{
    ui.tedtWordTerm->clear();
    ui.tedtWordBriefValue->clear();
    ui.tedtWordDetailValue->clear();
    ui.cboTags->setCurrentText(tr(""));
    ui.chkWordIsLearned->setChecked(false);
    ui.chkWordIsMarked->setChecked(false);
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::_saveAll(
    QDialog::DialogCode *a_code
)
{
    qCHECK_DO(!slot_termCheck(), return);

    bool bRv        = false;
    int  currentRow = - 1;

    // normilize term, value
    {
        cQString termNorm = ui.tedtWordTerm->toPlainText().trimmed();
        ui.tedtWordTerm->setPlainText(termNorm);

        cQString valueNorm = ui.tedtWordBriefValue->toPlainText().trimmed();
        ui.tedtWordBriefValue->setPlainText(valueNorm);
    }

    // checks
    {
        if (ui.tedtWordTerm->toPlainText().isEmpty()) {
            *a_code = QDialog::Rejected;
            return;
        }
    }

    if (_model->rowCount() > 0) {
        currentRow = _currentRow;
    } else {
        currentRow = 0;

        bRv = _model->insertRow(currentRow);
        qCHECK_PTR(bRv, _model);
    }

    QSqlRecord record = _model->record(currentRow);
    {
        record.setValue(DB_F_MAIN_TERM,       ui.tedtWordTerm->toPlainText());
        record.setValue(DB_F_MAIN_VALUE,      ui.tedtWordBriefValue->toPlainText());
        record.setValue(DB_F_MAIN_TAG,        ui.cboTags->currentText() );
        record.setValue(DB_F_MAIN_IS_LEARNED, ui.chkWordIsLearned->isChecked());
        record.setValue(DB_F_MAIN_IS_MARKED,  ui.chkWordIsMarked->isChecked());
    }

    bRv = _model->setRecord(currentRow, record);
    qCHECK_PTR(bRv, _model);

    bRv = _model->submitAll();
    if (!bRv) {
        QString msg;

        if (19 == _model->lastError().number()) {
            msg = QString(tr("Save fail: term is exists"));
        } else {
            msg = QString(tr("Save fail: %1 - %2"))
                                .arg(_model->lastError().number())
                                .arg(_model->lastError().text());
        }

        QMessageBox::warning(this, qApp->applicationName(), msg);
        _model->revertAll();

        *a_code = QDialog::Rejected;
    } else {
        *a_code = QDialog::Accepted;
    }

    // set current index
    _sqlNavigator->goTo(currentRow);
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::_settingsLoad()
{
    QSize size;
    bool  isTerminLowerCase = false;
    {
        QSettings settings(qS2QS(xlib::core::Application::configPath()), QSettings::IniFormat, this);

        settings.beginGroup("word_editor");
        size              = settings.value("size", QSize(APP_WIDTH, APP_HEIGHT)).toSize();
        isTerminLowerCase = settings.value("termin_lower_case", true).toBool();
        settings.endGroup();
    }

    // apply settings
    {
        // main
        resize(size);
        ui.chkTerminLowerCase->setChecked(isTerminLowerCase);
    }
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::_settingsSave()
{
    QSettings settings(qS2QS(xlib::core::Application::configPath()), QSettings::IniFormat, this);

    // main
    settings.beginGroup("word_editor");
    settings.setValue("size", size());
    settings.setValue("termin_lower_case", ui.chkTerminLowerCase->isChecked());
    settings.endGroup();
}
//-------------------------------------------------------------------------------------------------
bool
WordEditor::_isTermExists(
    cQString &a_term
)
{
    bool      bRv = false;
    QSqlQuery qryQuery( _model->database() );

    cQString sql =
        "SELECT COUNT(*) AS f_records_count "
        "   FROM  " DB_T_MAIN " "
        "   WHERE " DB_F_MAIN_TERM " LIKE '" + a_term.trimmed() + "';";

    bRv = qryQuery.exec(sql);
    qCHECK_REF(bRv, qryQuery);

    bRv = qryQuery.next();
    qCHECK_REF(bRv, qryQuery);

    bRv = qryQuery.value(0).toBool();

    return bRv;
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::_googleTranslate(
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
*   private slots
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
WordEditor::slot_pbtnLangsSwap_OnClicked()
{
    cQString textFrom  = ui.cboLangFrom->currentText();
    cQString textTo    = ui.cboLangTo->currentText();

    int      indexFrom = ui.cboLangFrom->findText(textTo);
    qTEST(indexFrom > - 1);

    int      indexTo   = ui.cboLangTo->findText(textFrom);
    qTEST(indexTo > - 1);

    ::qSwap(indexFrom, indexTo);

    ui.cboLangFrom->setCurrentIndex(indexFrom);
    ui.cboLangTo->setCurrentIndex(indexTo);
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::slot_termTranslate()
{
    if (ui.tedtWordTerm->toPlainText().isEmpty()) {
        ui.tedtWordBriefValue->clear();
        ui.tedtWordDetailValue->clear();

        return;
    }

    cQString textFrom = ui.tedtWordTerm->toPlainText();
    cQString langFrom = ui.cboLangFrom->currentText();
    cQString langTo   = ui.cboLangTo->currentText();
    QString  textToBrief;
    QString  textToDetail;
    QString  textToRaw;

    _googleTranslate(textFrom, langFrom, langTo, &textToBrief, &textToDetail, &textToRaw);

    // term - to lowercase
    if (ui.chkTerminLowerCase->isChecked()) {
        cQString term      = ui.tedtWordTerm->toPlainText();
        cQString termLower = ui.tedtWordTerm->toPlainText().toLower();

        if (term != termLower) {
            ui.tedtWordTerm->setText(termLower);
        }
    } else {
        // n/a
    }

    // value - to lowercase
    if (ui.chkTerminLowerCase->isChecked()) {
        cQString value      = textToBrief;
        cQString valueLower = textToBrief.toLower();

        if (value != valueLower) {
            ui.tedtWordBriefValue->setText(valueLower);
        }
    } else {
        ui.tedtWordBriefValue->setText(textToBrief);
    }

    // value details - n/a lowcase
    ui.tedtWordDetailValue->setText(textToDetail);

    qDebug() << qDEBUG_VAR(textFrom);
    qDebug() << qDEBUG_VAR(textToBrief);
    qDebug() << qDEBUG_VAR(textToRaw);
}
//-------------------------------------------------------------------------------------------------
bool
WordEditor::slot_termCheck()
{
    bool     bRv = false;
    QPalette plInfo;
    QString  msg;

    // is term empty
    bRv = ui.tedtWordTerm->toPlainText().trimmed().isEmpty();
    if (bRv) {
        msg = QString(tr("Termin is an empty"));

        QPalette pallete = ui.lblInfo->palette();
        pallete.setColor(QPalette::WindowText, Qt::red);

        qSwap(plInfo, pallete);

        ui.lblInfo->setPalette(plInfo);
        ui.lblInfo->setText(msg);

        return false;
    }

    // is term exists
    if (ui.chkTerminLowerCase->isChecked()) {
        cQString term      = ui.tedtWordTerm->toPlainText();
        cQString termLower = ui.tedtWordTerm->toPlainText().toLower();

        if (term != termLower) {
            ui.tedtWordTerm->setText(termLower);
        }
    }

    bRv = _isTermExists( ui.tedtWordTerm->toPlainText() );
    if (bRv && _insertMode) {
        // insert: term already exists (false)
        msg = QString(tr("Termin '%1' already exists"))
                            .arg( ui.tedtWordTerm->toPlainText() );

        QPalette pallete = ui.lblInfo->palette();
        pallete.setColor(QPalette::WindowText, Qt::red);

        qSwap(plInfo, pallete);

        ui.lblInfo->setPalette(plInfo);
        ui.lblInfo->setText(msg);

        return false;
    }
    else if (bRv && !_insertMode) {
        // edit: term already exists (true)
    #if 1
        msg = QString(tr("Termin '%1' now editing"))
                            .arg( ui.tedtWordTerm->toPlainText() );

        QPalette pallete = ui.lblInfo->palette();
        pallete.setColor(QPalette::WindowText, Qt::blue);

        qSwap(plInfo, pallete);

        ui.lblInfo->setPalette(plInfo);
        ui.lblInfo->setText(msg);
    #endif

        return true;
    }
    else {
        // ok, term is a new
        msg = QString(tr("Termin '%1' is a new"))
                            .arg( ui.tedtWordTerm->toPlainText() );

        qSwap(plInfo, _plInfoDefault);

        ui.lblInfo->setPalette(plInfo);
        ui.lblInfo->setText(msg);
    }

    return true;
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::slot_bbxButtons_OnClicked(
    QAbstractButton *a_button
)
{
    QDialog::DialogCode code = QDialog::Rejected;

    QDialogButtonBox::StandardButton type = ui.bbxButtons->standardButton(a_button);
    switch (type) {
    case QDialogButtonBox::Reset:
        _resetAll();
        break;
    case QDialogButtonBox::Cancel:
        close();
        break;
    case QDialogButtonBox::Ok:
        _saveAll(&code);
        close();
        break;
    default:
        qTEST(false);
        break;
    }

    setResult(code);
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::slot_WordTermOrValue_OnTextChanged()
{
    ui.tedtWordTerm->document()->setModified(true);
    ui.tedtWordBriefValue->document()->setModified(true);

    cbool flag = ui.tedtWordTerm->document()->isModified() ||
                 ui.tedtWordBriefValue->document()->isModified();

    setWindowModified(flag);
}
//-------------------------------------------------------------------------------------------------
