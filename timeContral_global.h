#ifndef TIMECONTRAL_GLOBAL_H
#define TIMECONTRAL_GLOBAL_H

// #include <QtCore/qglobal.h>
#include <QtGlobal>
#if defined(TIMECONTRAL_LIBRARY)
#define TIMECONTRAL_EXPORT Q_DECL_EXPORT
#else
#define TIMECONTRAL_EXPORT Q_DECL_IMPORT
#endif

#endif // TIMECONTRAL_GLOBAL_H
