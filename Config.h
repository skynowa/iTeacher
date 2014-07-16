/**
 * \file  Config.h
 * \brief config
 */


#ifndef iTeacher_ConfigH
#define iTeacher_ConfigH
//-------------------------------------------------------------------------------------------------
// main window
#define APP_NAME                     "iTeacher"
#define APP_GUID                     APP_NAME"_simple_guid"
#define ORG_NAME                     ""
#define APP_COMMENT                  "learning foreign words"
#define APP_WINDOW_CLASS             "QTool"
#define APP_WIDTH                    700
#define APP_HEIGHT                   600
#define APP_FONT_SIZE_DEFAULT        9
#define DB_FILE_NAME_EMPTY           "db_empty.db"

// table view
#define TABLEVIEW_ROW_HEIGHT         20

#define TABLEVIEW_COLUMN_WIDTH_0     100
#define TABLEVIEW_COLUMN_WIDTH_1     120
#define TABLEVIEW_COLUMN_WIDTH_2     400
#define TABLEVIEW_COLUMN_WIDTH_3     70
#define TABLEVIEW_COLUMN_WIDTH_4     70
#define TABLEVIEW_COLUMN_WIDTH_5     100

// DB tables, fields
#define DB_T_MAIN                    "t_main"
#define DB_F_MAIN_ID                 "f_main_id"
#define DB_F_MAIN_TERM               "f_main_term"
#define DB_F_MAIN_VALUE              "f_main_value"
#define DB_F_MAIN_IS_LEARNED         "f_main_is_learned"
#define DB_F_MAIN_IS_MARKED          "f_main_is_marked"
#define DB_F_MAIN_TAG                "f_main_tag"

#define DB_T_TAGS                    "t_tags"
#define DB_F_TAGS_ID                 "f_tags_id"
#define DB_F_TAGS_NAME               "f_tags_name"

// audio
#define AUDIO_WORD_FILE_NAME         "word.mp3"
#define AUDIO_TRANSLATION_FILE_NAME  "translation.mp3"

// resources
#define RES_MAIN_ICON                ":/App.ico"
#define RES_MAIN_PNG                 ":/App.png"

// etc
#define HAVE_XLIB                    1
#define DB_DIR_NAME                  "Db"
#define DB_FILE_EXT                  ".db"
#define BACKUP_DIR_NAME              "Backup"
#define TEMP_DIR_NAME                "Temp"
#define TRANSLATION_LANG_ENGLISH     "en"
#define TRANSLATION_LANG_RUSSIAN     "ru"
#define BUTTON_BOX_WIDTH             75
//-------------------------------------------------------------------------------------------------
#endif // iTeacher_ConfigH
