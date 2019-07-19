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
    std::unordered_map<int64_t, blog::data::ArticleCategoryRelInfo::ptr> articles;
    std::unordered_map<int64_t, blog::data::ArticleCategoryRelInfo::ptr> categorys;

    for(auto& i : results) {
        datas[i->getId()] = i;
        articles[i->getArticleId()] = i;
        categorys[i->getCategoryId()] = i;
    }

    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    m_articles.swap(articles);
    m_categorys.swap(categorys);
    return true;
}

void ArticleCategoryRelManager::add(blog::data::ArticleCategoryRelInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
    m_articles[info->getArticleId()] = info;
    m_categorys[info->getCategoryId()] = info;
}

#define XX(map, key) \
    sylar::RWMutex::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

blog::data::ArticleCategoryRelInfo::ptr ArticleCategoryRelManager::get(int64_t id) {
    XX(m_datas, id);
}

blog::data::ArticleCategoryRelInfo::ptr ArticleCategoryRelManager::getByArticleId(int64_t id) {
    XX(m_articles, id);
}

blog::data::ArticleCategoryRelInfo::ptr ArticleCategoryRelManager::getByCategoryId(int64_t id) {
    XX(m_categorys, id);
}

#undef XX

}
