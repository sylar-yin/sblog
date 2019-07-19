#include "comment_manager.h"
#include "sylar/log.h"
#include "sylar/util.h"
#include "blog/util.h"

namespace blog {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

bool CommentManager::loadAll() {
    auto db = GetDB();
    if(!db) {
        SYLAR_LOG_ERROR(g_logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::CommentInfo::ptr> results;
    if(blog::data::CommentInfoDao::QueryAll(results, db)) {
        SYLAR_LOG_ERROR(g_logger) << "CommentManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, blog::data::CommentInfo::ptr> datas;
    std::map<int64_t, blog::data::CommentInfo::ptr> verifys;
    std::unordered_map<int64_t, std::map<int64_t, blog::data::CommentInfo::ptr> > articles;

    for(auto& i : results) {
        datas[i->getId()] = i;
        articles[i->getUserId()][i->getId()] = i;
        if(i->getState() == 1
                && i->getIsDeleted() == 0) {
            verifys[i->getId()] = i;
        }
    }

    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    m_articles.swap(articles);
    m_verifys.swap(verifys);
    return true;
}

void CommentManager::add(blog::data::CommentInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
    m_articles[info->getUserId()][info->getId()] = info;
    if(info->getState() == 1
            && info->getIsDeleted() == 0) {
        m_verifys[info->getId()] = info;
    }
}

#define XX(map, key) \
    sylar::RWMutex::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

blog::data::CommentInfo::ptr CommentManager::get(int64_t id) {
    XX(m_datas, id);
}

int64_t CommentManager::listByArticleId(std::vector<blog::data::CommentInfo::ptr>& infos,
                        int64_t article_id, int64_t offset, int64_t size, bool valid) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto uit = m_articles.find(article_id);
    if(uit == m_articles.end()) {
        return 0;
    }
    if(offset >= (int32_t)uit->second.size()) {
        return uit->second.size();
    }
    auto it = uit->second.rbegin();
    std::advance(it, offset);
    for(; (int32_t)infos.size() < size && it != uit->second.rend(); ++it) {
        if(!valid || (!it->second->getIsDeleted() && it->second->getState() == 2)) {
            infos.push_back(it->second);
        }
    }
    return uit->second.size();
}

void CommentManager::delVerify(int64_t id) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_verifys.erase(id);
}

void CommentManager::addVerify(data::CommentInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_verifys[info->getId()] = info;
}

int64_t CommentManager::listVerifyPages(std::vector<data::CommentInfo::ptr>& infos
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

#undef XX

}
