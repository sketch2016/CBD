#ifndef IMAGEBROSWER_H
#define IMAGEBROSWER_H

#include <QDialog>
#include <QFileSystemModel>

namespace Ui {
class ImageBroswer;
}

class ImageBroswer : public QDialog
{
    Q_OBJECT

public:
    explicit ImageBroswer(QWidget *parent = 0);
    ~ImageBroswer();

    void applyRootpath(QString &path);
    void updateData(QString &filename);
    void startShow();

private slots:
    void on_filelist_clicked(const QModelIndex &index);

    void on_filelist_activated(const QModelIndex &index);

    void on_pre_clicked();

    void on_next_clicked();

private:
    Ui::ImageBroswer *ui;
    QString rootpath;
    QFileSystemModel *model;
};

#endif // IMAGEBROSWER_H
