#include "article_manager.h"
#include "sylar/log.h"
#include "sylar/util.h"
#include "blog/util.h"
#include "blog/struct.h"
#include "sylar/db/redis.h"

namespace blog {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

bool ArticleManager::loadAll() {
    auto db = GetDB();
    if(!db) {
        SYLAR_LOG_ERROR(g_logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::ArticleInfo::ptr> results;
    if(blog::data::ArticleInfoDao::QueryAll(results, db)) {
        SYLAR_LOG_ERROR(g_logger) << "ArticleManager loadAll fail";
        return false;
    }

    std::map<int64_t, blog::data::ArticleInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<int64_t, blog::data::ArticleInfo::ptr> > users;
    std::map<int64_t, blog::data::ArticleInfo::ptr> verifys;

    for(auto& i : results) {
        datas[i->getId()] = i;
        users[i->getUserId()][i->getId()] = i;
        if(i->getState() == 1) {
            verifys[i->getId()] = i;
        }
    }

    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    m_users.swap(users);
    m_verifys.swap(verifys);
    return true;
}

void ArticleManager::add(blog::data::ArticleInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
    m_users[info->getUserId()][info->getId()] = info;
    if(info->getState() == 1
            && info->getIsDeleted() == 0) {
        m_verifys[info->getId()] = info;
    }
}

void ArticleManager::delVerify(int64_t id) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_verifys.erase(id);
}

void ArticleManager::addVerify(data::ArticleInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_verifys[info->getId()] = info;
}

#define XX(map, key) \
    sylar::RWMutex::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

blog::data::ArticleInfo::ptr ArticleManager::get(int64_t id) {
    XX(m_datas, id);
}

bool ArticleManager::listByUserId(std::vector<data::ArticleInfo::ptr>& infos, int64_t id, bool valid) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto it = m_users.find(id);
    if(it == m_users.end()) {
        return false;
    }
    for(auto& i : it->second) {
        if(!valid || !i.second->getIsDeleted()) {
            infos.push_back(i.second);
        }
    }
    return true;
}

int64_t ArticleManager::listByUserIdPages(std::vector<data::ArticleInfo::ptr>& infos, int64_t id
                                          ,int32_t offset, int32_t size, bool valid, int state) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    if(id == 0) {
        if(offset >= (int32_t)m_datas.size()) {
            return m_datas.size();
        }
        auto it = m_datas.rbegin();
        std::advance(it, offset);
        for(; (int32_t)infos.size() < size && it != m_datas.rend(); ++it) {
            if(!valid || !it->second->getIsDeleted()) {
                if(!state || it->second->getState() == state) {
                    infos.push_back(it->second);
                }
            }
        }
        return m_datas.size();
    } else {
        auto uit = m_users.find(id);
        if(uit == m_users.end()) {
            return 0;
        }
        if(offset >= (int32_t)uit->second.size()) {
            return uit->second.size();
        }
        auto it = uit->second.rbegin();
        std::advance(it, offset);
        for(; (int32_t)infos.size() < size && it != uit->second.rend(); ++it) {
            if(!valid || !it->second->getIsDeleted()) {
                if(!state || it->second->getState() == state) {
                    infos.push_back(it->second);
                }
            }
        }
        return uit->second.size();
    }
}

int64_t ArticleManager::listVerifyPages(std::vector<data::ArticleInfo::ptr>& infos
                                        ,int32_t offset, int32_t size) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    if(offset >= (int32_t)m_verifys.size()) {
        return m_verifys.size();
    }
    auto it = m_verifys.begin();
    std::advance(it, offset);
    std::vector<int64_t> invalids;
    for(; (int32_t)infos.size() < size && it != m_verifys.end(); ++it) {
        if(it->second->getIsDeleted()) {
            invalids.push_back(it->first);
            continue;
        }
        if(it->second->getState() != 1) {
            invalids.push_back(it->first);
            continue;
        }
        infos.push_back(it->second);
    }
    int64_t total = m_verifys.size();
    lock.unlock();
    for(auto& i : invalids) {
        delVerify(i);
    }
    return total;
}

std::string ArticleManager::statusString() {
    std::stringstream ss;
    sylar::RWMutex::ReadLock lock(m_mutex);
    ss << "ArticleManager total=" << m_datas.size()
       << " verify=" << m_verifys.size()
       << std::endl;
    for(auto& i : m_users) {
        ss << "    user(" << i.first << ") size=" << i.second.size() << std::endl;
    }
    lock.unlock();
    return ss.str();
}

void ArticleManager::start() {
    sylar::RWMutex::WriteLock lock(m_mutex);
    if(m_timer) {
        return;
    }
    m_timer = sylar::IOManager::GetThis()->addTimer(60 * 1000,
                std::bind(&ArticleManager::onTimer, this), true);
    m_updateTimer = sylar::IOManager::GetThis()->addTimer(2 * 1000,
                std::bind(&ArticleManager::onUpdateTimer, this), true);
}

void ArticleManager::stop() {
    sylar::RWMutex::WriteLock lock(m_mutex);
    if(!m_timer) {
        return;
    }
    m_timer->cancel();
    m_timer = nullptr;

    m_updateTimer->cancel();
    m_updateTimer = nullptr;
}

void ArticleManager::onUpdateTimer() {
    std::set<int64_t> updates;
    {
        sylar::RWMutex::WriteLock lock(m_viewsMutex);
        updates.swap(m_updates);
    }

    if(updates.empty()) {
        return;
    }
    auto conn = blog::GetDB();
    if(!conn) {
        SYLAR_LOG_ERROR(g_logger) << "get db connect fail";

        sylar::RWMutex::WriteLock lock(m_viewsMutex);
        for(auto& i : updates) {
            m_updates.insert(i);
        }
        return;
    }
    for(auto& i : updates) {
        auto info = get(i);
        if(info) {
            if(data::ArticleInfoDao::Update(info, conn)) {
                addUpdate(i);
            }
        }
    }
}

void ArticleManager::onTimer() {
    time_t now = time(0);
    std::vector<data::ArticleInfo::ptr> infos;
    sylar::RWMutex::ReadLock lock(m_mutex);
    for(auto& i : m_datas) {
        if(i.second->getState() != (int)State::UNPUBLISH) {
            continue;
        }

        if(i.second->getPublishTime() < now) {
            i.second->setState((int)State::PUBLISH);
            i.second->setUpdateTime(now);
            infos.push_back(i.second);
        }
    }
    lock.unlock();

    if(infos.empty()) {
        return;
    }
    auto db = GetDB();
    if(!db) {
        SYLAR_LOG_ERROR(g_logger) << "getDB error";
        return;
    }
    for(auto& i : infos) {
        if(data::ArticleInfoDao::Update(i, db)) {
            SYLAR_LOG_ERROR(g_logger) << "Update error errno="
                << db->getErrno() << " errstr=" << db->getErrStr()
                << " data=" << i->toJsonString();
        }
    }
}

std::pair<data::ArticleInfo::ptr, data::ArticleInfo::ptr> ArticleManager::nearby(int64_t id) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto it = m_datas.find(id);
    if(it == m_datas.end()) {
        return std::make_pair(nullptr, nullptr);
    }
    data::ArticleInfo::ptr next;
    auto iit = it;
    ++iit;
    for(;iit != m_datas.end(); ++iit) {
        if(iit->second->getIsDeleted()) {
            continue;
        }
        if(iit->second->getState() == (int)State::PUBLISH) {
            next = iit->second;
            break;
        }
    }

    data::ArticleInfo::ptr prev;
    SYLAR_ASSERT(id == it->first);
    while(it != m_datas.begin()) {
        --it;
        if(it->second->getIsDeleted()) {
            continue;
        }
        if(it->second->getState() == (int)State::PUBLISH) {
            prev = it->second;
            break;
        }
    }
    return std::make_pair(prev, next);
}

bool ArticleManager::addViews(uint64_t id, const std::string& cooke_id) {
    time_t now = time(0);
    sylar::RWMutex::ReadLock lock(m_viewsMutex);
    auto it = m_viewsCache.find(id);
    if(it != m_viewsCache.end()) {
        auto iit = it->second.find(cooke_id);
        if(iit != it->second.end() && (now - iit->second) < 10 * 60 ) {
            return false;
        }
    }
    lock.unlock();

    sylar::RWMutex::WriteLock lock2(m_viewsMutex);
    m_viewsCache[id][cooke_id] = now;
    return true;
}

void ArticleManager::addUpdate(int64_t id) {
    sylar::RWMutex::WriteLock lock(m_viewsMutex);
    m_updates.insert(id);
}

bool ArticleManager::incViews(uint64_t id, const std::string& cooke_id, uint64_t user_id) {
    auto info = get(id);
    if(!info) {
        return false;
    }
    bool v = addViews(id, cooke_id);
    if(v) {
        info->setViews(info->getViews() + 1);
        addUpdate(id);
    }
    return true;
}

bool ArticleManager::incPraise(uint64_t id, const std::string& cooke_id, uint64_t user_id) {
    auto info = get(id);
    if(!info) {
        return false;
    }
    auto rpy = sylar::RedisUtil::Cmd("blog", "hexists pra_a2u:%lld %lld", id, user_id);
    if(!rpy) {
        SYLAR_LOG_ERROR(g_logger) << "hexists fail";
        return false;
    }
    if(rpy->integer == 1) {
        return true;
    }
    rpy = sylar::RedisUtil::Cmd("blog", "hset pra_a2u:%lld %lld %lld", id, user_id, time(0));
    if(!rpy) {
        SYLAR_LOG_ERROR(g_logger) << "hset fail";
        return false;
    }
    rpy = sylar::RedisUtil::Cmd("blog", "hset pra_u2a:%lld %lld %lld", user_id, id, time(0));
    if(!rpy) {
        SYLAR_LOG_ERROR(g_logger) << "hset fail";
        return false;
    }
    info->setPraise(info->getPraise() + 1);
    addUpdate(id);

    return true;
}

bool ArticleManager::incFavorites(uint64_t id, const std::string& cooke_id, uint64_t user_id) {
    auto info = get(id);
    if(!info) {
        return false;
    }
    auto rpy = sylar::RedisUtil::Cmd("blog", "hexists fav_a2u:%lld %lld", id, user_id);
    if(!rpy) {
        SYLAR_LOG_ERROR(g_logger) << "hexists fail";
        return false;
    }
    if(rpy->integer == 1) {
        return true;
    }
    rpy = sylar::RedisUtil::Cmd("blog", "hset fav_a2u:%lld %lld %lld", id, user_id, time(0));
    if(!rpy) {
        SYLAR_LOG_ERROR(g_logger) << "hset fail";
        return false;
    }
    rpy = sylar::RedisUtil::Cmd("blog", "hset fav_u2a:%lld %lld %lld", user_id, id, time(0));
    if(!rpy) {
        SYLAR_LOG_ERROR(g_logger) << "hset fail";
        return false;
    }
    info->setFavorites(info->getFavorites() + 1);
    addUpdate(id);
    return true;
}

bool ArticleManager::decPraise(uint64_t id, const std::string& cooke_id, uint64_t user_id) {
    auto info = get(id);
    if(!info) {
        return false;
    }
    bool v = false;
    auto rpy = sylar::RedisUtil::Cmd("blog", "hdel pra_a2u:%lld %lld", id, user_id);
    if(!rpy) {
        SYLAR_LOG_ERROR(g_logger) << "hset fail";
        return false;
    }
    if(rpy->integer == 1) {
        v = true;
    }
    rpy = sylar::RedisUtil::Cmd("blog", "hdel pra_u2a:%lld %lld", user_id, id);
    if(!rpy) {
        SYLAR_LOG_ERROR(g_logger) << "hset fail";
        return false;
    }
    if(rpy->integer == 1) {
        v = true;
    }
    if(v) {
        info->setPraise(info->getPraise() - 1);
        addUpdate(id);
    }
    return true;
}

bool ArticleManager::decFavorites(uint64_t id, const std::string& cooke_id, uint64_t user_id) {
    auto info = get(id);
    if(!info) {
        return false;
    }
    bool v = false;
    auto rpy = sylar::RedisUtil::Cmd("blog", "hdel fav_a2u:%lld %lld", id, user_id);
    if(!rpy) {
        SYLAR_LOG_ERROR(g_logger) << "hset fail";
        return false;
    }
    if(rpy->integer == 1) {
        v = true;
    }
    rpy = sylar::RedisUtil::Cmd("blog", "hset fav_u2a:%lld %lld", user_id, id);
    if(!rpy) {
        SYLAR_LOG_ERROR(g_logger) << "hset fail";
        return false;
    }
    if(rpy->integer == 1) {
        v = true;
    }
    if(v) {
        info->setFavorites(info->getFavorites() - 1);
        addUpdate(id);
    }
    return true;
}

bool ArticleManager::listUserFav(int64_t id, std::map<int64_t, int64_t>& articles) {
#define PROC(id, mask, articles) \
    auto rpy = sylar::RedisUtil::Cmd("blog", mask, id); \
    if(!rpy) { \
        SYLAR_LOG_ERROR(g_logger) << "hgetall fail"; \
        return false; \
    } \
    for(size_t i = 0; i < rpy->elements; i += 2) { \
        articles[sylar::TypeUtil::Atoi(rpy->element[i]->str)] \
            = sylar::TypeUtil::Atoi(rpy->element[i + 1]->str); \
    } \
    return true;
    PROC(id, "hgetall fav_u2a:%lld", articles);
}

bool ArticleManager::listUserPra(int64_t id, std::map<int64_t, int64_t>& articles) {
    PROC(id, "hgetall pra_u2a:%lld", articles);
}

bool ArticleManager::listArticleFav(int64_t id, std::map<int64_t, int64_t>& users) {
    PROC(id, "hgetall fav_a2u:%lld", users);
}

bool ArticleManager::listArticlePra(int64_t id, std::map<int64_t, int64_t>& users) {
    PROC(id, "hgetall pra_a2u:%lld", users);
}
#undef PROC

#undef XX

}
