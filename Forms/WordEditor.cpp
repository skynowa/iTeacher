/**
 * \file   WordEditor.cpp
 * \brief
 */


#include "WordEditor.h"

#include "../QtLib/Utils.h"

#include "TagsEditor.h"
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
    cQString            &a_termNew /* = QString() */
) :
    QDialog       (a_parent),
    _isConstructed(false),
    _model        (a_tableModel),
    _sqlNavigator (a_sqlNavigator),
    _currentRow   (a_sqlNavigator->view()->currentIndex().row()),
    _insertMode   (a_insertMode),
    _termNew      (a_termNew.trimmed()),
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
        connect(ui.pbtnLangsSwap, SIGNAL( clicked() ),
                this,             SLOT  ( slot_pbtnLangsSwap_OnClicked() ));

        connect(ui.pbtnTranslate, SIGNAL( clicked() ),
                this,             SLOT  ( slot_translate() ));

        connect(ui.pbtnCheck,     SIGNAL( clicked() ),
                this,             SLOT  ( slot_check() ));

        connect(ui.pbtnTagsEdit,  SIGNAL( clicked() ),
                this,             SLOT  ( slot_pbtnTagsEdit_OnClicked() ));

        connect(ui.bbxButtons,    SIGNAL( clicked(QAbstractButton *) ),
                this,             SLOT  ( slot_bbxButtons_OnClicked(QAbstractButton *) ));
    }

    // shortcuts
    {
        // term translate (Ctrl+Return)
        QShortcut *shortcut = new QShortcut(this);
        shortcut->setKey(QKeySequence(Qt::CTRL + Qt::Key_Return));

        connect(shortcut, SIGNAL( activated() ),
                this,     SLOT  ( slot_translate() ));
    }

    // UI
    {
        // title
        {
            QFileInfo info( _sqlNavigator->model()->database().databaseName() );

            cQString title = QString("%1 - %2")
                                .arg( qS2QS(xlib::core::Application::name()) )
                                .arg( info.fileName() );

            setWindowTitle(title);
        }

        QSqlRecord record = _model->record(_currentRow);

        // term, value
        {
            ui.tedtTerm->setText( record.value(DB_F_MAIN_TERM).toString() );
            ui.tedtValueBrief->setText( record.value(DB_F_MAIN_VALUE).toString() );
            ui.tedtValueDetail->clear();
            ui.tedtValueWeb->clear();
            ui.tedtValueRaw->clear();
        }

        // checkboxes
        ui.chkIsLearned->setChecked( record.value(DB_F_MAIN_IS_LEARNED).toBool() );
        ui.chkIsMarked->setChecked ( record.value(DB_F_MAIN_IS_MARKED).toBool() );

        // tags
        {
            QSqlQuery query;
            query.prepare("SELECT " DB_F_TAGS_ID ", " DB_F_TAGS_NAME " FROM " DB_T_TAGS ";");
            bool rv = query.exec();
            qCHECK_REF(rv, query);

            for ( ; query.next(); ) {
                ui.cboTags->addItem(query.value(1).toString(), query.value(0));
            }

            cint index = ui.cboTags->findText( record.value(DB_F_TAGS_NAME).toString() );
            ui.cboTags->setCurrentIndex(index);
        }

        // ui.lblInfo
        {
            // ui.gridLayout->addWidget(ui.lblInfo);

            _plInfoDefault = ui.lblInfo->palette();
        }

        // check term
        if (!_termNew.isEmpty()) {
            ui.tedtTerm->setText(_termNew);

            slot_check();
            _languagesAutoDetect();
            slot_translate();
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
            ui.tedtTerm->selectAll();
            ui.tedtTerm->setFocus();
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
    ui.tedtTerm->clear();
    ui.tedtValueBrief->clear();
    ui.tedtValueDetail->clear();
    ui.tedtValueWeb->clear();
    ui.tedtValueRaw->clear();
    ui.chkIsLearned->setChecked(false);
    ui.chkIsMarked->setChecked(false);
    ui.cboTags->setCurrentText(tr(""));
    ui.chkLowerCase->setChecked(false);
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::_saveAll(
    QDialog::DialogCode *a_code
)
{
    qTEST(!ui.cboLangFrom->currentText().isEmpty());
    qTEST(ui.cboLangFrom->currentText() == LANG_EN || ui.cboLangFrom->currentText() == LANG_RU);

    qCHECK_DO(!slot_check(), return);

    bool bRv        = false;
    int  currentRow = - 1;

    // normilize term, value
    {
        cQString termNorm = ui.tedtTerm->toPlainText().trimmed();
        ui.tedtTerm->setPlainText(termNorm);

        cQString valueNorm = ui.tedtValueBrief->toPlainText().trimmed();
        ui.tedtValueBrief->setPlainText(valueNorm);
    }

    // checks
    {
        if (ui.tedtTerm->toPlainText().isEmpty()) {
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
        int iRv = ui.cboLangFrom->currentText().compare(LANG_EN, Qt::CaseInsensitive);
        if (iRv == 0) {
            // en-ru
            record.setValue(DB_F_MAIN_TERM,  ui.tedtTerm->toPlainText());
            record.setValue(DB_F_MAIN_VALUE, ui.tedtValueBrief->toPlainText());
        } else {
            // ru-en
            record.setValue(DB_F_MAIN_TERM,  ui.tedtValueBrief->toPlainText());
            record.setValue(DB_F_MAIN_VALUE, ui.tedtTerm->toPlainText());
        }

        record.setValue(DB_F_MAIN_IS_LEARNED, ui.chkIsLearned->isChecked());
        record.setValue(DB_F_MAIN_IS_MARKED,  ui.chkIsMarked->isChecked());
        record.setValue(DB_F_TAGS_NAME,       ui.cboTags->currentData());
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
    QSize  size;
    QPoint position;
    bool   isTerminLowerCase = false;
    {
        QSettings settings(qS2QS(xlib::core::Application::configPath()), QSettings::IniFormat, this);

        settings.beginGroup("word_editor");
        size              = settings.value("size", QSize(APP_WIDTH, APP_HEIGHT)).toSize();
        position          = settings.value("position", QPoint(200, 200)).toPoint();
        isTerminLowerCase = settings.value("term_lower_case", true).toBool();
        settings.endGroup();
    }

    // apply settings
    {
        // main
        resize(size);
        move(position);
        ui.chkLowerCase->setChecked(isTerminLowerCase);
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
    settings.setValue("position", pos());
    settings.setValue("term_lower_case", ui.chkLowerCase->isChecked());
    settings.endGroup();
}
//-------------------------------------------------------------------------------------------------
bool
WordEditor::_isTerminExists(
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
WordEditor::_languagesAutoDetect()
{
    qCHECK_DO(_termNew.isEmpty(), return);

    WordEditor::Language langFrom;
    WordEditor::Language langTo;
    QString              langCodeFrom;
    QString              langCodeTo;
    _googleLanguagesDetect(_termNew, &langFrom, &langTo, &langCodeFrom, &langCodeTo);

    // TODO: QComboBox::findText: case-insensitive
    cint indexFrom = ui.cboLangFrom->findText(langCodeFrom);
    qTEST(indexFrom > - 1);

    cint indexTo   = ui.cboLangTo->findText(langCodeTo);
    qTEST(indexTo > - 1);

    ui.cboLangFrom->setCurrentIndex(indexFrom);
    ui.cboLangTo->setCurrentIndex(indexTo);
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::_googleLanguagesDetect(
    cQString             &a_text,
    WordEditor::Language *a_langFrom,
    WordEditor::Language *a_langTo,
    QString              *a_langCodeFrom,
    QString              *a_langCodeTo
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
        *a_langFrom     = WordEditor::lgEn;
        *a_langTo       = WordEditor::lgRu;

        *a_langCodeFrom = LANG_EN;
        *a_langCodeTo   = LANG_RU;

        qDebug() << "Langs: en-ru\n";
    }
    else if (isRu) {
        *a_langFrom     = WordEditor::lgRu;
        *a_langTo       = WordEditor::lgEn;

        *a_langCodeFrom = LANG_RU;
        *a_langCodeTo   = LANG_EN;

        qDebug() << "Langs: ru-en\n";
    }
    else if (isMixed) {
        qDebug() << "Langs: mixed-mixed\n";

        cbool isPreferEn = (countEn >= countRu);
        cbool isPreferRu = (countRu >  countEn);

        if      (isPreferEn) {
            *a_langFrom     = WordEditor::lgEn;
            *a_langTo       = WordEditor::lgRu;

            *a_langCodeFrom = LANG_EN;
            *a_langCodeTo   = LANG_RU;

            qDebug() << "Langs (prefer): en-ru\n";
        }
        else if (isPreferRu) {
            *a_langFrom     = WordEditor::lgRu;
            *a_langTo       = WordEditor::lgEn;

            *a_langCodeFrom = LANG_RU;
            *a_langCodeTo   = LANG_EN;

            qDebug() << "Langs (prefer): ru-en\n";
        }
        else {
            qTEST(false);
        }
    }
    else if (isUnknown) {
        *a_langFrom     = WordEditor::lgUnknown;
        *a_langTo       = WordEditor::lgUnknown;

        *a_langCodeFrom = "";
        *a_langCodeTo   = "";

        qDebug() << "Langs: unknown-unknown\n";
    }
    else {
        *a_langFrom     = WordEditor::lgUnknown;
        *a_langTo       = WordEditor::lgUnknown;

        *a_langCodeFrom = "";
        *a_langCodeTo   = "";

        qDebug() << qDEBUG_VAR(countEn);
        qDebug() << qDEBUG_VAR(countRu);

        qTEST(false);
    }
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
WordEditor::slot_translate()
{
    if (ui.tedtTerm->toPlainText().isEmpty()) {
        ui.tedtValueBrief->clear();
        ui.tedtValueDetail->clear();
        ui.tedtValueWeb->clear();
        ui.tedtValueRaw->clear();

        return;
    }

    QString  textFrom = ui.tedtTerm->toPlainText();
    cQString langFrom = ui.cboLangFrom->currentText();
    cQString langTo   = ui.cboLangTo->currentText();
    QString  textToBrief;
    QString  textToDetail;
    QString  textToRaw;

    // lowercase
    if (ui.chkLowerCase->isChecked()) {
        textFrom = textFrom.toLower();
    }

    _googleTranslate(textFrom, langFrom, langTo, &textToBrief, &textToDetail, &textToRaw);

    ui.tedtTerm->setText(textFrom);
    ui.tedtValueBrief->setText(textToBrief);
    ui.tedtValueDetail->setText(textToDetail);
    ui.tedtValueWeb->setHtml(textToRaw);
    ui.tedtValueRaw->setPlainText(textToRaw);
}
//-------------------------------------------------------------------------------------------------
bool
WordEditor::slot_check()
{
    bool     bRv = false;
    QPalette plInfo;
    QString  msg;

    QString termMinimized;
    {
        termMinimized = ui.tedtTerm->toPlainText().trimmed();
        termMinimized = qS2QS(xlib::core::String::minimize(qQS2S(termMinimized),
            TERM_MINIMIZED_SIZE_MAX));
    }

    // is term empty
    bRv = ui.tedtTerm->toPlainText().trimmed().isEmpty();
    if (bRv) {
        msg = QString(tr("Termin is an empty"));

        QPalette pallete = ui.lblInfo->palette();
        pallete.setColor(QPalette::WindowText, Qt::red);

        ::qSwap(plInfo, pallete);

        ui.lblInfo->setPalette(plInfo);
        ui.lblInfo->setText(msg);

        return false;
    }

    // is term exists
    bRv = _isTerminExists( ui.tedtTerm->toPlainText() );
    if (bRv && _insertMode) {
        // insert: term already exists (false)
        msg = QString(tr("Termin '%1' already exists")).arg(termMinimized);

        QPalette pallete = ui.lblInfo->palette();
        pallete.setColor(QPalette::WindowText, Qt::red);

        ::qSwap(plInfo, pallete);

        ui.lblInfo->setPalette(plInfo);
        ui.lblInfo->setText(msg);

        return false;
    }
    else if (bRv && !_insertMode) {
        // edit: term already exists (true)
    #if 1
        msg = QString(tr("Termin '%1' now editing")).arg(termMinimized);

        QPalette pallete = ui.lblInfo->palette();
        pallete.setColor(QPalette::WindowText, Qt::blue);

        ::qSwap(plInfo, pallete);

        ui.lblInfo->setPalette(plInfo);
        ui.lblInfo->setText(msg);
    #endif

        return true;
    }
    else {
        // ok, term is a new
        msg = QString(tr("Termin '%1' is a new")).arg(termMinimized);

        ::qSwap(plInfo, _plInfoDefault);

        ui.lblInfo->setPalette(plInfo);
        ui.lblInfo->setText(msg);
    }

    return true;
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::slot_pbtnTagsEdit_OnClicked()
{
    TagsEditor dlgTagsEditor(this, _model->database());
    (int)dlgTagsEditor.exec();
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
