#include "imageviewerform.h"
#include "ui_imageviewerform.h"

imageViewerForm::imageViewerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::imageViewerForm)
{
    ui->setupUi(this);

    ui->label_image->setBackgroundRole(QPalette::Base);
    ui->label_image->setScaledContents(true);
}

imageViewerForm::~imageViewerForm()
{
    delete ui;
}

void imageViewerForm::showImage(QImage image)
{
    ui->label_image->clear();
    QPixmap image_pix_map = QPixmap::fromImage(image);
    ui->label_image->setPixmap(image_pix_map);
    ui->label_image->adjustSize();
}
