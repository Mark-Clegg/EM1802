#ifndef INDICATORLED_H
#define INDICATORLED_H
#include <QtWidgets/QLabel>
#include <QtWidgets/QHBoxLayout>
#include <QImage>
#include <QPixmap>

class IndicatorLED : public QWidget
{
    Q_OBJECT

private:
    QHBoxLayout *Layout;
    QLabel *Icon;
    QLabel *Identifier;
    QImage *DefaultImage;
    QImage *IconOnImage = Q_NULLPTR;
    QImage *IconOffImage = Q_NULLPTR;;
    bool State;
    QImage *SelectImage(Qt::GlobalColor);

public:
    explicit IndicatorLED(QWidget *parent = nullptr);
    ~IndicatorLED();

    void setText(QString);

    void setOnColour(Qt::GlobalColor);
    void setOffColour(Qt::GlobalColor);

    void operator=(bool);
    operator bool();
};

#endif // INDICATORLED_H
