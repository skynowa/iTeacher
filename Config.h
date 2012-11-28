/**
 * \file  Config.h
 * \brief config
 */


#ifndef iTeacher_ConfigH
#define iTeacher_ConfigH
//---------------------------------------------------------------------------
//#undef UNICODE
//#undef _UNICODE

// main window
#define CONFIG_APP_NAME                     "iTeacher"
#define CONFIG_ORG_NAME                     ""
#define CONFIG_APP_COMMENT                  "learning english (or other) words"
#define CONFIG_APP_WINDOW_CLASS             "QTool"
#define CONFIG_APP_WIDTH                    700
#define CONFIG_APP_HEIGHT                   600

// DB tables, fields
#define CONFIG_DB_T_MAIN                    "t_main"

#define CONFIG_DB_F_MAIN_ID                 "f_main_id"
#define CONFIG_DB_F_MAIN_TERM               "f_main_term"
#define CONFIG_DB_F_MAIN_VALUE              "f_main_value"
#define CONFIG_DB_F_MAIN_IS_LEARNED         "f_main_is_learned"
#define CONFIG_DB_F_MAIN_IS_MARKED          "f_main_is_marked"

// resources
#define CONFIG_RES_MAIN_ICON                ":/App.ico"

// etc
#define CONFIG_BACKUP_DIR_NAME              "Backups"
#define CONFIG_DB_FILE_EXT                  ".db"
#define CONFIG_GUID                         CONFIG_APP_NAME"_simple_guid"
#define CONFIG_TABLEVIEW_ROW_HEIGHT         20
//---------------------------------------------------------------------------
#endif // iTeacher_ConfigH
