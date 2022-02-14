/**
 * \file  Config.h
 * \brief config
 */


#pragma once

//-------------------------------------------------------------------------------------------------
#include <Qt>
#include <QVariant>
//-------------------------------------------------------------------------------------------------
// main window
#define APP_TRAINER_NAME         "Trainer"
#define APP_TRAINER_GUID         APP_TRAINER_NAME"_simple_guid"
#define APP_WIDTH                700
#define APP_HEIGHT               600
#define APP_FONT_SIZE_DEFAULT    9

// DB table
#define DB_FILE_EXT              ".db"
#define DB_F_MAIN_ID             "ID"
#define DB_F_MAIN_TERM           "Term"
#define DB_F_MAIN_VALUE          "Value"
#define DB_F_MAIN_IS_LEARNED     "Learned"
#define DB_F_MAIN_IS_MARKED      "Marked"
#define DB_F_MAIN_TAG            "Tag"

struct TableViewHeader
{
    int            section;
    const QVariant value;
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

// DB table - Tags
#define DB_T_TAGS                "Tags"
#define DB_F_TAGS_ID             "ID"
#define DB_F_TAGS_NAME           "Name"
#define DB_ALL_TAGS_ID           "1"

// QSettings (Trainer)
#define CFG_TRAINER_GROUP_MAIN  "trainer"
#define CFG_TRAINER_SIZE        "size"
#define CFG_TRAINER_POSITION    "position"
//-------------------------------------------------------------------------------------------------
