#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QThread>

#include <fstream>
using namespace  std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(800,430);
    connect(ui->btn_input_browse,&QPushButton::clicked,this,&MainWindow::btn_browse_clicked);
    connect(ui->btn_input_reload,&QPushButton::clicked,this,&MainWindow::btn_reload_clicked);
    connect(ui->btn_output_browse,&QPushButton::clicked,this,&MainWindow::btn_browse_output_clicked);
    connect(ui->btn_process,&QPushButton::clicked,this,&MainWindow::btn_process_clicked);


    input_status = false;
    output_status = false;
    lines_count = 0;


}
void MainWindow::btn_browse_clicked()
{
    input_file_name = QFileDialog::getOpenFileName(this,
                                                    tr("Open SQL File"), "", tr("SQL File (*.sql)"));
    ui->lbl_input_file->setText(input_file_name);
    btn_reload_clicked();

}

void MainWindow::btn_reload_clicked()
{
    input_status = false;
    ifstream in(input_file_name.toStdString());
    if(in.fail())
    {
        ui->lbl_input_status->setText("Error: Can't Open input file");
        return;
    }
    string line;
    lines_count = 0;
    string word;
    while(in>>word&&getline(in,line))
    {
        lines_count++;
        QString q_word = QString(word.c_str());
        q_word = q_word.toLower();
        if(!q_word.isEmpty()&&q_word!="insert")
        {
            ui->lbl_input_status->setText(QString("Error: All lines must be either empty or staring with insert [Line %1]").arg(lines_count+1));
            return;
        }
    }
    ui->lbl_input_status->setText(QString( "Valid SQL file with %1 lines").arg(lines_count));
    input_status = true;
    ui->pb_progress->setMaximum(lines_count);
    in.close();


}
void MainWindow::done(QString res)
{
    ui->lbl_process_status->setText(res);
    workerThread.quit();
    workerThread.wait();
    ui->btn_process->setEnabled(true);


}
void MainWindow::progress(int value)
{
    ui->pb_progress->setValue(value);
}
void MainWindow::btn_browse_output_clicked()
{
    output_status = false;
    QStringList fileNames;
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    if(dialog.exec())
    {
        fileNames = dialog.selectedFiles();
    }
    if(fileNames.length()==0)
    {
        ui->lbl_output_status->setText("Can't select a folder");
        return;
    }
    output_folder_name = fileNames[0];
    ui->lbl_output_folder->setText(output_folder_name);
    output_status = true;
    ui->lbl_output_status->setText("Successfully selected the output folder");
}
void MainWindow::btn_process_clicked()
{
    ui->btn_process->setEnabled(false);
    Worker *worker = new Worker(ui->sb_statements_per_batch->value(),input_file_name);
    worker->moveToThread(&workerThread);
    connect(&workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(&workerThread, &QThread::started, worker, &Worker::doWork);

    connect(worker, &Worker::done, this, &MainWindow::done);
    connect(worker, &Worker::progress, this, &MainWindow::progress);
    workerThread.start();
    ui->lbl_process_status->setText("Proccessing...");
}

MainWindow::~MainWindow()
{
    delete ui;
    workerThread.quit();
    workerThread.wait();
}
Worker::Worker(int stats_to_merge,QString file_name)
{
    this->stats_to_merge = stats_to_merge;
    this->file_name = file_name;
}

void Worker::doWork(){
    ifstream in(file_name.toStdString());
    if(in.fail())
    {
        emit done("Can't open input file");
        return;
    }
    ofstream out(file_name.toStdString()+".combined.sql");
    string line;
    int l_count = 0;
    while(!in.eof())
    {
        getline(in,line);
        line = line.substr(0,line.length()-1);
        out<<line;
        int ct = 0;
        int p1 = line.find('(');
        p1 = line.find('(',p1+1);
        if(p1==-1) continue; //empty line
        while(getline(in,line)&&ct<stats_to_merge)
        {
            line = line.substr(p1);
            line = line.substr(0,line.length()-1);
            out<<" , "<<line;
            out.flush();
            ct++;
            l_count++;
            emit progress(l_count);
        }
        out<<";"<<endl;
    }
    in.close();
    out.flush();
    out.close();
    emit done("Done");



}
