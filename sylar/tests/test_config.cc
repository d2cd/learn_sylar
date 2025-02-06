#include "log.h"
#include "config.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

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
    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << root.Scalar();
    print_yaml(root, 0);
}

// //自定义类型
// class Person{
// public:
//     Person() : m_name("ffy"), m_age(20), m_sex(false){};
//     std::string to_string() const{
//         std::stringstream ss;
//         ss << "Person name=" << m_name
//            <<" age=" << m_age
//            << " sex=" << m_sex;
//         return ss.str();
//     }
//     bool operator==(const Person& e) const{
//         return e.m_name == m_name && 
//                 e.m_age == m_age  &&
//                 e.m_sex == m_sex;
//     }
//     void set_name(const std::string& name){
//         m_name = name;
//     }
//     void set_age(const int& age){
//         m_age = age;
//     }
//     void set_sex(const bool& sex){
//         m_sex = sex;
//     }
//     const std::string& getName() const {return m_name;}
//     const int getAge() const {return m_age;}
//     const bool getSex() const {return m_sex;}
    
// private:
//     std::string m_name;
//     int m_age;
//     bool m_sex;
// };

// namespace sylar{
// template<>
// class LexicalCast<Person, std::string> {
// public:
//     std::string operator()(Person vec){
//         YAML::Node node;
//         node["m_name"] = vec.getName();
//         node["m_age"] = vec.getAge();
//         node["m_sex"] = vec.getSex();
//         std::stringstream ss;
//         ss << node << std::endl;
//         return ss.str();
//     }  
// };

// template<>
// class LexicalCast<std::string, Person>{
// public:
//     Person operator()(std::string v){
//         Person ret; 
//         YAML::Node node = YAML::Load(v);

//         std::stringstream ss;
//         ss << node;
//         ret.set_name(node["m_name"].as<std::string>());
//         ret.set_age(node["m_age"].as<int>());
//         ret.set_sex(node["m_sex"].as<bool>());
//         return ret;
//     }
// };

// }



// void test_class(){
// sylar::ConfigVar<Person>::ptr g_person_value_config = sylar::Config::Lookup<Person>("system.person", Person(), "Class_Person");
// sylar::ConfigVar<std::vector<Person>>::ptr g_person_vec_value_config = sylar::Config::Lookup<std::vector<Person>>("system.vec_person", std::vector<Person>{Person(),Person()}, "Class_Person");

// g_person_value_config->addListener(10, [](const Person& old_val, const Person& new_val){
//     SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "old_val:" << old_val.to_string() << "new_val:" << new_val.to_string();
// });
// SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"before:  person " <<g_person_value_config->getValue().to_string();
// SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"before yaml:  person " << g_person_value_config->to_string();
// {
//     auto& m = g_person_vec_value_config->getValue();
//     for(auto& i : m){
//         SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "before: vecperson "<< "" << i.to_string(); 
//     }
// }
// SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) <<"before yaml:  vecperson " << g_person_vec_value_config->to_string();


// YAML::Node root=YAML::LoadFile("/home/ffy/sylar/bin/conf/log.yml");
// sylar::Config::LoadFromYaml(root);
// SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "after:  person "<<g_person_value_config->getValue().to_string();  
// {
//     auto& m = g_person_vec_value_config->getValue();
//     for(auto& i : m){
//         SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "after: vecperson " << i.to_string(); 
//     }
// } 
// }

// sylar::ConfigVar<int>::ptr g_int_value_config = sylar::Config::Lookup<int>("system.port", (int)8080, "system port");
// sylar::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config = sylar::Config::Lookup<std::vector<int>>("system.int_vec", std::vector<int>{1,2,4}, "int_vec");
// sylar::ConfigVar<std::list<int>>::ptr g_int_list_value_config = sylar::Config::Lookup<std::list<int>>("system.int_list", std::list<int>{102,110,123}, "int_list");
// sylar::ConfigVar<std::set<int>>::ptr g_int_set_value_config = sylar::Config::Lookup<std::set<int>>("system.int_set", std::set<int>{-1,5,-1,-32}, "int_set");
// sylar::ConfigVar<std::unordered_set<int>>::ptr g_int_uset_value_config = sylar::Config::Lookup<std::unordered_set<int>>("system.int_uset", std::unordered_set<int>{-1,5,-1,-32}, "int_uset");
// sylar::ConfigVar<std::map<std::string, int>>::ptr g_str_int_map_value_config = sylar::Config::Lookup<std::map<std::string, int>>("system.str_int_map", std::map<std::string, int>{{"m",5}}, "str_int_map");
// sylar::ConfigVar<std::unordered_map<std::string, int>>::ptr g_str_int_umap_value_config = sylar::Config::Lookup<std::unordered_map<std::string, int>>("system.str_int_umap", std::unordered_map<std::string, int>{{"m",34}}, "str_int_umap");


// void test_config(){
//     SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "before " << g_int_value_config->getValue();
//     XX(g_int_vec_value_config, "int_vec", "before");
//     XX(g_int_list_value_config, "int_list", "before");
//     XX(g_int_set_value_config, "int_set", "before");
//     XX(g_int_uset_value_config, "int_uset", "before");

//     XX_M(g_str_int_map_value_config, "str_int_map", "before");
//     XX_M(g_str_int_umap_value_config, "str_int_umap", "before");

//     std::cout << "LoadFromYaml success" <<std::endl;

//     YAML::Node root=YAML::LoadFile("/home/ffy/sylar/bin/conf/log.yml");
//     sylar::Config::LoadFromYaml(root);
//     std::cout << "LoadFromYaml success" <<std::endl;
//     // print_yaml(root, 0);

//     SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "after" << g_int_value_config->getValue();
//     XX(g_int_vec_value_config, "int_vec", "after");
//     XX(g_int_list_value_config, "int_list", "after");
//     XX(g_int_set_value_config, "int_set", "after");
//     XX(g_int_uset_value_config, "int_uset", "after");
//     XX_M(g_str_int_map_value_config, "str_int_map", "after");
//     XX_M(g_str_int_umap_value_config, "str_int_umap", "after"); 
    
// }    
void test_config_log(){
    // sylar::ConfigVar<std::set<sylar::LogDefine>>::ptr g_logdefine_config = sylar::Config::Lookup<std::set<sylar::LogDefine>>("logs", std::set<sylar::LogDefine>(), "set_loggers");

    //检查日志系统和配置的整合.
    // SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << sylar::Config::Lookup<std::set<sylar::LogDefine>>()
    // std::cout << "niubi" << std::endl;
    std::cout << sylar::LoggerMgr::getInstance()->toYamlString() << std::endl;

    YAML::Node root = YAML::LoadFile("/home/ffy/sylar/bin/conf/log.yml");
    
    sylar::Config::LoadFromYaml(root);

    std::cout << "======================" << std::endl;

    std::cout << sylar::LoggerMgr::getInstance()->toYamlString() << std::endl;

    SYLAR_LOG_WARN(SYLAR_LOG_NAME("system")) << "hello";  
    auto root_logger = SYLAR_LOG_NAME("root");
    SYLAR_LOG_WARN(root_logger) << " hello";
    //但是这样的formater更改并没没有写入配置中。而且会改变所有appender的formatter
    root_logger->setFormatter("%d - %p");
    SYLAR_LOG_WARN(root_logger) << " hello";




}
int main(){

    // test_yaml();
    // test_config();
    // test_class();
    test_config_log();
    return 0;
}