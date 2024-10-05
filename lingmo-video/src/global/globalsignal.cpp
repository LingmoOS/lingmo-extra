#include "globalsignal.h"

GlobalUserSignal* GlobalUserSignal::instance = new GlobalUserSignal;

GlobalUserSignal *GlobalUserSignal::getInstance()
{
    return instance;
}

GlobalUserSignal::GlobalUserSignal(QObject *parent) : QObject(parent)
{

}

GlobalCoreSignal* GlobalCoreSignal::instance = new GlobalCoreSignal;

GlobalCoreSignal *GlobalCoreSignal::getInstance()
{
    return instance;
}

GlobalCoreSignal::GlobalCoreSignal(QObject *parent) : QObject(parent)
{

}
