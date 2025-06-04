#include "CsvWriter.h"
#include <QtCore/qdebug.h>

bool CsvWriter::open()
{
    QFile file(m_filePath);
    return file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
}

void CsvWriter::setHeaders(const QStringList &headers)
{
    m_headers = headers;
}

void CsvWriter::addRow(const QStringList &rowValues)
{
    m_dataRows.append(rowValues);
}

bool CsvWriter::save()
{
    QFile file(m_filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        return false;
    }

    QTextStream out(&file);
    if (!m_headers.isEmpty()) {
        out << m_headers.join(',') << "\n";
    }

    for (const QStringList &row : m_dataRows) {
        out << row.join(',') << "\n";
    }

    file.close();
    return true;
}


QVector<QStringList> CsvWriter::getCsvRows(const QString &filePath) {
    QVector<QStringList> rows;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file:" << filePath;
        return rows;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;  // Skip blank lines

        QStringList fields = line.split(',', Qt::KeepEmptyParts); // You can also use Qt::SkipEmptyParts
        rows.append(fields);
    }

    file.close();
    return rows;
}

void CsvWriter::clear()
{
    m_dataRows.clear();
}
