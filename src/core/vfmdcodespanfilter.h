#ifndef CODESPANFILTER_H
#define CODESPANFILTER_H

#include "vfmdregexp.h"
#include "vfmdline.h"

class HtmlStateWatcher;

class VfmdCodeSpanFilter
{
public:
    VfmdCodeSpanFilter();

    void reset();
    void addFilteredLineToHtmlStateWatcher(const VfmdByteArray &lineContent, HtmlStateWatcher *watcher);

private:
    unsigned int m_openBackticksCount;
};

#endif // CODESPANFILTER_H
