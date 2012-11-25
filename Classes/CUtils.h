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
        static bool    setApplicationSingle(const QString &applicationGuid);
            ///< set application single inststance
        static void    widgetAlignCenter   (QWidget *widget);
            ///<
        static void    widgetAlignTopCenter(QWidget *widget);
            ///<
        static void    applicationActivate (const QString &className, const QString &windowName);
            ///<
        static void    importCsv           (const QString &filePath, QSqlTableModel *sqlTableModel,
                                            const QVector<QString> &a_fieldNames, const QString &columnSeparator);
            ///< import CSV file to DB
        static QString googleTranslate     (const QString &textFrom, const QString &langTo);
            ///< translate text by Google API

    private:
                       CUtils              ();
                      ~CUtils              ();
};
//---------------------------------------------------------------------------
#endif // iTeacher_CUtilsH
