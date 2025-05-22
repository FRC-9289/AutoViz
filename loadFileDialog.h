#include <QObject>
#include <QString>
#include <QFileDIalog>

class LoadFileDialog : public QObject {
    Q_OBJECT
public:
    explicit LoadFileDialog(QObject* parent = nullptr) : QObject(parent){}

    Q_INVOKABLE QString openFolderDialog() {
        QString dir = QFileDialog::getExistingDirectory(
            nullptr,
            "Select Folder",
            QString()
        );
        return dir;
    }
};
