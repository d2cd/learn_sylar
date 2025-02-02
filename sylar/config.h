#include<sstream>
#include<memory>
#include<string>
#include<boost/lexical_cast.hpp>
#include<functional>
#include"log.h"
#include<yaml-cpp/yaml.h>


#include<list>
#include<map>
#include<set>
#include<unordered_map>
#include<unordered_set>

#include<iostream>

namespace sylar{  

  
template<class F, class T>
class LexicalCast{
public:
    T operator()(const F& v){
        return boost::lexical_cast<T> (v);
    }
};

template<class T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::stringstream ss;
    std::string operator()(std::vector<T> vec){
        for(size_t i=0; i<vec.size(); i++){
            ss << LexicalCast<T,std::string>()(vec[i]);
        }
        return ss.str();
    }  
};
template<class T>
class LexicalCast<std::string, std::vector<T>>{
public:
    std::vector<T> operator()(std::string v){
        std::vector<T> ret;
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;

        for(size_t i=0; i<node.size(); i++){
            ss.str("");
            ss << node[i];
            ret.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return ret;
    }
};


template<class T>
class LexicalCast<std::list<T>, std::string> {
public:
    std::string operator()(std::list<T> lis){
        std::stringstream ss;
        ss.str("");
        for(auto& v : lis)
        {
            ss << LexicalCast<T, std::string>()(v);
        }
        return ss.str();
    }  
};
template<class T>
class LexicalCast<std::string, std::list<T>>{
public:
    std::list<T> operator()(std::string v){
        std::list<T> ret;
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        for(size_t i=0; i<node.size(); i++){
            ss.str("");
            ss << node[i];
            ret.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return ret;
    }
};

template<class T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator()(std::set<T> st){
        std::stringstream ss;
        ss.str("");
        for(auto& v : st)
        {
            ss << LexicalCast<T, std::string>()(v);
        }
        return ss.str();
    }  
};


template<class T>
class LexicalCast<std::string, std::set<T>>{
public:
    std::set<T> operator()(std::string v){
        std::set<T> ret;
        YAML::Node node = YAML::Load(v);
        for(size_t i=0; i<node.size(); i++){
            std::stringstream ss;
            ss.str("");
            ss << node[i];
            ret.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return ret;
    }
};


template<class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator()(std::unordered_set<T> st){
        std::stringstream ss;
        ss.str("");
        for(auto& v : st)
        {
            ss << LexicalCast<T, std::string>()(v);
        }
        return ss.str();
    }  
};

template<class T>
class LexicalCast<std::string, std::unordered_set<T>>{
public:
    std::unordered_set<T> operator()(std::string v){
        std::unordered_set<T> ret;
        YAML::Node node = YAML::Load(v);
        std::stringstream ss;
        for(size_t i=0; i<node.size(); i++){
            ss.str("");
            ss << node[i];
            ret.insert(LexicalCast<std::string,T>()(ss.str()));
        }
        return ret;
    }
};

template<class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
    std::stringstream ss;
    std::string operator()(std::map<std::string, T>& st){
        YAML::Node node;
        for(auto& v : st)
        {
            node[v.first] = LexicalCast<T, std::string>()(v.second);
        }
        ss.str("");
        ss << node;
        return ss.str();
    }  
};

template<class T>
class LexicalCast<std::string, std::map<std::string, T>>{
public:
    std::map<std::string, T> operator()(std::string v){
        std::stringstream ss;
        YAML::Node node = YAML::Load(v); //是以字典的形式保存, k和v
        typedef std::map<std::string, T> vec;
        vec ret;
        for(auto it = node.begin(); it!=node.end(); it++){
            ss.str("");
            ss << it->second;
            ret.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string,T>()(ss.str())));
        }
        return ret;
    }
};


template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::stringstream ss;
    std::string operator()(std::unordered_map<std::string, T>& st){
        YAML::Node node;
        for(auto& v : st)
        {
            node[v.first] = LexicalCast<T, std::string>()(v.second);
        }
        ss.str("");
        ss << node;
        return ss.str();
    }  
};
template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T>>{
public:
    std::unordered_map<std::string, T> operator()(std::string v){
        std::stringstream ss;
        YAML::Node node = YAML::Load(v); //是以字典的形式保存, k和v
        typedef std::unordered_map<std::string, T> vec;
        vec ret;
        for(auto it = node.begin(); it!=node.end(); it++){
            ss.str("");
            ss << it->second;
            ret.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
        }
        return ret;
    }
};









class ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name, const std::string& description=""):
        m_name(name), m_description(description){
            std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower); 
            //这里需要思考下，为什么m_name重新transform一些，之前的formatter的pattern初始化也遇到这个问题
        }
    
    virtual ~ConfigVarBase(){}; 
    //这里可以是
    //virtual ~configVarBase() = default; 但一定不能是 virtual ~ConfigVarBase();
    //会导致 undefined reference to `vtable for sylar::ConfigVarBase
    //undefined reference to `typeinfo for sylar::ConfigVarBase'
    //undefined reference to `sylar::ConfigVarBase::~ConfigVarBase()'

    const std::string& getName(){return m_name;}
    const std::string& getDescription(){return m_description;}

    virtual std::string to_string() = 0;
    virtual bool from_string(const std::string& val) = 0; 
private:
    std::string m_name;
    std::string m_description;
};


template<class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string>>
class ConfigVar:public ConfigVarBase{
public:
    typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb;
    typedef std::shared_ptr<ConfigVar> ptr;
    ConfigVar(const std::string& name, const T& default_value, const std::string& description=""):
        ConfigVarBase(name, description), m_val(default_value){}
    std::string to_string() override
    {
        try{
            // return boost::lexical_cast<std::string> (m_val);
            return ToStr()(m_val);
        }catch(std::exception &e){
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar toString() exception:"
                << e.what() << " convert: string to " ;//<< typeid(m_val).name();
        }
        return "";
    }
    bool from_string(const std::string& val) override
    {
        try{
            // m_val = boost::lexical_cast<T> (val);
            setValue(FromStr()(val));
            return true;
        }catch(std::exception &e){
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar from_string() exception:"
                << e.what() << " convert: " << typeid(val).name() << " --->  to string: " << val;
        }
        return false;
    }
    const T getValue()const {return m_val;}
    void setValue(const T& val){
        if(m_val == val){
            return;
        }
        for(auto& i : m_map){
            i.second(m_val, val);
        }
        m_val = val;
    }
    //Listener负责监听外部指令
    void addListener(uint64_t key, const on_change_cb& cb){
        std::cout << "------------------in addlistener!!--------------------" << std::endl;
        m_map[key] = cb;
    }
    void delListener(uint64_t key){
        m_map.erase(key);
    }
    on_change_cb getListener(uint64_t key){
        auto it = m_map.find(key);
        return it == m_map.end() ? nullptr : it->second;
    }
    void clearListeners(){
        m_map.clear();
    }

private:
    T m_val;
    //因为functional没有比较函数，我们没法比较两个functional是否一样
    std::map<uint64_t, on_change_cb> m_map;
    
};

class Config{
//这个config只包含一个静态成员和一个静态方法。因此只需要在config.cc里面调用以下就能够初始化
public:
    // typedef std::shared_ptr<Config> ptr;
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name, const T& default_value, const std::string& description=""){
        // auto tmp = Lookup<T>(name);
        // if(tmp){
        //     SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "lookup name=" << name << " exists";
        //     return tmp;
        // }
        auto t = GetDatas().find(name);
        if(t != GetDatas().end()){
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T>> ((*t).second); //或者t->second
            if(tmp){
                SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Lookup name:" << name << " exist";
                return tmp;
            }else{
                SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "Lookup name:" << name << " exist but not type " << typeid(T).name();
                return nullptr;
            }
        }

        if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz_.1234567890")
            != std::string::npos){
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "lookup name invalid: " << name;
            throw std::invalid_argument(name);
        }
        auto m = typename ConfigVar<T>::ptr(new ConfigVar<T>(name, default_value, description));
        // typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        GetDatas()[name] = m;
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Add new ConfigVar::ptr for " << name;
        return m;
    } 

    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name){
        auto it = GetDatas().find(name);
        if(it == GetDatas().end()){
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T>> (it->second);
    } 
    static ConfigVarBase::ptr LookupBase(const std::string& name);


    static void LoadFromYaml(const YAML::Node& node);
private:
//使用静态函数包含静态类原因：静态成员初始化的顺序难以确定，这样可以保证调用lookup, s_datas已经初始化
static ConfigVarMap& GetDatas(){
    static ConfigVarMap s_datas;
    return s_datas;
}
};


}