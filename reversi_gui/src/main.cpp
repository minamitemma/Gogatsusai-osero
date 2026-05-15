#include <QApplication>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

#include "mainwindow.h"

namespace
{

QString rankingFilePath()
{
    return QDir(QDir::homePath() + "/.reversi_gui").filePath("rankings.json");
}

QString legacyRankingFilePath()
{
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (basePath.isEmpty()) {
        return QString();
    }
    return QDir(basePath).filePath("rankings.json");
}

bool removeFileIfExists(const QString &path)
{
    if (path.isEmpty() || !QFile::exists(path)) {
        return true;
    }
    return QFile::remove(path);
}

bool resetRankingFile()
{
    return removeFileIfExists(rankingFilePath()) && removeFileIfExists(legacyRankingFilePath());
}

} // namespace

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("ReversiGUI");

    QCommandLineParser parser;
    parser.setApplicationDescription("6x6 Reversi GUI");
    parser.addHelpOption();

    QCommandLineOption resetOption(
        "reset",
        "Delete saved ranking records before starting."
    );
    parser.addOption(resetOption);
    parser.process(app);

    if (parser.isSet(resetOption)) {
        if (resetRankingFile()) {
            QTextStream(stdout) << "Ranking records reset: " << rankingFilePath() << Qt::endl;
        } else {
            QTextStream(stderr) << "Failed to reset ranking records: " << rankingFilePath() << Qt::endl;
        }
    }
    
    MainWindow window;
    window.setWindowTitle("Reversi Game - 6x6");
    window.resize(800, 700);
    window.show();
    
    return app.exec();
}
