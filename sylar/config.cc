#include "config.h"
#include <list>
namespace sylar{
    //不能重复声明static
    ConfigVarBase::ptr Config::LookupBase(const std::string& name){
        if(s_datas.find(name) == s_datas.end()){
            return nullptr;
        }
        return s_datas[name];
    }
    Config::ConfigVarMap Config::s_datas;
    //具体来说，静态成员函数只能调用其他静态成员函数和非成员函数
    static void listAllMember(const std::string& prefix, const YAML::Node& node, std::list<std::pair<std::string, const YAML::Node> >& all_nodes){
        if(prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz_.1234567890")!=std::string::npos){
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Config invalid name: " << prefix << " : " << node;
            return; 
        }
        all_nodes.push_back(std::make_pair(prefix, node));
        if(node.IsMap()){
            for(auto it = node.begin(); it != node.end(); ++it){
                listAllMember(prefix.empty() ? it->first.Scalar() : prefix+"."+it->first.Scalar(), it->second, all_nodes);
            }
        }
    }

    void Config::LoadFromYaml(const YAML::Node& node)
    {
        std::list<std::pair<std::string, const YAML::Node> > all_nodes;
        listAllMember("", node, all_nodes);
        
        for(auto& t : all_nodes){
            std::string key = t.first;
            if(key.empty()){
                continue;
            }
            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
            //ConfigVarBase是抽象类，它定义了ptr, ConfigVar子类，但是我们不知道它是什么类型，这里看出多态的好处
            //这里最好用父类的LookupBase()
            ConfigVarBase::ptr var = Config::LookupBase(key);
            if(var){
                if(t.second.IsScalar()){
                    var->from_string(t.second.Scalar());
                }else{
                    std::stringstream ss;
                    ss << t.second; //转化为数据流，这个是YAML::Node定义过的
                    var->from_string(ss.str());
                }
            }
            
        }
        
    }
}