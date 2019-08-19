#ifndef __BLOG_MANAGER_CHANNEL_MANAGER_H__
#define __BLOG_MANAGER_CHANNEL_MANAGER_H__

#include "blog/data/channel_info.h"
#include "sylar/singleton.h"
#include "sylar/mutex.h"
#include <map>
#include <unordered_map>

namespace blog {

class ChannelManager {
public:
    bool loadAll();
    void add(blog::data::ChannelInfo::ptr info);
    blog::data::ChannelInfo::ptr get(int64_t id);
    void listAll(std::map<int64_t, data::ChannelInfo::ptr>& infos);
    std::string statusString();
private:
    sylar::RWMutex m_mutex;
    std::unordered_map<int64_t, blog::data::ChannelInfo::ptr> m_datas;
};

typedef sylar::Singleton<ChannelManager> ChannelMgr;

}

#endif
