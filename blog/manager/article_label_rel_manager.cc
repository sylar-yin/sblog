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
    std::unordered_map<int64_t, blog::data::ArticleLabelRelInfo::ptr> articles;
    std::unordered_map<int64_t, blog::data::ArticleLabelRelInfo::ptr> labels;

    for(auto& i : results) {
        datas[i->getId()] = i;
        articles[i->getArticleId()] = i;
        labels[i->getLabelId()] = i;
    }

    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    m_articles.swap(articles);
    m_labels.swap(labels);
    return true;
}

void ArticleLabelRelManager::add(blog::data::ArticleLabelRelInfo::ptr info) {
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
    m_articles[info->getArticleId()] = info;
    m_labels[info->getLabelId()] = info;
}

#define XX(map, key) \
    sylar::RWMutex::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

blog::data::ArticleLabelRelInfo::ptr ArticleLabelRelManager::get(int64_t id) {
    XX(m_datas, id);
}

blog::data::ArticleLabelRelInfo::ptr ArticleLabelRelManager::getByArticleId(int64_t id) {
    XX(m_articles, id);
}

blog::data::ArticleLabelRelInfo::ptr ArticleLabelRelManager::getByLabelId(int64_t id) {
    XX(m_labels, id);
}

#undef XX

}
