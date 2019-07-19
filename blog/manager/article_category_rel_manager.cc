#include "article_category_rel_manager.h"
#include "sylar/log.h"
#include "sylar/util.h"
#include "blog/util.h"

namespace blog {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

bool ArticleCategoryRelManager::loadAll() {
    auto db = GetDB();
    if(!db) {
        SYLAR_LOG_ERROR(g_logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::ArticleCategoryRelInfo::ptr> results;
    if(blog::data::ArticleCategoryRelInfoDao::QueryAll(results, db)) {
        SYLAR_LOG_ERROR(g_logger) << "ArticleCategoryRelManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, blog::data::ArticleCategoryRelInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<int64_t, blog::data::ArticleCategoryRelInfo::ptr> > articles;

    for(auto& i : results) {
        datas[i->getId()] = i;
        articles[i->getArticleId()][i->getCategoryId()] = i;
    }

    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    m_articles.swap(articles);
    return true;
}

void ArticleCategoryRelManager::add(blog::data::ArticleCategoryRelInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
    m_articles[info->getArticleId()][info->getCategoryId()] = info;
}

#define XX(map, key) \
    sylar::RWMutex::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

blog::data::ArticleCategoryRelInfo::ptr ArticleCategoryRelManager::get(int64_t id) {
    XX(m_datas, id);
}

bool ArticleCategoryRelManager::listByArticleId(std::vector<data::ArticleCategoryRelInfo::ptr>& infos
                                                ,int64_t id, bool valid) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto it = m_articles.find(id);
    if(it == m_articles.end()) {
        return false;
    }
    for(auto& i : it->second) {
        if(!valid || !i.second->getIsDeleted()) {
            infos.push_back(i.second);
        }
    }
    return true;
}

blog::data::ArticleCategoryRelInfo::ptr ArticleCategoryRelManager::getByArticleIdCategoryId(int64_t article_id
                    ,int64_t category_id) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto it = m_articles.find(article_id);
    if(it == m_articles.end()) {
        return nullptr;
    }
    auto iit = it->second.find(category_id);
    if(iit == it->second.end()) {
        return nullptr;
    }
    return iit->second;
}

#undef XX

}
