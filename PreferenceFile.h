#ifndef PREFERENCE_FILE_H
#define PREFERENCE_FILE_H

class QPoint;
class QSize;
class QString;

int readPreferenceFile(const QString& filePath);

const QPoint& position();

const QSize& resolution();

void writePreferenceFile(const QString& filePath);

#endif // PREFERENCE_FILE_H
