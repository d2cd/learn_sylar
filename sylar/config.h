#include<sstream>
#include<memory>
#include<string>
#include<boost/lexical_cast.hpp>
#include"log.h"
#include<yaml-cpp/yaml.h>


namespace sylar{
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
            ss << boost::lexical_cast<std::string>(vec[i]);
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
        for(size_t i=0; i<node.size(); i++){
            std::stringstream ss;
            ss.str("");
            ss << node[i];
            ret.push_back(boost::lexical_cast<T>(ss.str()));
        }
        return ret;
    }
    
};

template<class T, class FromStr = LexicalCast<std::string, T>, class ToStr = LexicalCast<T, std::string>>
class ConfigVar:public ConfigVarBase{
public:
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
    static typename ConfigVar<T>::ptr Lookup(const std::string& name, const T& default_value, const std::string& description=""){
        auto tmp = Lookup<T>(name);
        if(tmp){
            SYLAR_LOG_INFO(SYLAR_LOG_ROOT()) << "lookup name=" << name << " exists";
            return tmp;
        }
        if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz_.1234567890")
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
    static typename ConfigVar<T>::ptr Lookup(const std::string& name){
        auto it = s_datas.find(name);
        if(it == s_datas.end()){
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T>> (it->second);
    } 
    static ConfigVarBase::ptr LookupBase(const std::string& name);


    static void LoadFromYaml(const YAML::Node& node);
private:
static ConfigVarMap s_datas;
    
};


}