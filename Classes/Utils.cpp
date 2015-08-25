/**
 * \file   Utils.cpp
 * \brief
 */


#include "Utils.h"


namespace iteacher
{

/**************************************************************************************************
*   private
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
Utils::Utils()
{
}
//-------------------------------------------------------------------------------------------------
/* virtual */
Utils::~Utils()
{
}
//-------------------------------------------------------------------------------------------------


/**************************************************************************************************
*   public
*
**************************************************************************************************/

//-------------------------------------------------------------------------------------------------
/* static */
bool
Utils::isTerminExists(
    const QSqlDatabase &a_database,
    cQString           &a_term
)
{
    bool      bRv = false;
    QSqlQuery qryQuery(a_database);

    cQString sql =
        "SELECT COUNT(*) AS f_records_count "
        "   FROM  " DB_T_MAIN " "
        "   WHERE " DB_F_MAIN_TERM " LIKE '" + a_term.trimmed() + "';";

    bRv = qryQuery.exec(sql);
    qCHECK_REF(bRv, qryQuery);

    bRv = qryQuery.next();
    qCHECK_REF(bRv, qryQuery);

    bRv = qryQuery.value(0).toBool();

    return bRv;
}
//-------------------------------------------------------------------------------------------------

} // namespace qtlib
