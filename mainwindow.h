#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class Worker : public QObject {
    Q_OBJECT
private:
    int stats_to_merge;
    QString file_name;
    int line_num;
public:
    Worker(int stats_to_merge,QString file_name);
public slots:
    void doWork();
signals:
    void done(QString result);
    void progress(int value);
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void btn_browse_clicked();
    void btn_reload_clicked();
    void btn_browse_output_clicked();
    void btn_process_clicked();

    void done(QString res);
    void progress(int value);

private:
    Ui::MainWindow *ui;
    QString input_file_name;
    QString output_folder_name;
    bool input_status;
    bool output_status;
    int lines_count;
    QThread workerThread;

};
#endif // MAINWINDOW_H
