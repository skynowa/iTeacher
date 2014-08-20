/**
 * \file  Config.h
 * \brief config
 */


#ifndef iTeacher_ConfigH
#define iTeacher_ConfigH
//-------------------------------------------------------------------------------------------------
// main window
#define APP_NAME                "iTeacher"
#define APP_DESCRIPTION         "Learning foreign words"
#define APP_COPYRIGHT_YEARS     "2000-2014"
#define APP_USAGE               ""
#define APP_HELP                ""
#define APP_VERSION_MAJOR       "1"
#define APP_VERSION_MINOR       "0"
#define APP_VERSION_PATCH       "0"
#define APP_VERSION_TYPE        "Beta"
#define APP_VERSION_REVISION    ""
#define APP_VENDOR_NAME         "Skynowa Studio"
#define APP_VENDOR_DOMAIN       ""
#define APP_VENDOR_AUTHOR       "skynowa"
#define APP_VENDOR_URL          "http://bitbucket.org/skynowa/iteacher"
#define APP_VENDOR_EMAIL        "skynowa@gmail.com"
#define APP_VENDOR_SKYPE        "skynowa777"
#define APP_DONATE_PAYPAL       ""
#define APP_DONATE_WEBMONEY     ""
#define APP_DONATE_YANDEXMONEY  ""
#define APP_DONATE_PRIVATE24    ""

#define APP_GUID                APP_NAME"_simple_guid"
#define APP_WINDOW_CLASS        "QTool"
#define APP_WIDTH               700
#define APP_HEIGHT              600
#define APP_FONT_SIZE_DEFAULT   9
#define DB_FILE_NAME_EMPTY      "db_empty.db"

// DB table - t_main
#define DB_T_MAIN               "t_main"
#define DB_F_MAIN_ID            "f_main_id"
#define DB_F_MAIN_TERM          "f_main_term"
#define DB_F_MAIN_VALUE         "f_main_value"
#define DB_F_MAIN_IS_LEARNED    "f_main_is_learned"
#define DB_F_MAIN_IS_MARKED     "f_main_is_marked"
#define DB_F_MAIN_TAG           "f_main_tag"

#define TVMAIN_COLUMN_WIDTH_0   100
#define TVMAIN_COLUMN_WIDTH_1   120
#define TVMAIN_COLUMN_WIDTH_2   400
#define TVMAIN_COLUMN_WIDTH_3   70
#define TVMAIN_COLUMN_WIDTH_4   70
#define TVMAIN_COLUMN_WIDTH_5   100

// DB table - t_tags
#define DB_T_TAGS               "t_tags"
#define DB_F_TAGS_ID            "f_tags_id"
#define DB_F_TAGS_NAME          "f_tags_name"

#define TVTAGS_COLUMN_WIDTH_0   50
#define TVTAGS_COLUMN_WIDTH_1   200

// audio
#define AUDIO_TERM_FILE_NAME    "term.mp3"
#define AUDIO_VALUE_FILE_NAME   "value.mp3"

// etc
#define DB_FILE_EXT             ".db"
#define LANG_EN                 "en"
#define LANG_RU                 "ru"
#define TABLEVIEW_ROW_HEIGHT    20
#define BUTTON_BOX_WIDTH        75
#define TERM_MINIMIZED_SIZE_MAX 30
//-------------------------------------------------------------------------------------------------
#endif // iTeacher_ConfigH
