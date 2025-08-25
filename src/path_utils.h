#pragma once
#include <QCoreApplication>
#include <QDir>
#include <QString>

// مسیر امن: <پوشه‌ی exe>/image/<name>
// (CMake ما پوشه image را کنار exe کپی می‌کند)
inline QString imgPath(QString s) {
    if (s.startsWith(":/")) return s;                 // اگر روزی برگشتی به QRC
    if (QDir::isAbsolutePath(s)) return s;            // مسیر مطلق
    s.replace('\\', '/');
    const QString base = QCoreApplication::applicationDirPath();
    if (s.startsWith("image/")) return QDir(base).filePath(s);
    return QDir(base).filePath("image/" + s);
}
