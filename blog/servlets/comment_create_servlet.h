#ifndef __BLOG_SERVLETS_COMMENT_CREATE_SERVLET_H__
#define __BLOG_SERVLETS_COMMENT_CREATE_SERVLET_H__

#include "sylar/http/servlet.h"
#include "blog/struct.h"

namespace blog {
namespace servlet {

class CommentCreateServlet : public BlogLoginedServlet {
public:
    typedef std::shared_ptr<CommentCreateServlet> ptr;
    CommentCreateServlet();
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                   ,sylar::http::HttpResponse::ptr response
                   ,sylar::http::HttpSession::ptr session
                   ,Result::ptr result) override;
};

}
}

#endif
