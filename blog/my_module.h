#include "sylar/module.h"
#include "sylar/db/sqlite3.h"

namespace blog {

class MyModule : public sylar::Module {
public:
    typedef std::shared_ptr<MyModule> ptr;
    MyModule();
    bool onLoad() override;
    bool onUnload() override;
    bool onServerReady() override;
    bool onServerUp() override;
    std::string statusString() override;
};

}
