#ifndef FILEPATHOPERATIONS_H
#define FILEPATHOPERATIONS_H
#include <QString>
#include <QDir>
#include <QSet>

class FilePathOperations
{
public:
    static QString GenerateUniqueFileName(const QDir& output_dir, const QString& input_file_basename
                                          , const QString& ext
                                          , const QSet<QString>& extra = QSet<QString>());
    static QString GenerateUniqueFileName(const QString& filename
                                          , const QSet<QString>& extra = QSet<QString>());
    static QString GenerateTempFileName(const QString& filename);
private:
    FilePathOperations();
};

#endif // FILEPATHOPERATIONS_H
