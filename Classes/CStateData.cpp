#include "CStateData.h"
#include <QtDebug>
//---------------------------------------------------------------------------
QDebug
operator << (
    QDebug            debug,
    const CStateData &checkerState
)
{
    QStyle::State state = checkerState.state();
    debug << "CheckerState::State(";

    QStringList states;

    if (state & QStyle::State_Active) states <<        QLatin1String("Active");
    if (state & QStyle::State_AutoRaise) states <<     QLatin1String("AutoRaise");
    if (state & QStyle::State_Bottom) states <<        QLatin1String("Bottom");
    if (state & QStyle::State_Children) states <<      QLatin1String("Children");
    if (state & QStyle::State_DownArrow) states <<     QLatin1String("DownArrow");
    if (state & QStyle::State_Editing) states <<       QLatin1String("Editing");
    if (state & QStyle::State_Enabled) states <<       QLatin1String("Enabled");
    if (state & QStyle::State_FocusAtBorder) states << QLatin1String("FocusAtBorder");
    if (state & QStyle::State_HasFocus) states <<      QLatin1String("HasFocus");
    if (state & QStyle::State_Horizontal) states <<    QLatin1String("Horizontal");
    if (state & QStyle::State_Item) states <<          QLatin1String("Item");
    if (state & QStyle::State_KeyboardFocusChange) states << QLatin1String("KeyboardFocusChange");
    if (state & QStyle::State_MouseOver) states <<     QLatin1String("MouseOver");
    if (state & QStyle::State_NoChange) states <<      QLatin1String("NoChange");
    if (state & QStyle::State_Off) states <<           QLatin1String("Off");
    if (state & QStyle::State_On) states <<            QLatin1String("On");
    if (state & QStyle::State_Open) states <<          QLatin1String("Open");
    if (state & QStyle::State_Raised) states <<        QLatin1String("Raised");
    if (state & QStyle::State_ReadOnly) states <<      QLatin1String("ReadOnly");
    if (state & QStyle::State_Selected) states <<      QLatin1String("Selected");
    if (state & QStyle::State_Sibling) states <<       QLatin1String("Sibling");
    if (state & QStyle::State_Sunken) states <<        QLatin1String("Sunken");
    if (state & QStyle::State_Top) states <<           QLatin1String("Top");
    if (state & QStyle::State_UpArrow) states <<       QLatin1String("UpArrow");

    qSort(states);
    debug << states.join(QLatin1String(" | "));
    debug << ")";

    return debug;
}
//---------------------------------------------------------------------------

