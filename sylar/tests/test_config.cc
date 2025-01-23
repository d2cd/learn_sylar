#include "log.h"
#include "config.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

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

sylar::ConfigVar<int>::ptr g_int_value_config = sylar::Config::Lookup<int>("system.port", (int)8080, "system port");
sylar::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config = sylar::Config::Lookup<std::vector<int>>("system.int_vec", std::vector<int>{1,2,4}, "int_vec");
sylar::ConfigVar<std::list<int>>::ptr g_int_list_value_config = sylar::Config::Lookup<std::list<int>>("system.int_list", std::list<int>{102,110,123}, "int_list");
sylar::ConfigVar<std::set<int>>::ptr g_int_set_value_config = sylar::Config::Lookup<std::set<int>>("system.int_set", std::set<int>{-1,5,-1,-32}, "int_set");
sylar::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_value_config = sylar::Config::Lookup<std::unordered_set<int>>("system.int_uset", std::unordered_set<int>{-1,5,-1,-32}, "int_uset");
sylar::ConfigVar<std::map<std::string, int>>::ptr g_str_int_map_value_config = sylar::Config::Lookup<std::map<std::string, int>>("system.str_int_map", std::map<std::string, int>{{"m",5}}, "str_int_map");
sylar::ConfigVar<std::unordered_map<std::string, int>>::ptr g_str_int_umap_value_config = sylar::Config::Lookup<std::unordered_map<std::string, int>>("system.str_int_umap", std::unordered_map<std::string, int>{{"m",34}}, "str_int_umap");


void test_config(){
    #define XX(configvar, name, prefix)\
    {\
        auto& m = configvar->getValue();\
            for(auto& i : m){\
                SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name << i; \
            }\
    }\
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name <<" yaml: " <<configvar->to_string();

    #define XX_M(configvar, name, prefix)\
    {\
        auto& m = configvar->getValue();\
            for(auto& t : m){\
                SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name ": {" << t.first << ":" << t.second << "} "; \
            }\
    }\
    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << #prefix " " #name <<" yaml: " <<configvar->to_string();


    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "before " << g_int_value_config->getValue();
    XX(g_int_vec_value_config, "int_vec", "before");
    XX(g_int_list_value_config, "int_list", "before");
    XX(g_int_set_value_config, "int_set", "before");
    XX(g_int_uset_value_config, "int_uset", "before");

    XX_M(g_str_int_map_value_config, "str_int_map", "before");
    XX_M(g_str_int_umap_value_config, "str_int_umap", "before");

    std::cout << "LoadFromYaml success" <<std::endl;

    YAML::Node root=YAML::LoadFile("/home/ffy/sylar/bin/conf/log.yml");
    sylar::Config::LoadFromYaml(root);
    std::cout << "LoadFromYaml success" <<std::endl;
    // print_yaml(root, 0);

    SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "after" << g_int_value_config->getValue();
    XX(g_int_vec_value_config, "int_vec", "after");
    XX(g_int_list_value_config, "int_list", "after");
    XX(g_int_set_value_config, "int_set", "after");
    XX(g_int_uset_value_config, "int_uset", "after");
    XX_M(g_str_int_map_value_config, "str_int_map", "after");
    XX_M(g_str_int_umap_value_config, "str_int_umap", "after");






    
    
    
}
int main(){

    // test_yaml();
    test_config();
    return 0;
}