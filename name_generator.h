#ifndef NAME_GENERATOR_H
#define NAME_GENERATOR_H

#include <QString>
#include <QFileInfo>

class NameGenerator
{
public:
    enum Mode
    {
        PrefixMode,
        SuffixMode
    };

    void setPrefix(const QString& prefix);
    void setMode(Mode mode);

    QString generate(const QFileInfo& fileInfo, int sequenceNumber) const;

private:
    QString m_prefix;
    Mode m_mode = PrefixMode;
};

#endif
