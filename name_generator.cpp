#include "name_generator.h"

void NameGenerator::setPrefix(const QString& prefix)
{
    m_prefix = prefix;
}

void NameGenerator::setMode(Mode mode)
{
    m_mode = mode;
}

QString NameGenerator::generate(const QFileInfo& fileInfo, int sequenceNumber) const
{
    QString extension = fileInfo.suffix();
    QString number = QString::number(sequenceNumber).rightJustified(3, '0');

    QString newBase;
    if (m_mode == PrefixMode)
        newBase = m_prefix + number;
    else
        newBase = number + m_prefix;

    if (extension.isEmpty())
        return newBase;
    return newBase + "." + extension;
}
