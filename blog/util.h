#ifndef __BLOG_UTIL_H__
#define __BLOG_UTIL_H__

#include "sylar/db/sqlite3.h"
#include <string>

namespace blog {

bool is_email(const std::string& str);
bool is_valid_account(const std::string& str);
sylar::IDB::ptr GetDB();

#define DEFINE_AND_CHECK_STRING(result, var, param) \
    std::string var = request->getParam(param); \
    if(var.empty()) { \
        result->setResult(400, "param " param " is null");\
        break; \
    }

#define DEFINE_AND_CHECK_TYPE(result, type, var, param) \
    type var; \
    if(!request->checkGetParamAs(param, var)) { \
        result->setResult(400, "param " param " is null");\
        break; \
    }

std::string get_max_length_string(const std::string& str, size_t len);

}


#endif
