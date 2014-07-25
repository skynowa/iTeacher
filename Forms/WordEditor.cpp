/**
 * \file   WordEditor.cpp
 * \brief
 */


#include "WordEditor.h"

#include "../QtLib/Utils.h"

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
    cQString            &a_newTerm /* = QString() */
) :
    QDialog       (a_parent),
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
    //// qTEST(- 1  <  _currentRow);

    _construct();
}
//-------------------------------------------------------------------------------------------------
/*virtual*/
WordEditor::~WordEditor()
{
    _destruct();
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
    ui.setupUi(this);

    // signals
    {
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
        ui.tedtWordTerm->setText( record.value(DB_F_MAIN_TERM).toString() );
        ui.tedtWordBriefValue->setText( record.value(DB_F_MAIN_VALUE).toString() );

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


/**************************************************************************************************
*   private slots
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
void
WordEditor::slot_termTranslate()
{
    if (ui.tedtWordTerm->toPlainText().isEmpty()) {
        ui.tedtWordBriefValue->clear();
        ui.tedtWordDetailValue->clear();

        return;
    }

    if (ui.chkTerminLowerCase->isChecked()) {
        cQString term      = ui.tedtWordTerm->toPlainText();
        cQString termLower = ui.tedtWordTerm->toPlainText().toLower();

        if (term != termLower) {
            ui.tedtWordTerm->setText(termLower);
        }
    }

    cQString textFrom = ui.tedtWordTerm->toPlainText().toUtf8();
    cQString langFrom = QString("en").toUtf8();
    cQString langTo   = QString("ru").toUtf8();
    QString  textToBrief;
    QString  textToDetail;
    QString  textToRaw;

    qtlib::Utils::googleTranslate(textFrom, langFrom, langTo, &textToBrief, &textToDetail,
        &textToRaw);

    ui.tedtWordBriefValue->setText(textToBrief);
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
