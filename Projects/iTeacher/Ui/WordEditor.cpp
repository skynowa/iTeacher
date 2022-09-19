/**
 * \file   WordEditor.cpp
 * \brief
 */


#include "WordEditor.h"

#include "../QtLib/Utils.h"
#include "../QtLib/SignalGuard.h"
#include "../QtLib/Application.h"

#include "TagsEditor.h"
#include <xLib/Package/Application.h>
#include <xLib/Package/Translate.h>


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
void
WordEditor::setTerm(
    cQString &a_value
)
{
    _termNew = a_value;
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::retranslate(
    WordEditor *a_dlgWordEditor
)
{
    qCHECK_DO(a_dlgWordEditor == nullptr, return);

    cQString data = QApplication::clipboard()->text();

    a_dlgWordEditor->setTerm(data);
    a_dlgWordEditor->translate();
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
        connect(ui.tbtnTermCopy,  &QPushButton::clicked,
                this,             &WordEditor::tbtnTermCopy_OnClicked);

        connect(ui.tbtnTermPaste, &QPushButton::clicked,
                this,             &WordEditor::tbtnTermPaste_OnClicked);

        connect(ui.tbtnValueCopy, &QPushButton::clicked,
                this,             &WordEditor::tbtnValueCopy_OnClicked);

        connect(ui.tbtnTermValueSwap, &QPushButton::clicked,
                this,             &WordEditor::tbtnTermValueSwap_OnClicked);

        connect(ui.tbtnReset,     &QPushButton::clicked,
                this,             &WordEditor::tbtnReset_OnClicked);

        connect(ui.tbtnTranslate, &QPushButton::clicked,
                this,             &WordEditor::translate);

        connect(ui.tbtnGoogleTranslate, &QPushButton::clicked,
                this,             &WordEditor::googleTranslate);

        connect(ui.tbtnCheck,     &QPushButton::clicked,
                this,             &WordEditor::check);

        connect(ui.pbtnTagsEdit,  &QPushButton::clicked,
                this,             &WordEditor::pbtnTagsEdit_OnClicked);

        connect(ui.bbxButtons,    &QDialogButtonBox::clicked,
                this,             &WordEditor::bbxButtons_OnClicked);
    }

    // shortcuts
    {
        // term translate (Ctrl+Return)
        QShortcut *shortcut = new QShortcut(this);
        shortcut->setKey(QKeySequence(Qt::CTRL + Qt::Key_Return));

        connect(shortcut, &QShortcut::activated,
                this,     &WordEditor::translate);
    }

    // UI
    {
        // title
        {
            QFileInfo info( _sqlNavigator->model()->database().databaseName() );

            cQString title = QString("%1 - %2")
                                .arg( qS2QS(xl::package::Application::info().name) )
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
    }

    _settingsLoad();
    translate();

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
    qCHECK_DO(!check(), return);

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
        // en-ru
        record.setValue(DB_F_MAIN_TERM,       ui.tedtTerm->toPlainText());
        record.setValue(DB_F_MAIN_VALUE,      ui.tedtValueBrief->toPlainText());

        record.setValue(DB_F_MAIN_IS_LEARNED, ui.chkIsLearned->isChecked());
        record.setValue(DB_F_MAIN_IS_MARKED,  ui.chkIsMarked->isChecked());
        record.setValue(DB_F_TAGS_NAME,       ui.cboTags->currentData());
    }

    bRv = _model->setRecord(currentRow, record);
    qCHECK_PTR(bRv, _model);

    bRv = _model->submitAll();
    if (!bRv) {
        QString msg;

        // http://www.sqlite.org/c3ref/c_abort.html
        cint sqliteConstraint = 19;

        switch ( _model->lastError().nativeErrorCode().toInt() ) {
        case sqliteConstraint:
            msg = tr("Term already exists");
            break;
        default:
            msg = QString(tr("Save fail: %1 - %2"))
                        .arg(_model->lastError().number())
                        .arg(_model->lastError().text());
            break;
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
        QSettings settings(qS2QS(xl::package::Application::configPath()), QSettings::IniFormat, this);

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
    QSettings settings(qS2QS(xl::package::Application::configPath()), QSettings::IniFormat, this);

    // main
    settings.beginGroup("word_editor");
    settings.setValue("size", size());
    settings.setValue("position", pos());
    settings.setValue("term_lower_case", ui.chkLowerCase->isChecked());
    settings.endGroup();
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   private slots
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
WordEditor::tbtnTermCopy_OnClicked()
{
    QApplication::clipboard()->setText( ui.tedtTerm->toPlainText() );
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::tbtnTermPaste_OnClicked()
{
    ui.tedtTerm->setText( QApplication::clipboard()->text() );
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::tbtnTermValueSwap_OnClicked()
{
    QString term  = ui.tedtTerm->toPlainText();
    QString value = ui.tedtValueBrief->toPlainText();

    ui.tedtTerm->setText(value);
    ui.tedtValueBrief->setText(term);
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::tbtnReset_OnClicked()
{
    _resetAll();
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::tbtnValueCopy_OnClicked()
{
    QApplication::clipboard()->setText( ui.tedtValueBrief->toPlainText() );
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::translate()
{
    // set focus
    {
        setVisible(true);
        raise();
        activateWindow();

        ui.tedtTerm->selectAll();
        ui.tedtTerm->setFocus();
    }

    // check term
    if (_termNew.isEmpty() && ui.tedtTerm->toPlainText().isEmpty()) {
        return;
    }

    if ( ui.tedtTerm->toPlainText().isEmpty() ) {
        ui.tedtTerm->setText(_termNew);
    }

    _termNew.clear();

    if (ui.tedtTerm->toPlainText().isEmpty()) {
        ui.tedtValueBrief->clear();
        ui.tedtValueDetail->clear();
        ui.tedtValueWeb->clear();
        ui.tedtValueRaw->clear();

        return;
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    check();

    QString  textFrom = ui.tedtTerm->toPlainText();
    QString  textToBrief;
    QString  textToDetail;
    QString  textToRaw;

    // lowercase
    if (ui.chkLowerCase->isChecked()) {
        textFrom = textFrom.toLower();
    }

    {
        xl::package::Translate translate;

        std::tstring_t _textToBrief;
        std::tstring_t _textToDetail;
        std::tstring_t _textToRaw;
        std::tstring_t _langFrom;
        std::tstring_t _langTo;
        translate.run(textFrom.toStdString(), &_textToBrief, &_textToDetail, &_textToRaw,
            &_langFrom, &_langTo);

        // [out]
        textToBrief  = QString::fromStdString(_textToBrief);
        textToDetail = QString::fromStdString(_textToDetail);
        textToRaw    = QString::fromStdString(_textToRaw);
    }

    ui.tedtTerm->setText(textFrom);
    ui.tedtValueBrief->setText(textToBrief);
    ui.tedtValueDetail->setText(textToDetail);
    ui.tedtValueWeb->setHtml(textToRaw);
    ui.tedtValueRaw->setPlainText(textToRaw);

    QApplication::restoreOverrideCursor();
}
//-------------------------------------------------------------------------------------------------
/**
 * TODO: Make common code
 * \see Main::googleTranslate()
 */
void
WordEditor::googleTranslate() const
{
    cQString text      = ui.tedtTerm->toPlainText();
    cQString langFrom  = "en";
    cQString langTo    = "ru";
    cQString operation = "translate";

    cQString url = QString("https://translate.google.com/?sl=%1&tl=%2&op=%3&text=%4")
                        .arg(langFrom)
                        .arg(langTo)
                        .arg(operation)
                        .arg(text);

    QDesktopServices::openUrl( QUrl(url, QUrl::TolerantMode) );
}
//-------------------------------------------------------------------------------------------------
bool
WordEditor::check()
{
    QPalette plInfo;
    QString  msg;

    QString termMinimized;
    {
        termMinimized = ui.tedtTerm->toPlainText().trimmed();
        termMinimized = qS2QS(xl::core::String::minimize(qQS2S(termMinimized),
            TERM_MINIMIZED_SIZE_MAX));
    }

    // is term empty
    const bool bRv = ui.tedtTerm->toPlainText().trimmed().isEmpty();
    if (bRv) {
        msg = QString(tr("Term is an empty"));

        QPalette pallete = ui.lblInfo->palette();
        pallete.setColor(QPalette::WindowText, Qt::red);

        ::qSwap(plInfo, pallete);

        ui.lblInfo->setPalette(plInfo);
        ui.lblInfo->setText(msg);

        return false;
    }

    bool isTermExists {};
    {
        cQString &term = ui.tedtTerm->toPlainText();

        QSqlDatabase db = _model->database();

        SqliteDb sqliteDb(nullptr, &db, static_cast<const qtlib::SqlRelationalTableModelEx &>(*_model));
        isTermExists = sqliteDb.isTerminExists(term);
    }

    if (isTermExists && _insertMode) {
        // insert: term already exists (false)
        msg = QString(tr("Term '%1' already exists")).arg(termMinimized);

        QPalette pallete = ui.lblInfo->palette();
        pallete.setColor(QPalette::WindowText, Qt::red);

        ::qSwap(plInfo, pallete);

        ui.lblInfo->setPalette(plInfo);
        ui.lblInfo->setText(msg);

        return false;
    }
    else if (isTermExists && !_insertMode) {
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
WordEditor::pbtnTagsEdit_OnClicked()
{
    TagsEditor dlgTagsEditor(this, _model->database());
    (int)dlgTagsEditor.exec();
}
//-------------------------------------------------------------------------------------------------
void
WordEditor::bbxButtons_OnClicked(
    QAbstractButton *a_button
)
{
    QDialog::DialogCode code = QDialog::Rejected;

    QDialogButtonBox::StandardButton type = ui.bbxButtons->standardButton(a_button);
    switch (type) {
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
