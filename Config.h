/**
 * \file  Config.h
 * \brief config
 */


#pragma once

//-------------------------------------------------------------------------------------------------
// Options
#define OPTION_QTMULTIMEDIA 0 // TODO: QSound
//-------------------------------------------------------------------------------------------------
#include <Qt>
#include <QVariant>

#if OPTION_QTMULTIMEDIA
    #include <QSound>
#endif
//-------------------------------------------------------------------------------------------------
// main window
#define APP_NAME                 "iTeacher"
#define APP_DESCRIPTION          "Learning foreign words"
#define APP_COPYRIGHT_YEARS      QString("2000-%1").arg( QDate::currentDate().year() ).toStdString()
#define APP_USAGE                ""
#define APP_HELP                 ""
#define APP_VERSION_MAJOR        "1"
#define APP_VERSION_MINOR        "0"
#define APP_VERSION_PATCH        "0"
#define APP_VERSION_TYPE         "Beta"
#define APP_VERSION_REVISION     ""
#define APP_VENDOR_NAME          "Skynowa Studio"
#define APP_VENDOR_DOMAIN        ""
#define APP_VENDOR_AUTHOR        "skynowa"
#define APP_VENDOR_URL           "http://bitbucket.org/skynowa/iteacher"
#define APP_VENDOR_EMAIL         "skynowa@gmail.com"
#define APP_VENDOR_SKYPE         "skynowa777"
#define APP_VENDOR_JABBER        "skynowa@jabber.ru"
#define APP_VENDOR_ICQ           "627713628"
#define APP_DONATE_PAYPAL        ""
#define APP_DONATE_WEBMONEY      ""
#define APP_DONATE_YANDEXMONEY   ""
#define APP_DONATE_PRIVATE24     ""

#define APP_GUID                 APP_NAME"_simple_guid"
#define APP_WIDTH                700
#define APP_HEIGHT               600
#define APP_FONT_SIZE_DEFAULT    9
#define DB_FILE_NAME_EMPTY       "db_empty.db"

// DB table - t_main
#define DB_T_MAIN                "t_main"
#define DB_F_MAIN_ID             "f_main_id"
#define DB_F_MAIN_TERM           "f_main_term"
#define DB_F_MAIN_VALUE          "f_main_value"
#define DB_F_MAIN_IS_LEARNED     "f_main_is_learned"
#define DB_F_MAIN_IS_MARKED      "f_main_is_marked"
#define DB_F_MAIN_TAG            "f_main_tag"

struct TableViewHeader
{
    int             section;
    const QVariant &value;
};

const TableViewHeader tableViewHeaders[]
{
    {0, QObject::tr("Id")},
    {1, QObject::tr("Term")},
    {2, QObject::tr("Value")},
    {3, QObject::tr("Learned")},
    {4, QObject::tr("Marked")},
    {5, QObject::tr("Tag")}
};

#define TVMAIN_COLUMN_WIDTH_0    100
#define TVMAIN_COLUMN_WIDTH_1    120
#define TVMAIN_COLUMN_WIDTH_2    400
#define TVMAIN_COLUMN_WIDTH_3    70
#define TVMAIN_COLUMN_WIDTH_4    70
#define TVMAIN_COLUMN_WIDTH_5    100

// DB table - t_tags
#define DB_T_TAGS                "t_tags"
#define DB_F_TAGS_ID             "f_tags_id"
#define DB_F_TAGS_NAME           "f_tags_name"
#define DB_ALL_TAGS_ID           "1"

#define TVTAGS_COLUMN_WIDTH_0    50
#define TVTAGS_COLUMN_WIDTH_1    200

// QSettings fields
#define  CFG_GROUP_MAIN          "main"
#define  CFG_SIZE                "size"
#define  CFG_POSITION            "position"
#define  CFG_VISIBLE             "visible"
#define  CFG_DICTIONARY_NUM      "dictionary_num"

#define  CFG_GROUP_TABLE         "table"
#define  CFG_FONT_SIZE           "font_size"
#define  CFG_ROW_HEIGHT          "row_height"
#define  CFG_CURRENT_ROW         "current_row"
#define  CFG_COLUMN_WIDTH_0      "column_width_0"
#define  CFG_COLUMN_WIDTH_1      "column_width_1"
#define  CFG_COLUMN_WIDTH_2      "column_width_2"
#define  CFG_COLUMN_WIDTH_3      "column_width_3"
#define  CFG_COLUMN_WIDTH_4      "column_width_4"
#define  CFG_COLUMN_WIDTH_5      "column_width_5"

#define  CFG_GROUP_FILE          "file"
#define  CFG_IMPORT_EXPORT_ORDER "import_export_order"
#define  CFG_HIDE_ON_CLOSE       "hide_on_close"

#define  CFG_GROUP_SHORTCUTS     "shortcuts"
#define  CFG_SHOW_HIDE           "show_hide"
#define  CFG_QUICK_CLIPBOARD_TRANSLATE "quick_clipboard_translate"
#define  CFG_CLIPBOARD_IMPORT    "clipboard_import"

// audio
#define OPTION_QMEDIA_PLAYER     0
#define AUDIO_TERM_FILE_NAME     "term.mp3"
#define AUDIO_VALUE_FILE_NAME    "value.mp3"

// tray
#define QSYSTEM_TRAYICON_SIZE    QSize(32, 32)

// etc
#define DB_FILE_EXT              ".db"
#define CSV_SEPARATOR            "#"
#define TABLEVIEW_ROW_HEIGHT     20
#define BUTTON_BOX_WIDTH         75
#define TERM_MINIMIZED_SIZE_MAX  30
//-------------------------------------------------------------------------------------------------
