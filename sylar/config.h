#include<sstream>
#include<memory>
#include<string>
#include<boost/lexical_cast.hpp>
#include"log.h"


namespace sylar{
class ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;
    ConfigVarBase(const std::string& name, const std::string& description=""):
        m_name(name), m_description(description){}
    
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

template<class T>
class ConfigVar:public ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    ConfigVar(const std::string& name, const T& default_value, const std::string& description=""):
        ConfigVarBase(name, description), m_val(default_value){}
    std::string to_string() override
    {
        try{
            return boost::lexical_cast<std::string> (m_val);
        }catch(std::exception &e){
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar toString() exception:"
                << e.what() << " convert: string to " ;//<< typeid(m_val).name();
        }
        return "";
    }
    bool from_string(const std::string& val) override
    {
        try{
            m_val = boost::lexical_cast<T> (val);
            return true;
        }catch(std::exception &e){
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "ConfigVar fromString() exception:"
                << e.what() << " convert: " << typeid(val).name() << " to string";
        }
        return false;
    }
    const T getValue()const {return m_val;}
    void setValue(const T& val){m_val = val;}
private:
    T m_val;
};

class Config{
//这个config只包含一个静态成员和一个静态方法。因此只需要在config.cc里面调用以下就能够初始化
public:
    // typedef std::shared_ptr<Config> ptr;
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;

    template<class T>
    static typename ConfigVar<T>::ptr lookup(const std::string& name, const T& default_value, const std::string& description=""){
        auto tmp = lookup<T>(name);
        if(tmp){
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "lookup name=" << name << " exists";
            return tmp;
        }
        if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz_.1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ")
            != std::string::npos){
            SYLAR_LOG_ERROR(SYLAR_LOG_ROOT()) << "lookup name invalid: " << name;
            throw std::invalid_argument(name);
        }
        auto m = typename ConfigVar<T>::ptr(new ConfigVar<T>(name, default_value, description));
        // typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        s_datas[name] = m;
        SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "Add new ConfigVar::ptr for " << name;
        return m;
    } 

    template<class T>
    static typename ConfigVar<T>::ptr lookup(const std::string& name){
        auto it = s_datas.find(name);
        if(it == s_datas.end()){
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T>> (it->second);
    } 
private:
static ConfigVarMap s_datas;
    
};


}