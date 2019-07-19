#include "article_label_rel_manager.h"
#include "sylar/log.h"
#include "sylar/util.h"
#include "blog/util.h"

namespace blog {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

bool ArticleLabelRelManager::loadAll() {
    auto db = GetDB();
    if(!db) {
        SYLAR_LOG_ERROR(g_logger) << "Get SQLite3 connection fail";
        return false;
    }
    std::vector<data::ArticleLabelRelInfo::ptr> results;
    if(blog::data::ArticleLabelRelInfoDao::QueryAll(results, db)) {
        SYLAR_LOG_ERROR(g_logger) << "ArticleLabelRelManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, blog::data::ArticleLabelRelInfo::ptr> datas;
    std::unordered_map<int64_t, std::map<int64_t, blog::data::ArticleLabelRelInfo::ptr> > articles;

    for(auto& i : results) {
        datas[i->getId()] = i;
        articles[i->getArticleId()][i->getLabelId()] = i;
    }

    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    m_articles.swap(articles);
    return true;
}

void ArticleLabelRelManager::add(blog::data::ArticleLabelRelInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
    m_articles[info->getArticleId()][info->getLabelId()] = info;
}

#define XX(map, key) \
    sylar::RWMutex::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

blog::data::ArticleLabelRelInfo::ptr ArticleLabelRelManager::get(int64_t id) {
    XX(m_datas, id);
}

blog::data::ArticleLabelRelInfo::ptr ArticleLabelRelManager::getByArticleIdLabelId(int64_t article_id, int64_t label_id) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto it = m_articles.find(article_id);
    if(it == m_articles.end()) {
        return nullptr;
    }
    auto iit = it->second.find(label_id);
    return iit == it->second.end() ? nullptr : iit->second;
}

bool ArticleLabelRelManager::listByArticleId(std::vector<data::ArticleLabelRelInfo::ptr>& infos
                ,int64_t article_id, bool valid) {
    sylar::RWMutex::ReadLock lock(m_mutex);
    auto it = m_articles.find(article_id);
    if(it == m_articles.end()) {
        return nullptr;
    }
    for(auto& i : it->second) {
        if(!valid || !i.second->getIsDeleted()) {
            infos.push_back(i.second);
        }
    }
    return true;
}

#undef XX

}
