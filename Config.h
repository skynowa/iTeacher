/**
 * \file  Config.h
 * \brief config
 */


#ifndef iTeacher_ConfigH
#define iTeacher_ConfigH
//---------------------------------------------------------------------------
// main window
#define CONFIG_APP_NAME                     "iTeacher"
#define CONFIG_ORG_NAME                     ""
#define CONFIG_APP_COMMENT                  "learning english (or other) words"
#define CONFIG_APP_WINDOW_CLASS             "QTool"
#define CONFIG_APP_WIDTH                    700
#define CONFIG_APP_HEIGHT                   600
#define CONFIG_APP_FONT_SIZE_DEFAULT        9
#define CONFIG_APP_SETTINGS_FIE_EXT         ".ini"

// DB tables, fields
#define CONFIG_DB_T_MAIN                    "t_main"

#define CONFIG_DB_F_MAIN_ID                 "f_main_id"
#define CONFIG_DB_F_MAIN_TERM               "f_main_term"
#define CONFIG_DB_F_MAIN_VALUE              "f_main_value"
#define CONFIG_DB_F_MAIN_IS_LEARNED         "f_main_is_learned"
#define CONFIG_DB_F_MAIN_IS_MARKED          "f_main_is_marked"

// audio
#define CONFIG_AUDIO_WORD_FILE_NAME         "word.mp3"
#define CONFIG_AUDIO_TRANSLATION_FILE_NAME  "translation.mp3"

// resources
#define CONFIG_RES_MAIN_ICON                ":/App.ico"

// etc
#define CONFIG_DB_DIR_NAME                  "Db"
#define CONFIG_BACKUP_DIR_NAME              "Backup"
#define CONFIG_TEMP_DIR_NAME                "Temp"
#define CONFIG_DB_FILE_EXT                  ".db"
#define CONFIG_GUID                         CONFIG_APP_NAME"_simple_guid"
#define CONFIG_TABLEVIEW_ROW_HEIGHT         20
#define CONFIG_TRANSLATION_LANG_ENGLISH     "en"
#define CONFIG_TRANSLATION_LANG_RUSSIAN     "ru"
//---------------------------------------------------------------------------
#endif // iTeacher_ConfigH
