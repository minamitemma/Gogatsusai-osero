#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

#include "mainwindow.h"

namespace
{

QString rankingFilePath()
{
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (basePath.isEmpty()) {
        basePath = QDir::homePath() + "/.reversi_gui";
    }
    return QDir(basePath).filePath("rankings.json");
}

void resetRankingFile()
{
    const QString path = rankingFilePath();
    if (QFile::exists(path)) {
        QFile::remove(path);
    }
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
        resetRankingFile();
        QTextStream(stdout) << "Ranking records reset: " << rankingFilePath() << Qt::endl;
    }
    
    MainWindow window;
    window.setWindowTitle("Reversi Game - 6x6");
    window.resize(800, 700);
    window.show();
    
    return app.exec();
}
