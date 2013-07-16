/**
 * \file  Config.h
 * \brief config
 */


#ifndef iTeacher_ConfigH
#define iTeacher_ConfigH
//------------------------------------------------------------------------------
// main window
#define APP_NAME                     "iTeacher"
#define ORG_NAME                     ""
#define APP_COMMENT                  "learning english (or other) words"
#define APP_WINDOW_CLASS             "QTool"
#define APP_WIDTH                    700
#define APP_HEIGHT                   600
#define APP_FONT_SIZE_DEFAULT        9
#define APP_SETTINGS_FILE_EXT        ".ini"

// DB tables, fields
#define DB_T_MAIN                    "t_main"

#define DB_F_MAIN_ID                 "f_main_id"
#define DB_F_MAIN_TERM               "f_main_term"
#define DB_F_MAIN_VALUE              "f_main_value"
#define DB_F_MAIN_IS_LEARNED         "f_main_is_learned"
#define DB_F_MAIN_IS_MARKED          "f_main_is_marked"
#define DB_F_MAIN_TAG                "f_main_tag"

#define DB_T_TAGS                    "t_tags"

#define DB_F_TAGS_ID                 "f_main_id"
#define DB_F_TAGS_NAME               "f_main_naem"

// audio
#define AUDIO_WORD_FILE_NAME         "word.mp3"
#define AUDIO_TRANSLATION_FILE_NAME  "translation.mp3"

// resources
#define RES_MAIN_ICON                ":/App.ico"

// etc
#define DB_DIR_NAME                  "Db"
#define BACKUP_DIR_NAME              "Backup"
#define TEMP_DIR_NAME                "Temp"
#define DB_FILE_EXT                  ".db"
#define APP_GUID                     APP_NAME"_simple_guid"
#define TABLEVIEW_ROW_HEIGHT         20
#define TRANSLATION_LANG_ENGLISH     "en"
#define TRANSLATION_LANG_RUSSIAN     "ru"
//------------------------------------------------------------------------------
#endif // iTeacher_ConfigH
