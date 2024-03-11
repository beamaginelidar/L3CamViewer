#ifndef IMAGEVIEWERFORM_H
#define IMAGEVIEWERFORM_H

#include <QWidget>

namespace Ui {
class imageViewerForm;
}

class imageViewerForm : public QWidget
{
    Q_OBJECT

public:
    explicit imageViewerForm(QWidget *parent = nullptr);
    ~imageViewerForm();

    void showImage(QImage image);
private:
    Ui::imageViewerForm *ui;
};

#endif // IMAGEVIEWERFORM_H
