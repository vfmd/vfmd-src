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
    void addFilteredLineToHtmlStateWatcher(const VfmdLine &line, HtmlStateWatcher *watcher);

private:
    const VfmdRegexp m_reCodeSpanMark;
    unsigned int m_openBackticksCount;
};

#endif // CODESPANFILTER_H
