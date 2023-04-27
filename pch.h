#ifndef STDAFX_H
#define STDAFX_H

// Preprocessor options
// Standard library headers
#include <cstdlib>
#include <array>
// Qt headers
#include <Qt>
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
// Platform headers
#ifdef Q_OS_WINDOWS
#include <Windows.h>
#endif
// Conditional definitions
#ifdef QT_DEBUG
#define ALWAYS_INLINE
#else
#define ALWAYS_INLINE Q_ALWAYS_INLINE
#endif

#define NO_INLINE Q_NEVER_INLINE

static_assert(CHAR_BIT == 8, "Unsupported machine, CHAR_BIT != 8");

#endif // STDAFX_H
