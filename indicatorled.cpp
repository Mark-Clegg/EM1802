#include "indicatorled.h"

IndicatorLED::IndicatorLED(QWidget *parent)
    : QWidget(parent)
{
    Layout = new QHBoxLayout(this);
    Layout->setSpacing(6);
    Layout->setContentsMargins(0,0,0,0);

    DefaultImage = new QImage(":/images/LED-Black.png");

    IconOnImage = DefaultImage;
    IconOffImage = DefaultImage;

    Icon = new QLabel(this);
    Icon->setFixedWidth(20);
    Icon->setPixmap(QPixmap::fromImage(*IconOffImage));

    Identifier = new QLabel(this);
    Identifier->setMinimumWidth(40);

    Layout->addWidget(Icon);
    Layout->addWidget(Identifier);
    setLayout(Layout);

    connect(this, &IndicatorLED::objectNameChanged, Identifier, &QLabel::setText);
}

void IndicatorLED::setText(QString s)
{
    Identifier->setText(s);
}

void IndicatorLED::setOnColour(Qt::GlobalColor Colour)
{
    if(IconOnImage != DefaultImage)
        delete(IconOnImage);
    IconOnImage = SelectImage(Colour);
}

void IndicatorLED::setOffColour(Qt::GlobalColor Colour)
{
    if(IconOffImage != DefaultImage)
        delete(IconOffImage);
    IconOffImage = SelectImage(Colour);
}

void IndicatorLED::operator=(bool f)
{
    State = f;
    Icon->setPixmap(QPixmap::fromImage(State ? *IconOnImage : *IconOffImage));
}

IndicatorLED::operator bool()
{
    return State;
}

IndicatorLED::~IndicatorLED()
{
    if(IconOnImage != DefaultImage)
        delete IconOnImage;
    if(IconOffImage != DefaultImage)
        delete IconOffImage;
    delete Layout;
    delete DefaultImage;
    delete Icon;
    delete Identifier;
}

QImage * IndicatorLED::SelectImage(Qt::GlobalColor Colour)
{
    switch(Colour)
    {
    case Qt::red:
        return new QImage(":/images/LED-Red.png");
    case Qt::blue:
        return new QImage(":/images/LED-Blue.png");
    case Qt::green:
        return new QImage(":/images/LED-Green.png");
    default:
        return new QImage(":/images/LED-Black.png");

    }
}
