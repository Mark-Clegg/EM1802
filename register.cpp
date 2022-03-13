#include "register.h"

constexpr uint16_t Register::Mask[5];

Register::Register(QWidget *parent)
    : QWidget(parent)
{
    static QRegularExpression CamelCase = QRegularExpression("([a-z])([A-Z])");
    Layout = new QHBoxLayout(this);
    Layout->setSpacing(6);
    Layout->setContentsMargins(0,0,0,0);

    Identifier = new QLabel(this);
    Identifier->setAlignment(Qt::AlignCenter);
    RegisterValue = new QLineEdit(this);
    RegisterValue->setInputMask("HHHH;0");
    RegisterValue->setPlaceholderText("0000");
    RegisterValue->setFont(QFont("Courier New"));
    RegisterValue->setAlignment(Qt::AlignHCenter);

    LIndicator = new QLabel(this);
    LIndicator->setFixedWidth(5);
    LIndicator->setFixedHeight(30);

    RIndicator = new QLabel(this);
    RIndicator->setFixedWidth(5);
    RIndicator->setFixedHeight(30);

    Layout->addWidget(Identifier);
    Layout->addWidget(LIndicator);
    Layout->addWidget(RegisterValue);
    Layout->addWidget(RIndicator);
    setLayout(Layout);

    Identifier->installEventFilter(this);

    QObject::connect(RegisterValue, &QLineEdit::textEdited, this, &Register::set);
    QObject::connect(this, &Register::objectNameChanged, Identifier, [this](QString Label) { Identifier->setText(Label.replace(CamelCase,"\\1 \\2"));});
}

bool Register::eventFilter( QObject* object, QEvent* event)
{
    if(object == Identifier && event->type() == QEvent::MouseButtonRelease)
        emit doubleClick(Value.Word);
    return false;
}

void Register::set()
{
    bool conversionStatus;
    Value.Word = RegisterValue->displayText().toUInt(&conversionStatus, 16);
    emit valueChanged(Value.Word);
}

void Register::setIdentifierCursor(QCursor Cursor)
{
    Identifier->setCursor(Cursor);
}

int Register::nibbleCount()
{
    return digits;
}

void Register::setNibbleCount(int i)
{
    digits = i;
    RegisterValue->setInputMask(QString::fromStdString(std::string(i,'H'))+";0");
    RegisterValue->setPlaceholderText(QString::fromStdString(std::string(i,'0')));
}

void Register::setStyle(int Digits, int LabelWidth, int ValueWidth, Qt::Alignment Align)
{
    setNibbleCount(Digits);
    Identifier->setMinimumWidth(LabelWidth);
    RegisterValue->setMinimumWidth(ValueWidth);
    Identifier->setAlignment(Align);
}

void Register::setInputMask(char c)
{
    RegisterValue->setInputMask(QString::fromStdString(std::string(digits,c))+";0");
}

uint8_t Register::high()
{
    return Value.HighByte;
}

void Register::setHigh(uint8_t i)
{
    Value.HighByte = i;
    RegisterValue->setText(QString("%1").arg(Value.Word,digits,16,QLatin1Char('0')).toUpper());
    emit valueChanged(Value.Word);
}

uint8_t Register::low()
{
    return Value.LowByte;
}

void Register::setLow(uint8_t i)
{
    Value.LowByte = i;
    RegisterValue->setText(QString("%1").arg(Value.Word,digits,16,QLatin1Char('0')).toUpper());
    emit valueChanged(Value.Word);
}

uint16_t Register::value()
{
    return Value.Word;
}

void Register::setValue(uint16_t i)
{
    Value.Word = i & Mask[digits];
    RegisterValue->setText(QString("%1").arg(Value.Word,digits,16,QLatin1Char('0')).toUpper());
    emit valueChanged(Value.Word);
    }

void Register::operator=(int i)
{
    Value.Word = i & Mask[digits];
    RegisterValue->setText(QString("%1").arg(Value.Word,digits,16,QLatin1Char('0')).toUpper());
    emit valueChanged(Value.Word);
}

Register::operator int()
{
    return Value.Word;
}

void Register::setLIndicator(Qt::GlobalColor c)
{
    QPalette pal = QPalette();

    pal.setColor(QPalette::Window, c);
    LIndicator->setAutoFillBackground(true);
    LIndicator->setPalette(pal);
}

void Register::setRIndicator(Qt::GlobalColor c)
{
    QPalette pal = QPalette();

    pal.setColor(QPalette::Window, c);
    RIndicator->setAutoFillBackground(true);
    RIndicator->setPalette(pal);
}

bool Register::isReadOnly()
{
    return RegisterValue->isReadOnly();
}

void Register::setReadOnly(bool state)
{
    RegisterValue->setReadOnly(state);
}

Register::~Register()
{
    delete Identifier;
    delete RegisterValue;
    delete Layout;
}
