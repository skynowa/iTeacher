/**
 * \file   CWordEditor.cpp
 * \brief
 */


#include "CWordEditor.h"

#include "../QtLib/CUtils.h"


/*******************************************************************************
*   public
*
*******************************************************************************/

//------------------------------------------------------------------------------
CWordEditor::CWordEditor(
    QWidget        *a_parent,
    QSqlTableModel *a_tableModel,
    CSqlNavigator  *a_sqlNavigator,
    cQString        &a_newTerm /* = QString() */
) :
    QDialog          (a_parent),
    _m_tmModel       (a_tableModel),
    _m_snSqlNavigator(a_sqlNavigator),
    _m_ciCurrentRow  (a_sqlNavigator->view()->currentIndex().row()),
    _m_csNewTerm     (a_newTerm.trimmed()),
    _m_sbInfo        (NULL),
    _m_plInfoDefault ()
{
    Q_ASSERT(NULL != a_parent);
    Q_ASSERT(NULL != a_tableModel);
    Q_ASSERT(NULL != a_sqlNavigator);
    //// Q_ASSERT(- 1  <  _m_ciCurrentRow);

    _construct();
}
//------------------------------------------------------------------------------
/*virtual*/
CWordEditor::~CWordEditor() {
    _destruct();
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   private
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CWordEditor::_construct() {
    m_Ui.setupUi(this);

    // CMain
    {
        QSqlRecord srRecord = _m_tmModel->record(_m_ciCurrentRow);

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

        // _m_sbInfo
        {
            _m_sbInfo = new QStatusBar(this);
            _m_sbInfo->setSizeGripEnabled(false);

            m_Ui.gridLayout->addWidget(_m_sbInfo);

            _m_plInfoDefault = _m_sbInfo->palette();
        }

        // check word term
        if (!_m_csNewTerm.isEmpty()) {
            m_Ui.tedtWordTerm->setText(_m_csNewTerm);
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
//------------------------------------------------------------------------------
void
CWordEditor::_destruct() {
    _settingsSave();
}
//------------------------------------------------------------------------------


/*******************************************************************************
*   private
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CWordEditor::_resetAll() {
    m_Ui.tedtWordTerm->clear();
    m_Ui.tedtWordValue->clear();
    m_Ui.cboTags->setCurrentText("");
    m_Ui.chkWordIsLearned->setChecked(false);
    m_Ui.chkWordIsMarked->setChecked(false);
}
//------------------------------------------------------------------------------
void
CWordEditor::_saveAll() {
    bool bRv         = false;
    int  iCurrentRow = - 1;

    if (_m_tmModel->rowCount() > 0) {
        iCurrentRow = _m_ciCurrentRow;
    } else {
        iCurrentRow = 0;

        bRv = _m_tmModel->insertRow(iCurrentRow);
        qCHECK_PTR(bRv, _m_tmModel);
    }

    QSqlRecord srRecord = _m_tmModel->record(iCurrentRow);

    {
        srRecord.setValue(DB_F_MAIN_TERM,       m_Ui.tedtWordTerm->toPlainText());
        srRecord.setValue(DB_F_MAIN_VALUE,      m_Ui.tedtWordValue->toPlainText());
        srRecord.setValue(DB_F_MAIN_TAG,        m_Ui.cboTags->currentText() );
        srRecord.setValue(DB_F_MAIN_IS_LEARNED, m_Ui.chkWordIsLearned->isChecked());
        srRecord.setValue(DB_F_MAIN_IS_MARKED,  m_Ui.chkWordIsMarked->isChecked());
    }

    _m_tmModel->setRecord(iCurrentRow, srRecord);

    bRv = _m_tmModel->submit();
    if (!bRv) {
        QString sMsg;

        if (19 == _m_tmModel->lastError().number()) {
            sMsg = QString(tr("Save fail: term is exists"));
        } else {
            sMsg = QString(tr("Save fail: %1 - %2"))
                                .arg(_m_tmModel->lastError().number())
                                .arg(_m_tmModel->lastError().text());
        }

        QMessageBox::warning(this, qApp->applicationName(), sMsg);
        _m_tmModel->revertAll();
    }

    // set current index
    _m_snSqlNavigator->goTo(iCurrentRow);
}
//------------------------------------------------------------------------------
void
CWordEditor::_settingsLoad() {
    QSize szSize;

    {
        QSettings stSettings(QCoreApplication::applicationName() + APP_SETTINGS_FILE_EXT,
                             QSettings::IniFormat, this);

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
//------------------------------------------------------------------------------
void
CWordEditor::_settingsSave() {
    QSettings stSettings(QCoreApplication::applicationName() + APP_SETTINGS_FILE_EXT,
                         QSettings::IniFormat, this);

    // main
    stSettings.beginGroup("word_editor");
    stSettings.setValue("size", size());
    stSettings.endGroup();
}
//------------------------------------------------------------------------------
bool
CWordEditor::_isTermExists(
    cQString &a_term
)
{
    bool      bRv = false;
    QSqlQuery qryQuery( _m_tmModel->database() );

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
//------------------------------------------------------------------------------


/*******************************************************************************
*   private slots
*
*******************************************************************************/

//------------------------------------------------------------------------------
void
CWordEditor::slot_termTranslate() {
    m_Ui.tedtWordValue->clear();

    qCHECK_DO(true == m_Ui.tedtWordTerm->toPlainText().isEmpty(), return);

    cQString sTextFrom = m_Ui.tedtWordTerm->toPlainText().toUtf8();
    cQString sLangFrom = QString("en").toUtf8();
    cQString sLangTo   = QString("ru").toUtf8();
    QString  sTextTo   = CUtils::googleTranslate(sTextFrom, sLangFrom, sLangTo);

    m_Ui.tedtWordValue->setText(sTextTo);
}
//------------------------------------------------------------------------------
bool
CWordEditor::slot_termCheck() {
    bool     bRv = false;
    QPalette plInfo;
    QString  sMsg;

    // is term empty
    bRv = m_Ui.tedtWordTerm->toPlainText().trimmed().isEmpty();
    if (bRv) {
        sMsg = QString(tr("The word '%1' already exists"))
                            .arg( m_Ui.tedtWordTerm->toPlainText() );

        QPalette pallete = _m_sbInfo->palette();
        pallete.setColor(QPalette::WindowText, Qt::red);

        qSwap(plInfo, pallete);

        _m_sbInfo->setPalette(plInfo);
        _m_sbInfo->showMessage(sMsg);

        return false;
    }

    // is term exists
    bRv = _isTermExists( m_Ui.tedtWordTerm->toPlainText() );
    if (bRv) {
        sMsg = QString(tr("The word '%1' already exists"))
                            .arg( m_Ui.tedtWordTerm->toPlainText() );

        QPalette pallete = _m_sbInfo->palette();
        pallete.setColor(QPalette::WindowText, Qt::red);

        qSwap(plInfo, pallete);

        _m_sbInfo->setPalette(plInfo);
        _m_sbInfo->showMessage(sMsg);

        return false;
    }

    // ok, term is a new
    {
        sMsg = QString(tr("The word '%1' is a new"))
                            .arg( m_Ui.tedtWordTerm->toPlainText() );

        qSwap(plInfo, _m_plInfoDefault);

        _m_sbInfo->setPalette(plInfo);
        _m_sbInfo->showMessage(sMsg);
    }

    return true;
}
//------------------------------------------------------------------------------
void
CWordEditor::slot_bbxButtons_OnClicked(
    QAbstractButton *a_button
)
{
    QDialogButtonBox::StandardButton sbType = m_Ui.bbxButtons->standardButton(a_button);
    switch (sbType) {
        case QDialogButtonBox::Ok:
        case QDialogButtonBox::Apply:
            // now use _m_tmModel->submitAll(), remove this check
            // qCHECK_DO(!slot_termCheck(), return);
            _saveAll();
            if (QDialogButtonBox::Ok == sbType) {
                close();
            }
            break;
        case QDialogButtonBox::Reset:
            _resetAll();
            break;
        case QDialogButtonBox::Cancel:
            _m_tmModel->revertAll();
            close();
            break;
        default:
            Q_ASSERT(false);
            break;
    }
}
//------------------------------------------------------------------------------
void
CWordEditor::slot_WordTermOrValue_OnTextChanged() {
    m_Ui.tedtWordTerm->document()->setModified(true);
    m_Ui.tedtWordValue->document()->setModified(true);

    cbool cbFlag = m_Ui.tedtWordTerm->document()->isModified() ||
                   m_Ui.tedtWordValue->document()->isModified();

    setWindowModified(cbFlag);
}
//------------------------------------------------------------------------------
