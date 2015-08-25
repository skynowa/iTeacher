/**
 * \file   Utils.h
 * \brief
 */


#pragma once

#include <QtSql/qsqltablemodel.h>
#include "../Config.h"
#include "../QtLib/Common.h"
//-------------------------------------------------------------------------------------------------
namespace iteacher
{

class Utils
    /// utils
{
public:
    // GUI
    static
    bool isTerminExists(const QSqlDatabase &database, cQString &term);
        ///< is termin exists

private:
         Utils();
        ~Utils();
};

} // namespace iteacher
//-------------------------------------------------------------------------------------------------
