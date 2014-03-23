#include "vfmdlinkrefmap.h"

VfmdLinkRefMap::VfmdLinkRefMap()
    : m_map(new VfmdDictionary<LinkData>)
    , m_lastAccessedLinkData(0)
{
}

VfmdLinkRefMap::~VfmdLinkRefMap()
{
    delete m_map;
}

void VfmdLinkRefMap::add(const VfmdByteArray &refId, const VfmdByteArray &url, const VfmdByteArray &title)
{
    if (!m_map->contains(refId)) {
        m_map->insertMulti(refId, new LinkData(url, title));
        m_lastAccessedRefId = VfmdByteArray();
        m_lastAccessedLinkData = 0;
    }
}

VfmdLinkRefMap::LinkData* VfmdLinkRefMap::linkDataForRefId(const VfmdByteArray &refId)
{
    if (refId.isInvalid()) {
        return 0;
    }
    if (m_lastAccessedRefId.isValid() && refId.isEqualTo(m_lastAccessedRefId)) {
        return m_lastAccessedLinkData;
    }
    LinkData *linkData = m_map->value(refId);
    m_lastAccessedRefId = refId;
    m_lastAccessedLinkData = linkData;
    return linkData;
}

bool VfmdLinkRefMap::hasData(const VfmdByteArray &refId)
{
    LinkData *linkData = linkDataForRefId(refId);
    return (linkData != 0);
}

VfmdByteArray VfmdLinkRefMap::linkUrl(const VfmdByteArray &refId)
{
    LinkData *linkData = linkDataForRefId(refId);
    if (linkData) {
        return linkData->url;
    }
    return VfmdByteArray();
}

VfmdByteArray VfmdLinkRefMap::linkTitle(const VfmdByteArray &refId)
{
    LinkData *linkData = linkDataForRefId(refId);
    if (linkData) {
        return linkData->title;
    }
    return VfmdByteArray();
}

// LinkData

VfmdLinkRefMap::LinkData::LinkData(const VfmdByteArray &u, const VfmdByteArray &t)
    : url(u), title(t)
{
}
