/**
 * \file   CUtils.h
 * \brief
 */


#ifndef iTeacher_CUtilsH
#define iTeacher_CUtilsH
//---------------------------------------------------------------------------
#include "../QtLib/Common.h"
//---------------------------------------------------------------------------
class CUtils {
    public:
        // GUI
        static bool         setApplicationSingle (const QString &applicationGuid);
            ///< set application single inststance
        static void         widgetAlignCenter    (QWidget *widget);
            ///< align main widget
        static void         widgetAlignTopCenter (QWidget *widget);
            ///< align main widget
        static void         applicationActivate  (const QString &className, const QString &windowName);
            ///< activate main application window


        // DB
        static int          sqlTableModelRowCount(QSqlTableModel *model);
            ///< DB row count
        static void         importCsv            (const QString &filePath, QSqlTableModel *sqlTableModel,
                                                  const QVector<QString> &a_fieldNames, const QString &columnSeparator);
            ///< import DB to CSV file
        static void         exportCsv            (const QString &filePath, QSqlTableModel *sqlTableModel,
                                                  const QVector<QString> &a_fieldNames, const QString &columnSeparator);
            ///< export CSV file to DB


        // web
        static QString      googleTranslate      (const QString &textFrom, const QString &langFrom, const QString &langTo);
            ///< translate text by Google API


        // converters
        static std::wstring toStdWString         (const QString &str);
            ///< convert a QString to an std::wstring
        static QString      fromStdWString       (const std::wstring &str);
            ///< convert an std::wstring to a QString

    private:
                            CUtils               ();
                           ~CUtils               ();
};
//---------------------------------------------------------------------------
#endif // iTeacher_CUtilsH
