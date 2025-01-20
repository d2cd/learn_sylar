#include "log.h"
#include "config.h"

sylar::ConfigVar<int>::ptr g_int_value_config = sylar::Config::lookup<int>("system.port", (int)8080, "system port");
int main(){
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_int_value_config->getValue();
    return 0;
}