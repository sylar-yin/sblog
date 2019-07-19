#ifndef __BLOG_SERVLETS_USER_QUERY_SERVLET_H__
#define __BLOG_SERVLETS_USER_QUERY_SERVLET_H__

#include "sylar/sylar.h"
#include "blog/struct.h"

namespace blog {
namespace servlet {

class UserQueryServlet : public BlogServlet {
public:
    typedef std::shared_ptr<UserQueryServlet> ptr;
    UserQueryServlet();
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                   ,sylar::http::HttpResponse::ptr response
                   ,sylar::http::HttpSession::ptr session
                   ,Result::ptr result) override;
};


}
}

#endif
