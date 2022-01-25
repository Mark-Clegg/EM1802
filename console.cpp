#include "console.h"
#include "ui_console.h"

Console::Console(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::Console)
{
    ui->setupUi(this);
    ui->ScreenBuffer->installEventFilter(this);

    QTextDocument *doc = ui->ScreenBuffer->document();
    QFont font = doc->defaultFont();
    font.setFamily("Courier New");
    doc->setDefaultFont(font);
}

bool Console::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        QString Keys = keyEvent->text();
        if(Keys.size() > 0)
            emit Input(Keys.front());
        return true;
    }
    else
        return QObject::eventFilter(obj, event);
}

void Console::Output(QChar c)
{
    if(c == '\r' || c == '\0')
        return;

    QString s(c);
    ui->ScreenBuffer->moveCursor(QTextCursor::End);
    ui->ScreenBuffer->insertPlainText(s);
    ui->ScreenBuffer->moveCursor (QTextCursor::End);
}

void Console::Clear()
{
    ui->ScreenBuffer->clear();
}

Console::~Console()
{
    delete ui;
}
