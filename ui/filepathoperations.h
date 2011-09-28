#ifndef FILEPATHOPERATIONS_H
#define FILEPATHOPERATIONS_H
#include <QString>
#include <QDir>

class FilePathOperations
{
public:
    static QString GenerateUniqueFileName(const QDir& output_dir, const QString& input_file_basename
                                          , const QString& ext);
    static QString GenerateUniqueFileName(const QString& filename);
    static QString GenerateTempFileName(const QString& filename);
private:
    FilePathOperations();
};

#endif // FILEPATHOPERATIONS_H
