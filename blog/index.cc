#include "index.h"
#include "blog/manager/article_category_rel_manager.h"
#include "blog/manager/article_label_rel_manager.h"
#include "blog/manager/category_manager.h"
#include "blog/manager/label_manager.h"
#include "sylar/log.h"

namespace blog {

static sylar::Logger::ptr g_logger = SYLAR_LOG_ROOT();

struct ParamArgsInfo {
    std::string name;
    uint64_t key;
    uint32_t type;
};

static std::map<std::string, ParamArgsInfo> s_param_names = {
#define XX(name, key, type) \
    {name, {name, (uint64_t)key, type}},
    INDEX_TYPE_MACRO(XX)
#undef XX
};

static std::map<uint64_t, ParamArgsInfo> s_param_names2 = {
#define XX(name, key, type) \
    {(uint64_t)key, {name, (uint64_t)key, type}},
    INDEX_TYPE_MACRO(XX)
#undef XX
};


std::string GetIndexTypeName(uint64_t id) {
    auto it = s_param_names2.find(id);
    return it == s_param_names2.end() ? std::to_string(id) : it->second.name;
}

int GetIndexTypeType(uint64_t id) {
    auto it = s_param_names2.find(id);
    return it == s_param_names2.end() ? 0 : it->second.type;
}

void ParseParams(std::map<uint64_t, std::set<uint64_t> >& params,
                 const std::map<std::string, std::string>& input_params) {
    for(auto& i : input_params) {
        auto it = s_param_names.find(i.first);
        if(it == s_param_names.end()) {
            continue;
        }
        auto parts = sylar::split(i.second, ',');
        for(auto& n : parts) {
            if(n.empty()) {
                continue;
            }
            if(it->second.type == 1) {
                params[it->second.key].insert(sylar::TypeUtil::Atoi(n));
            } else {
                params[it->second.key].insert(Index::StrHash(n));
            }
        }
    }
}

void ParseFields(std::map<uint64_t, std::set<uint64_t> >& params,
                 const std::string& str) {
    auto tmp = sylar::split(str, ';');
     std::map<std::string, std::string> input_params;
    for(auto& i : tmp) {
        auto tt = sylar::split(i, ':');
        if(tt.size() == 1) {
            input_params[i];
        } else {
            input_params[tt[0]] = tt[1];
        }
    }
    for(auto& i : input_params) {
        auto it = s_param_names.find(i.first);
        if(it == s_param_names.end()) {
            continue;
        }
        params[it->second.key];
        auto parts = sylar::split(i.second, ',');
        for(auto& n : parts) {
            if(n.empty()) {
                continue;
            }
            if(it->second.type == 1) {
                params[it->second.key].insert(sylar::TypeUtil::Atoi(n));
            } else {
                params[it->second.key].insert(Index::StrHash(n));
            }
        }
    }
}

Index::Index()
    :m_createTime(0)
    ,m_endTime(0) {
}

uint64_t Index::StrHash(const std::string& str) {
    return sylar::murmur3_hash64(str.c_str());
}

uint64_t Index::hash(const std::string& str, bool save) {
    auto v = StrHash(str);
    if(save) {
        m_strings.insert(std::make_pair(v, str));
    }
    return v;
}

std::string Index::getStr(uint64_t id) {
    auto it = m_strings.find(id);
    return it == m_strings.end() ? "" : it->second;
}

bool Index::set(uint64_t type, uint64_t key, uint32_t idx, bool v) {
    auto b = m_indexs[type][key];
    if(!b) {
        b.reset(new sylar::ds::Bitmap(m_docs.size()));
        m_indexs[type][key] = b;
    }
    b->set(idx, v);
    return true;
}

sylar::ds::Bitmap::ptr Index::get(uint64_t type, uint64_t key) {
    auto it = m_indexs.find(type);
    if(it == m_indexs.end()) {
        return nullptr;
    }
    auto iit = it->second.find(key);
    return iit == it->second.end() ? nullptr : iit->second;
}

void Index::build() {
    SYLAR_LOG_INFO(g_logger) << "Index build begin...";
    m_createTime = time(0);
    std::vector<data::ArticleInfo::ptr> infos;
    ArticleMgr::GetInstance()->listByUserIdPages(infos, 0, 0, 0x7FFFFFFF, true, 0);
    std::sort(infos.begin(), infos.end(), [](const data::ArticleInfo::ptr a
                ,const data::ArticleInfo::ptr b){
        if(a->getWeight() != b->getWeight()) {
            return a->getWeight() > b->getWeight();
        }
        return a->getId() > b->getId();
    });
    m_docs.reserve(infos.size());
    for(auto& info : infos) {
        m_docs.push_back(info->getId());
    }
    for(size_t i = 0; i < infos.size(); ++i) {
        buildIdx(infos[i], i);
    }
    m_endTime = time(0);
    SYLAR_LOG_INFO(g_logger) << "Index build over... used="
        << (m_endTime - m_createTime) << " doc.size=" << m_docs.size();
}

void Index::buildIdx(data::ArticleInfo::ptr info, uint32_t idx) {
    set((uint64_t)IndexType::USER_ID, info->getUserId(), idx, true);
    set((uint64_t)IndexType::STATE, info->getState(), idx, true);
    set((uint64_t)IndexType::YEAR_MON, hash(sylar::Time2Str(info->getCreateTime(), "%Y年%m月"), true), idx, true);

    std::vector<data::ArticleCategoryRelInfo::ptr> cats;
    ArticleCategoryRelMgr::GetInstance()->listByArticleId(cats, info->getId(), true);
    for(auto& i : cats) {
        auto cat = CategoryMgr::GetInstance()->get(i->getCategoryId());
        if(!cat) {
            SYLAR_LOG_ERROR(g_logger) << "invalid cat_id=" << i->getCategoryId();
            continue;
        }
        set((uint64_t)IndexType::CAT_ID, cat->getId(), idx, true);
        set((uint64_t)IndexType::CAT_NAME, hash(cat->getName().c_str(), true), idx, true);
    }

    std::vector<data::ArticleLabelRelInfo::ptr> labels;
    ArticleLabelRelMgr::GetInstance()->listByArticleId(labels, info->getId(), true);
    for(auto& i : labels) {
        auto label = LabelMgr::GetInstance()->get(i->getLabelId());
        if(!label) {
            SYLAR_LOG_ERROR(g_logger) << "invalid label_id=" << i->getLabelId();
            continue;
        }
        set((uint64_t)IndexType::LABEL_ID, label->getId(), idx, true);
        set((uint64_t)IndexType::LABEL_NAME, hash(label->getName().c_str(), true), idx, true);
    }

}

sylar::ds::Bitmap::ptr Index::query(const std::map<uint64_t, std::set<uint64_t> >& params) {
    sylar::ds::Bitmap::ptr b(new sylar::ds::Bitmap(m_docs.size(), 0xFF));
    for(auto& i : params) {
        sylar::ds::Bitmap::ptr t;
        for(auto& v : i.second) {
            auto tmp = get(i.first, v);
            if(!tmp) {
                SYLAR_LOG_ERROR(g_logger) << "invalid type=" << i.first
                    << " key=" << v;
                continue;
            }
            if(!t) {
                t = tmp->uncompress();
            } else {
                *t &= *tmp;
            }
        }
        if(!t) {
            return nullptr;
        }
        *b &= *t;
        if(!b->any()) {
            break;
        }
    }
    return b;
}

int32_t Index::search(std::vector<uint64_t>& ids, const std::map<uint64_t, std::set<uint64_t> >& params
                       ,uint32_t max_size) {
    auto b = query(params);
    if(!b) {
        return -1;
    }
    uint32_t i = 0;
    for(auto it = b->begin_new();
            !*it && i < max_size; it->next(), ++i) {
        ids.push_back(m_docs[**it]);
    }
    return b->getCount();
}

int32_t Index::property(std::map<uint64_t, std::map<uint64_t, uint64_t> >& props,
                        const std::map<uint64_t, std::set<uint64_t> >& params,
                        std::map<uint64_t, std::set<uint64_t> >& querys) {
    auto b = query(params);
    if(!b) {
        return -1;
    }
    for(auto & i : querys) {
        if(i.second.empty()) {
            auto it = m_indexs.find(i.first);
            if(it == m_indexs.end()) {
                continue;
            }
            for(auto& v : it->second) {
                auto t = get(i.first, v.first);
                if(!t) {
                    continue;
                }
                auto c = (*b & *t).getCount();
                if(c) {
                    props[i.first][v.first] = c;
                }
            }
        } else {
            for(auto& v : i.second) {
                auto t = get(i.first, v);
                if(!t) {
                    continue;
                }
                auto c = (*b & *t).getCount();
                if(c) {
                    props[i.first][v] = c;
                }
            }
        }
    }
    return 1;
}

std::string Index::toString() {
    std::stringstream ss;
    ss << "[Index create_time=" << sylar::Time2Str(m_createTime)
       << " end_time=" << sylar::Time2Str(m_endTime)
       << " used_time=" << (m_endTime - m_createTime)
       << " doc.size=" << m_docs.size()
       << "]" << std::endl;
    for(auto& i : m_indexs) {
        ss << "    " << i.first << "(" << i.second.size() << "):" << std::endl;
        for(auto& n : i.second) {
            ss << "        " << n.first << ": " << n.second->getCount() << std::endl;
        }
    }
    return ss.str();
}

Index::ptr IndexManager::get() {
    sylar::RWMutex::ReadLock lock(m_mutex);
    return m_index;
}

void IndexManager::build() {
    if(m_next) {
        return;
    }
    m_next.reset(new Index);
    m_next->build();
    SYLAR_LOG_INFO(g_logger) << m_next->toString();
    sylar::RWMutex::WriteLock lock(m_mutex);
    m_index = m_next;
    lock.unlock();
    m_next.reset();
}

}
