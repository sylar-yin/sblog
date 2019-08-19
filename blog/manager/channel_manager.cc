#include "channel_manager.h"
#include "sylar/log.h"
#include "sylar/util.h"
#include "blog/util.h"

namespace blog {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

bool ChannelManager::loadAll() {
    auto db = GetDB();
    if(!db) {
        SYLAR_LOG_ERROR(g_logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::ChannelInfo::ptr> results;
    if(blog::data::ChannelInfoDao::QueryAll(results, db)) {
        SYLAR_LOG_ERROR(g_logger) << "ChannelManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, blog::data::ChannelInfo::ptr> datas;
    for(auto& i : results) {
        datas[i->getId()] = i;
    }

    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    return true;
}

void ChannelManager::add(blog::data::ChannelInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
}

#define XX(map, key) \
    sylar::RWMutex::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

blog::data::ChannelInfo::ptr ChannelManager::get(int64_t id) {
    XX(m_datas, id);
}

std::string ChannelManager::statusString() {
    std::stringstream ss;
    sylar::RWMutex::ReadLock lock(m_mutex);
    ss << "ChannelManager total=" << m_datas.size();
    lock.unlock();
    return ss.str();
}

void ChannelManager::listAll(std::map<int64_t, data::ChannelInfo::ptr>& infos) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    for(auto& i : m_datas) {
        infos[i.first] = i.second;
    }
}

#undef XX

}
