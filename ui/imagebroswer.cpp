#include "include/ui/imagebroswer.h"
#include "ui_imagebroswer.h"
#include "include/util/logger.h"

#define TAG "ImageBroswer"

ImageBroswer::ImageBroswer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageBroswer)
{
    ui->setupUi(this);
    setFixedSize(this->width(),this->height());
    model = new QFileSystemModel();
    // 设置过滤器
    QStringList filter;
    filter << "*.png" << "*.jpg" << "*.bmp" << "*.gif" << "*.jpeg" << "*.webp";
    model->setNameFilters(filter);

}

ImageBroswer::~ImageBroswer()
{
    delete ui;
}

void ImageBroswer::applyRootpath(QString &path) {
    rootpath = path;

    model->setRootPath(path);
    ui->filelist->setModel(model);
    ui->filelist->setRootIndex(model->index(path));
    ui->filelist->setColumnHidden(1, true);
    ui->filelist->setColumnHidden(2, true);
    ui->filelist->setColumnHidden(3, true);

//    QModelIndex current = ui->filelist->indexAt(QPoint(0, 0));
//    QString filename = current.data().toString();
//    ui->filename->setText(filename);
}

void ImageBroswer::updateData(QString &filename) {
  //to do
}

void ImageBroswer::startShow() {
    //show();
    exec();
    //LOGD(TAG, "startShow, filename=%1", filename);
}


void ImageBroswer::on_filelist_clicked(const QModelIndex &index)
{
    int pos = index.row();
    int column = index.column();
    QString filename = index.data().toString();
    ui->filename->setText(filename);
    LOGD(TAG, "on_filelist_clicked, filename=%1 row=%2 column=%3", filename , pos, column);

    QString path = filename;
    if (filename.endsWith(".png") || filename.endsWith(".jpg") || filename.endsWith(".jpeg") || filename.endsWith(".bmp") || filename.endsWith(".webp")) {
        QModelIndex parent = index.parent();
        while (parent.isValid()) {
            path = parent.data().toString() + "/" + path;
            parent = parent.parent();
        }

        //QString path = "./" + index.parent().parent().data().toString() + "/" + index.parent().data().toString() + "/" + filename;
        LOGD(TAG, "on_filelist_clicked, path=%1", path);

        QPixmap pixmap;
        pixmap.load(path);
        pixmap = pixmap.scaled(ui->img->size(), Qt::KeepAspectRatio);
        ui->img->setScaledContents(true);
        ui->img->setPixmap(pixmap);
    } else {
        ui->img->clear();
    }

}

void ImageBroswer::on_filelist_activated(const QModelIndex &index)
{
    int pos = index.row();
    QString filename = index.data().toString();
    LOGD(TAG, "on_filelist_activated, filename=%1 row=%2", filename , pos);
}

void ImageBroswer::on_pre_clicked()
{
    QModelIndex current = ui->filelist->currentIndex();
    QModelIndex index = ui->filelist->indexAbove(current);
    if (index.isValid()) {
        on_filelist_clicked(index);
        ui->filelist->setCurrentIndex(index);
    }

}

void ImageBroswer::on_next_clicked()
{
    QModelIndex current = ui->filelist->currentIndex();
    QModelIndex index = ui->filelist->indexBelow(current);
    if (index.isValid()) {
        on_filelist_clicked(index);
        ui->filelist->setCurrentIndex(index);
    }
}
