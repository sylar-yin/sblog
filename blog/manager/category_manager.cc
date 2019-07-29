#include "category_manager.h"
#include "sylar/log.h"
#include "sylar/util.h"
#include "blog/util.h"

namespace blog {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

bool CategoryManager::loadAll() {
    auto db = GetDB();
    if(!db) {
        SYLAR_LOG_ERROR(g_logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::CategoryInfo::ptr> results;
    if(blog::data::CategoryInfoDao::QueryAll(results, db)) {
        SYLAR_LOG_ERROR(g_logger) << "CategoryManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, blog::data::CategoryInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<std::string, blog::data::CategoryInfo::ptr> > users;

    for(auto& i : results) {
        datas[i->getId()] = i;
        users[i->getUserId()][i->getName()] = i;
    }

    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    m_users.swap(users);
    return true;
}

void CategoryManager::add(blog::data::CategoryInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
    m_users[info->getUserId()][info->getName()] = info;
}

#define XX(map, key) \
    sylar::RWMutex::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

blog::data::CategoryInfo::ptr CategoryManager::get(int64_t id) {
    XX(m_datas, id);
}

bool CategoryManager::listByUserId(std::vector<blog::data::CategoryInfo::ptr>& infos, int64_t id, bool valid) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto it = m_users.find(id);
    if(it == m_users.end()) {
        return false;
    }
    if(valid) {
        for(auto& i : it->second) {
            if(i.second->getIsDeleted() == 0) {
                infos.push_back(i.second);
            }
        }
    } else {
        for(auto& i : it->second) {
            infos.push_back(i.second);
        }
    }
    return true;
}

bool CategoryManager::exists(int64_t id, const std::string& name) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto it = m_users.find(id);
    if(it == m_users.end()) {
        return false;
    }
    auto iit = it->second.find(name);
    return iit != it->second.end();
}

blog::data::CategoryInfo::ptr CategoryManager::getByUserIdName(int64_t id, const std::string& name) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto it = m_users.find(id);
    if(it == m_users.end()) {
        return nullptr;
    }
    auto iit = it->second.find(name);
    return iit != it->second.end() ? iit->second : nullptr;
}

std::string CategoryManager::statusString() {
    std::stringstream ss;
    sylar::RWMutex::ReadLock lock(m_mutex);
    ss << "CategoryManager total=" << m_datas.size()
       << " users=" << m_users.size() << std::endl;
    lock.unlock();
    return ss.str();
}

#undef XX

}
