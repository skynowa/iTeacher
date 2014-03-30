/**
 * \file   CWordEditor.cpp
 * \brief
 */


#include "CWordEditor.h"

#include "../QtLib/CUtils.h"
#include "../QtLib/CApplication.h"


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
CWordEditor::CWordEditor(
    QWidget        *a_parent,
    QSqlTableModel *a_tableModel,
    CSqlNavigator  *a_sqlNavigator,
    cQString        &a_newTerm /* = QString() */
) :
    QDialog        (a_parent),
    _tmModel       (a_tableModel),
    _snSqlNavigator(a_sqlNavigator),
    _ciCurrentRow  (a_sqlNavigator->view()->currentIndex().row()),
    _csNewTerm     (a_newTerm.trimmed()),
    _sbInfo        (Q_NULLPTR),
    _plInfoDefault ()
{
    qTEST_PTR(a_parent);
    qTEST_PTR(a_tableModel);
    qTEST_PTR(a_sqlNavigator);
    //// qTEST(- 1  <  _ciCurrentRow);

    _construct();
}
//-------------------------------------------------------------------------------------------------
/*virtual*/
CWordEditor::~CWordEditor()
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
CWordEditor::_construct()
{
    m_Ui.setupUi(this);

    // CMain
    {
        QSqlRecord srRecord = _tmModel->record(_ciCurrentRow);

        // term, value
        m_Ui.tedtWordTerm->setText       ( srRecord.value(DB_F_MAIN_TERM).toString() );
        m_Ui.tedtWordValue->setText      ( srRecord.value(DB_F_MAIN_VALUE).toString() );

        // tags
        {
            QSqlQuery query;
            query.prepare("SELECT " DB_F_TAGS_NAME " FROM " DB_T_TAGS ";");
            bool rv = query.exec();
            qCHECK_REF(rv, query);

            for ( ; query.next(); ) {
                m_Ui.cboTags->addItem( query.value(0).toString() );
            }

            cint index = m_Ui.cboTags->findText( srRecord.value(DB_F_MAIN_TAG).toString() );
            m_Ui.cboTags->setCurrentIndex(index);
        }

        // checkboxes
        m_Ui.chkWordIsLearned->setChecked( srRecord.value(DB_F_MAIN_IS_LEARNED).toBool() );
        m_Ui.chkWordIsMarked->setChecked ( srRecord.value(DB_F_MAIN_IS_MARKED).toBool() );

        // _sbInfo
        {
            _sbInfo = new QStatusBar(this);
            _sbInfo->setSizeGripEnabled(false);

            m_Ui.gridLayout->addWidget(_sbInfo);

            _plInfoDefault = _sbInfo->palette();
        }

        // check word term
        if (!_csNewTerm.isEmpty()) {
            m_Ui.tedtWordTerm->setText(_csNewTerm);
            slot_termCheck();
            slot_termTranslate();
        }
    }

    // signals
    {
        connect(m_Ui.pbtnTermTranslate, SIGNAL( clicked() ),
                 this,                  SLOT  ( slot_termTranslate() ));

        connect(m_Ui.pbtnTermCheck,     SIGNAL( clicked() ),
                 this,                  SLOT  ( slot_termCheck() ));

        connect(m_Ui.bbxButtons,        SIGNAL( clicked(QAbstractButton *) ),
                this,                   SLOT  ( slot_bbxButtons_OnClicked(QAbstractButton *) ));

        connect(m_Ui.tedtWordTerm,      SIGNAL( textChanged() ),
                this,                   SLOT  ( slot_WordTermOrValue_OnTextChanged() ));

        connect(m_Ui.tedtWordValue,     SIGNAL( textChanged() ),
                this,                   SLOT  ( slot_WordTermOrValue_OnTextChanged() ));
    }

    // shortcuts
    {
        // term translate (Ctrl+Return)
        QShortcut *shortcut = new QShortcut(this);
        shortcut->setKey(QKeySequence(Qt::CTRL + Qt::Key_Return));

        connect(shortcut, SIGNAL( activated() ),
                this,     SLOT  ( slot_termTranslate() ));
    }

    _settingsLoad();
}
//-------------------------------------------------------------------------------------------------
void
CWordEditor::_destruct()
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
CWordEditor::_resetAll()
{
    m_Ui.tedtWordTerm->clear();
    m_Ui.tedtWordValue->clear();
    m_Ui.cboTags->setCurrentText("");
    m_Ui.chkWordIsLearned->setChecked(false);
    m_Ui.chkWordIsMarked->setChecked(false);
}
//-------------------------------------------------------------------------------------------------
void
CWordEditor::_saveAll(
    QDialog::DialogCode *a_code
)
{
    bool bRv         = false;
    int  iCurrentRow = - 1;

    // normilize term, value
    {
        cQString termNorm = m_Ui.tedtWordTerm->toPlainText().trimmed();
        m_Ui.tedtWordTerm->setPlainText(termNorm);

        cQString valueNorm = m_Ui.tedtWordValue->toPlainText().trimmed();
        m_Ui.tedtWordValue->setPlainText(valueNorm);
    }

    // checks
    {
        if (m_Ui.tedtWordTerm->toPlainText().isEmpty()) {
            *a_code = QDialog::Rejected;
            return;
        }
    }

    if (_tmModel->rowCount() > 0) {
        iCurrentRow = _ciCurrentRow;
    } else {
        iCurrentRow = 0;

        bRv = _tmModel->insertRow(iCurrentRow);
        qCHECK_PTR(bRv, _tmModel);
    }

    QSqlRecord srRecord = _tmModel->record(iCurrentRow);
    {
        srRecord.setValue(DB_F_MAIN_TERM,       m_Ui.tedtWordTerm->toPlainText());
        srRecord.setValue(DB_F_MAIN_VALUE,      m_Ui.tedtWordValue->toPlainText());
        srRecord.setValue(DB_F_MAIN_TAG,        m_Ui.cboTags->currentText() );
        srRecord.setValue(DB_F_MAIN_IS_LEARNED, m_Ui.chkWordIsLearned->isChecked());
        srRecord.setValue(DB_F_MAIN_IS_MARKED,  m_Ui.chkWordIsMarked->isChecked());
    }

    bRv = _tmModel->setRecord(iCurrentRow, srRecord);
    qCHECK_PTR(bRv, _tmModel);

    bRv = _tmModel->submit();
    if (!bRv) {
        QString sMsg;

        if (19 == _tmModel->lastError().number()) {
            sMsg = QString(tr("Save fail: term is exists"));
        } else {
            sMsg = QString(tr("Save fail: %1 - %2"))
                                .arg(_tmModel->lastError().number())
                                .arg(_tmModel->lastError().text());
        }

        QMessageBox::warning(this, qApp->applicationName(), sMsg);
        _tmModel->revertAll();

        *a_code = QDialog::Rejected;
    } else {
        *a_code = QDialog::Accepted;
    }

    // set current index
    _snSqlNavigator->goTo(iCurrentRow);
}
//-------------------------------------------------------------------------------------------------
void
CWordEditor::_settingsLoad()
{
    QSize szSize;

    {
        QSettings stSettings(CApplication::iniFilePath(), QSettings::IniFormat, this);

        stSettings.beginGroup("word_editor");
        szSize = stSettings.value("size", QSize(APP_WIDTH, APP_HEIGHT)).toSize();
        stSettings.endGroup();
    }

    // apply settings
    {
        // main
        resize(szSize);
    }
}
//-------------------------------------------------------------------------------------------------
void
CWordEditor::_settingsSave()
{
    QSettings stSettings(CApplication::iniFilePath(), QSettings::IniFormat, this);

    // main
    stSettings.beginGroup("word_editor");
    stSettings.setValue("size", size());
    stSettings.endGroup();
}
//-------------------------------------------------------------------------------------------------
bool
CWordEditor::_isTermExists(
    cQString &a_term
)
{
    bool      bRv = false;
    QSqlQuery qryQuery( _tmModel->database() );

    cQString csSql =
                "SELECT COUNT(*) AS f_records_count "
                "   FROM  " DB_T_MAIN " "
                "   WHERE " DB_F_MAIN_TERM " LIKE '" + a_term.trimmed() + "';";

    bRv = qryQuery.exec(csSql);
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
CWordEditor::slot_termTranslate()
{
    m_Ui.tedtWordValue->clear();

    qCHECK_DO(true == m_Ui.tedtWordTerm->toPlainText().isEmpty(), return);

    cQString sTextFrom = m_Ui.tedtWordTerm->toPlainText().toUtf8();
    cQString sLangFrom = QString("en").toUtf8();
    cQString sLangTo   = QString("ru").toUtf8();
    QString  sTextTo   = CUtils::googleTranslate(sTextFrom, sLangFrom, sLangTo);

    m_Ui.tedtWordValue->setText(sTextTo);
}
//-------------------------------------------------------------------------------------------------
bool
CWordEditor::slot_termCheck()
{
    bool     bRv = false;
    QPalette plInfo;
    QString  sMsg;

    // is term empty
    bRv = m_Ui.tedtWordTerm->toPlainText().trimmed().isEmpty();
    if (bRv) {
        sMsg = QString(tr("Termin is an empty"));

        QPalette pallete = _sbInfo->palette();
        pallete.setColor(QPalette::WindowText, Qt::red);

        qSwap(plInfo, pallete);

        _sbInfo->setPalette(plInfo);
        _sbInfo->showMessage(sMsg);

        return false;
    }

    // is term exists
    bRv = _isTermExists( m_Ui.tedtWordTerm->toPlainText() );
    if (bRv) {
        sMsg = QString(tr("Termin '%1' already exists"))
                            .arg( m_Ui.tedtWordTerm->toPlainText() );

        QPalette pallete = _sbInfo->palette();
        pallete.setColor(QPalette::WindowText, Qt::red);

        qSwap(plInfo, pallete);

        _sbInfo->setPalette(plInfo);
        _sbInfo->showMessage(sMsg);

        return false;
    }

    // ok, term is a new
    {
        sMsg = QString(tr("Termin '%1' is a new"))
                            .arg( m_Ui.tedtWordTerm->toPlainText() );

        qSwap(plInfo, _plInfoDefault);

        _sbInfo->setPalette(plInfo);
        _sbInfo->showMessage(sMsg);
    }

    return true;
}
//-------------------------------------------------------------------------------------------------
void
CWordEditor::slot_bbxButtons_OnClicked(
    QAbstractButton *a_button
)
{
    QDialog::DialogCode code = QDialog::Rejected;

    QDialogButtonBox::StandardButton type = m_Ui.bbxButtons->standardButton(a_button);
    switch (type) {
        case QDialogButtonBox::Ok:
        case QDialogButtonBox::Apply:
            // now use _tmModel->submitAll(), remove this check
            qCHECK_DO(!slot_termCheck(), return);
            _saveAll(&code);
            if (QDialogButtonBox::Ok == type) {
                close();
            }
            break;
        case QDialogButtonBox::Reset:
            _resetAll();
            break;
        case QDialogButtonBox::Cancel:
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
CWordEditor::slot_WordTermOrValue_OnTextChanged()
{
    m_Ui.tedtWordTerm->document()->setModified(true);
    m_Ui.tedtWordValue->document()->setModified(true);

    cbool cbFlag = m_Ui.tedtWordTerm->document()->isModified() ||
                   m_Ui.tedtWordValue->document()->isModified();

    setWindowModified(cbFlag);
}
//-------------------------------------------------------------------------------------------------
