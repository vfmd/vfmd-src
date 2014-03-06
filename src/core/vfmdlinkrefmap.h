#ifndef VFMDLINKREFMAP_H
#define VFMDLINKREFMAP_H

#include "vfmddictionary.h"

class VfmdLinkRefMap
{
public:
    VfmdLinkRefMap();
    ~VfmdLinkRefMap();

    // add() adds only if refId does not exist in the map
    void add(const VfmdByteArray &refId, const VfmdByteArray &url, const VfmdByteArray &title);

    // access data
    bool hasData(const VfmdByteArray &refId) const;
    VfmdByteArray linkUrl(const VfmdByteArray &refId);
    VfmdByteArray linkTitle(const VfmdByteArray &refId);

private:
    struct LinkData {
        LinkData(const VfmdByteArray &u, const VfmdByteArray &t);
        VfmdByteArray url, title;
    };

    LinkData *linkDataForRefId(const VfmdByteArray &refId);

    VfmdDictionary<LinkData> *m_map;
    VfmdByteArray m_lastAccessedRefId;
    LinkData *m_lastAccessedLinkData;
};

#endif // VFMDLINKREFMAP_H
