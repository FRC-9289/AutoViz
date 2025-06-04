#ifndef CSVWRITER_H
#define CSVWRITER_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QVector>

class CsvWriter
{
public:

    bool open(); // Open the file for writing
    void setHeaders(const QStringList &headers);
    void addRow(const QStringList &rowValues);
    bool save(); // Write data to the file
    void clear(); // Clear all rows (excluding headers)
    QVector<QStringList> getCsvRows(const QString &filePath);

private:
    QString m_filePath;
    QStringList m_headers;
    QVector<QStringList> m_dataRows;
};

#endif // CSVWRITER_H
