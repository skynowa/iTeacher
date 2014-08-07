/**
 * \file   WordEditor.cpp
 * \brief
 */


#include "WordEditor.h"

#include "../QtLib/Utils.h"
#include "../QtLib/SignalGuard.h"
#include "../QtLib/Application.h"

#include "TagsEditor.h"
#include <xLib/Core/Application.h>


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
    _translator   (),
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

    GoogleTranslator::Language langFrom;
    GoogleTranslator::Language langTo;
    QString                    langCodeFrom;
    QString                    langCodeTo;

    GoogleTranslator translator;
    translator.languagesDetect(_termNew, &langFrom, &langTo, &langCodeFrom, &langCodeTo);

    // TODO: QComboBox::findText: case-insensitive
    cint indexFrom = ui.cboLangFrom->findText(langCodeFrom);
    qTEST(indexFrom > - 1);

    cint indexTo   = ui.cboLangTo->findText(langCodeTo);
    qTEST(indexTo > - 1);

    ui.cboLangFrom->setCurrentIndex(indexFrom);
    ui.cboLangTo->setCurrentIndex(indexTo);
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

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

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

    _translator.execute(GoogleTranslator::hrPost, textFrom, langFrom, langTo, &textToBrief,
        &textToDetail, &textToRaw);

    ui.tedtTerm->setText(textFrom);
    ui.tedtValueBrief->setText(textToBrief);
    ui.tedtValueDetail->setText(textToDetail);
    ui.tedtValueWeb->setHtml(textToRaw);
    ui.tedtValueRaw->setPlainText(textToRaw);

    QApplication::restoreOverrideCursor();
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
        msg = QString(tr("Term is an empty"));

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
        msg = QString(tr("Term '%1' already exists")).arg(termMinimized);

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
        msg = QString(tr("Term '%1' now editing")).arg(termMinimized);

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
        msg = QString(tr("Term '%1' is a new")).arg(termMinimized);

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
