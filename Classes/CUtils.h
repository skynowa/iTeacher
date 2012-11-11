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
        static bool setApplicationSingle(const QString &applicationGuid);
            ///< set application single inststance
        static void widgetAlignCenter   (QWidget *widget);
            ///<
        static void widgetAlignTopCenter(QWidget *widget);
            ///<
        static void applicationActivate (const QString &className, const QString &windowName);
            ///<

    private:
                    CUtils              ();
                   ~CUtils              ();
};
//---------------------------------------------------------------------------
#endif // iTeacher_CUtilsH
