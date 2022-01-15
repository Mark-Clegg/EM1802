#include "externalflag.h"

ExternalFlag::ExternalFlag(QWidget *parent)
    : QCheckBox{parent}
{
    this->setTristate(false);
}

void ExternalFlag::operator=(bool f)
{
    setCheckState(f ? Qt::Checked : Qt::Unchecked);
}

ExternalFlag::operator bool()
{
    return checkState() == Qt::Checked ? true : false;;
}

