#include "log.h"
#include "config.h"
#include <yaml-cpp/yaml.h>

sylar::ConfigVar<int>::ptr g_int_value_config = sylar::Config::Lookup<int>("system.port", (int)8080, "system port");


void print_yaml(YAML::Node node, int level){
    if(node.IsScalar()){
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Sca:\t" <<std::string(level*4, ' ') 
            << node.Scalar() << " - " << node.Type() << " - " << level;
    }else if(node.IsNull()){
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << std::string(level*4, ' ')
            << "NULL - " << " - " << node.Type() << " - " << level;
    }else if(node.IsMap()){

        for(auto it = node.begin(); it != node.end(); ++it)
        {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Map:\t"<< std::string(level*4, ' ')
            << it->first << " - " << it->second.Type() << " - " << level ;
            print_yaml(it->second, level+1);
        }
    }else if(node.IsSequence()){
        for(size_t i=0; i<node.size(); i++)
        {
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"Seq:\t" <<std::string(level*4, ' ')
            << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level+1);
        }
    }
    
}

void test_yaml(){

    YAML::Node root=YAML::LoadFile("/home/ffy/sylar/bin/conf/log.yml");
    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << root; //这样好像也行
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << root.Scalar();
    print_yaml(root, 0);
}

void test_config(){
    YAML::Node root=YAML::LoadFile("/home/ffy/sylar/bin/conf/log.yml");
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_int_value_config->getValue();
    sylar::Config::LoadFromYaml(root);
    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT())<<"---------";
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << g_int_value_config->getValue();
    
    
}
int main(){

    // test_yaml();
    test_config();
    return 0;
}