/**
 * \file   CMain.h
 * \brief  main widget
 */


#ifndef iTeacher_CMainH
#define iTeacher_CMainH
//---------------------------------------------------------------------------
#include "../Config.h"

#include <xLib/Common/xCommon.h>
#include "../QtLib/Common.h"
#include "../QtLib/CUtils.h"
#include "../QtLib/CSqlNavigator.h"
#include <Phonon/AudioOutput>
#include <Phonon/MediaObject>

#include "ui_CMain.h"
//---------------------------------------------------------------------------
class CMain :
    public QMainWindow
{
        Q_OBJECT

    public:
                               CMain                     (QWidget *parent = 0, Qt::WFlags flags = 0);
        virtual               ~CMain                     ();

        Ui::CMainClass         m_Ui;
        QString                m_sAppName;
        QString                m_sAppDir;
        QString                m_sDbDir;
        QString                m_sDbBackupDir;
        QString                m_sTempDir;
        CSqlNavigator          m_navNavigator;

    protected:
        virtual bool           eventFilter               (QObject *obj, QEvent *ev);
        virtual void           keyPressEvent             (QKeyEvent *ev);

    private:
        void                   _construct                ();
        void                   _destruct                 ();
        void                   _initMain                 ();
        void                   _initModel                ();
        void                   _initActions              ();

    private Q_SLOTS:
        // group "File"
        void                   slot_OnCreateDb           ();
        void                   slot_OnImportCsv          ();
        void                   slot_OnImportClipboard    ();
        void                   slot_OnExportCsv          ();
        void                   slot_OnExportPdf          ();
        void                   slot_OnExit               ();

        // group "Edit"
        void                   slot_OnFirst              ();
        void                   slot_OnPrior              ();
        void                   slot_OnNext               ();
        void                   slot_OnLast               ();
        void                   slot_OnTo                 ();
        void                   slot_OnInsert             ();
        void                   slot_OnRemove             ();
        void                   slot_OnEdit               ();
        void                   slot_OnPost               ();
        void                   slot_OnCancel             ();
        void                   slot_OnRefresh            ();

        // group "Audio"
        void                   slot_OnPlayWord           ();
        void                   slot_OnPlayTranslation    ();
        void                   slot_OnPlayWordTranslation();

        // group "Find"
        void                   slot_OnSearch             ();

        // group "View"
        void                   slot_OnZoomIn             ();
        void                   slot_OnZoomOut            ();
        void                   slot_OnZoomDefault        ();


        // group "Options"
        void                   slot_OnSettings           ();

        // group "Help"
        void                   slot_OnFaq                ();
        void                   slot_OnAbout              ();

        void                   slot_cboDictionaryPath_OnCurrentIndexChanged(const QString &arg);

        // Audio
        void                   slot_audioFile_OnFinished (QNetworkReply *reply);
        void                   slot_audio_OnRemove       ();

    private:
        void                   cboDictionaryPath_reload  ();

        // DB
        QSqlDatabase          *_m_dbDatabase;
        QSqlTableModel        *_m_tmModel;

        void                   dbOpen                    (const QString &filePath);
        void                   dbReopen                  (const QString &filePath);
        void                   dbClose                   ();

        // audio
        QNetworkAccessManager *_m_nmAudioFile;
        Phonon::MediaObject   *_m_moPlayer;

        void                   _googleSpeech             (const QString &textFrom, const QString &langTo);

        // settings
        void                   _settingsLoad             ();
        void                   _settingsSave             ();
};
//---------------------------------------------------------------------------
#endif // iTeacher_CMainH
