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
    cint            &a_currentRow,
    cQString        &a_newTerm /* = QString() */
) :
    QDialog         (a_parent),
    _m_tmModel      (a_tableModel),
    _m_ciCurrentRow (a_currentRow),
    _m_csNewTerm    (a_newTerm.trimmed()),
    _m_sbInfo       (NULL),
    _m_plInfoDefault()
{
    Q_ASSERT(NULL != _m_tmModel);
    Q_ASSERT(- 1  <  _m_ciCurrentRow);

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

        // lables
        m_Ui.tedtWordTerm->setText       ( srRecord.value(DB_F_MAIN_TERM).toString() );
        m_Ui.tedtWordValue->setText      ( srRecord.value(DB_F_MAIN_VALUE).toString() );
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

        connect(m_Ui.tedtWordTerm,      SIGNAL( textChanged()),
                this,                   SLOT  ( slot_WordTermOrValue_OnTextChanged() ));

        connect(m_Ui.tedtWordValue,     SIGNAL( textChanged()),
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
    m_Ui.chkWordIsLearned->setChecked(false);
    m_Ui.chkWordIsMarked->setChecked(false);
}
//------------------------------------------------------------------------------
void
CWordEditor::_saveAll() {
    QSqlRecord srRecord = _m_tmModel->record(_m_ciCurrentRow);

    {
        srRecord.setValue(DB_F_MAIN_TERM,       m_Ui.tedtWordTerm->toPlainText());
        srRecord.setValue(DB_F_MAIN_VALUE,      m_Ui.tedtWordValue->toPlainText());
        srRecord.setValue(DB_F_MAIN_IS_LEARNED, m_Ui.chkWordIsLearned->isChecked());
        srRecord.setValue(DB_F_MAIN_IS_MARKED,  m_Ui.chkWordIsMarked->isChecked());
    }

    _m_tmModel->setRecord(_m_ciCurrentRow, srRecord);

    bool bRv = _m_tmModel->submitAll();
    if (!bRv) {
        cQString csMsg = QString(tr("Save fail: %1"))
                            .arg(_m_tmModel->lastError().text());

        QMessageBox::warning(this, qApp->applicationName(), csMsg);
    }
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
                "   WHERE " DB_F_MAIN_TERM " = '" + a_term.trimmed() + "';";

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

    // check for term existing
    slot_termCheck();
}
//------------------------------------------------------------------------------
void
CWordEditor::slot_termCheck() {
    // check for term existing
    cbool cbIsTermExists = _isTermExists( m_Ui.tedtWordTerm->toPlainText() );

    // format info message
    QPalette plInfo;
    QString  sMsg;
    {
        if (cbIsTermExists) {
            sMsg = QString(tr("The word '%1' already exists"))
                                .arg( m_Ui.tedtWordTerm->toPlainText() );

            QPalette pallete = _m_sbInfo->palette();
            pallete.setColor(QPalette::WindowText, Qt::red);

            qSwap(plInfo, pallete);
        } else {
            sMsg = QString(tr("The word '%1' is a new"))
                                .arg( m_Ui.tedtWordTerm->toPlainText() );

            qSwap(plInfo, _m_plInfoDefault);
        }
    }

    Q_ASSERT(NULL != _m_sbInfo);
    _m_sbInfo->setPalette(plInfo);
    _m_sbInfo->showMessage(sMsg);
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
        case QDialogButtonBox::Apply: {
            cbool bRv1 = m_Ui.tedtWordTerm->toPlainText().trimmed().isEmpty();
            // cbool bRv2 = _isTermExists( m_Ui.tedtWordTerm->toPlainText() );
            if (bRv1 /*|| bRv2*/) {
                slot_termCheck();
            } else {
                _saveAll();

                if (QDialogButtonBox::Ok == sbType) {
                    close();
                }
            }}
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

    cbool cbFlag =
        m_Ui.tedtWordTerm->document()->isModified() ||
        m_Ui.tedtWordValue->document()->isModified();

    setWindowModified(cbFlag);
}
//------------------------------------------------------------------------------
