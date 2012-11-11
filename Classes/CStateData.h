#ifndef iTeacher_CStateData_H
#define iTeacher_CStateData_H
//---------------------------------------------------------------------------
#include <QtCore/QMetaType>
#include <QtGui/QStyle>
#include <QDebug>
//---------------------------------------------------------------------------
class CStateData
{
    public:
                            CStateData  () {}
                            CStateData  (QStyle::State state)      { State = state; }
                            CStateData  (QStyle::StateFlag state)  { State = state; }
                            CStateData  (const CStateData & other) { State = other.State; }
                           ~CStateData  () {}

        QStyle::State       state       () const { return State; }
        void                setState    (QStyle::State state) { State = state; }

        inline CStateData & operator =  (const CStateData &state) { State = state.State; return *this; }
        inline              operator int() const { return (int)State; }
        inline              operator QStyle::State() const { return State; }

        inline CStateData & operator |= (CStateData state) { State |= state.State; return *this; }
        inline CStateData & operator ^= (CStateData state) { State ^= state.State; return *this; }
        inline CStateData & operator &= (int mask)         { State &= mask; return *this; }
        inline CStateData & operator &= (uint mask)        { State &= mask; return *this; }

        inline CStateData   operator |  (CStateData state) const { CStateData tmp; tmp.State = State | state.State; return tmp; }
        inline CStateData   operator ^  (CStateData state) const { CStateData tmp; tmp.State = State ^ state.State; return tmp; }
        inline CStateData   operator &  (int mask)  const { CStateData tmp; tmp.State = State & mask; return tmp; }
        inline CStateData   operator &  (uint mask) const { CStateData tmp; tmp.State = State & mask; return tmp; }
        inline CStateData   operator ~  () const { CStateData tmp; tmp.State = ~State; return tmp; }

        inline bool         operator !  () const { return !State; }

        inline bool         testFlag    (QStyle::StateFlag f) const { return (State & f) == f && (f != 0 || State == int(f) ); }

    private:
        QStyle::State       State;
};
//---------------------------------------------------------------------------
Q_DECLARE_METATYPE(CStateData)

QDebug operator << (QDebug debug, const CStateData & checkerState);
//---------------------------------------------------------------------------
#endif // iTeacher_CStateData_H
