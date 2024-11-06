#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <filesystem>
#include <fstream>

#include "json_node.h"
using namespace jsonland;

class Serializer
{
public:

    virtual Serializer Object() { return Serializer(m_the_j); }
    virtual Serializer Object(std::string_view in_obj_name) { return Serializer(m_the_j); }
    virtual Serializer Array() { return Serializer(m_the_j); }
    virtual Serializer Array(std::string_view in_obj_name) { return Serializer(m_the_j); }

    virtual void ObjectItem(std::string_view item_name, int& item_value) {}
    virtual void ObjectItem(std::string_view item_name, std::string& item_value) {}

    virtual void String(std::string& in_str) {}
    virtual void Number(int in_num) {}

    template<typename TCONTAINer>
    void ARR(TCONTAINer& in_container)
    {
        if (m_reading)
        {
            for (auto& item_j : m_the_j)
            {
                auto& item = in_container.emplace_back();
                item.Serialize(item_j);
            }
        }
        else
        {
            for (auto& item : in_container)
            {
                
            }
        }
    }

protected:
    Serializer(jsonland::json_node& in_the_j)
    : m_the_j(in_the_j) {}

    jsonland::json_node& m_the_j;
    bool m_reading{true};
};

class SerializerWriter : public Serializer
{
public:
    SerializerWriter(jsonland::json_node& in_the_j)
    : Serializer(in_the_j)
    {
        m_reading = false;
    }

    Serializer Object() override
    {
        m_the_j = array_t;
        jsonland::json_node& new_obj = m_the_j.append_object();
        return SerializerWriter(new_obj);
    }
    Serializer Object(std::string_view in_obj_name) override
    {
        m_the_j = object_t;
        jsonland::json_node& new_obj = m_the_j.append_object(in_obj_name);
        return SerializerWriter(new_obj);
    }
    Serializer Array() override
    {
        m_the_j = array_t;
        jsonland::json_node& new_obj = m_the_j.append_array();
        return SerializerWriter(new_obj);
    }
    Serializer Array(std::string_view in_obj_name) override
    {
        m_the_j = object_t;
        jsonland::json_node& new_obj = m_the_j.append_array(in_obj_name);
        return SerializerWriter(new_obj);
    }

    void ObjectItem(std::string_view item_name, int& item_value) override
    {
        m_the_j[item_name] = item_value;
    }
    void ObjectItem(std::string_view item_name, std::string& item_value) override
    {
        m_the_j[item_name] = item_value;
    }

    void String(std::string& in_str) override
    {
        m_the_j = in_str;
    }
    void Number(int in_num) override
    {
        m_the_j = in_num;
    }
};

class SerializerReader : public Serializer
{
public:
    SerializerReader(jsonland::json_node& in_the_j)
    : Serializer(in_the_j)
    {
        m_reading = true;
    }


    Serializer Object() override
    {
        m_the_j = array_t;
        jsonland::json_node& new_obj = m_the_j.append_object();
        return SerializerReader(new_obj);
    }
    Serializer Object(std::string_view in_obj_name) override
    {
        m_the_j = object_t;
        jsonland::json_node& new_obj = m_the_j.append_object(in_obj_name);
        return SerializerReader(new_obj);
    }
    Serializer Array() override
    {
        m_the_j = array_t;
        jsonland::json_node& new_obj = m_the_j.append_array();
        return SerializerReader(new_obj);
    }
    Serializer Array(std::string_view in_obj_name) override
    {
        m_the_j = object_t;
        jsonland::json_node& new_obj = m_the_j.append_array(in_obj_name);
        return SerializerReader(new_obj);
    }
    void ObjectItem(std::string_view item_name, int& item_value) override
    {
        item_value = m_the_j[item_name].get_int<int>();
    }
    void ObjectItem(std::string_view item_name, std::string& item_value) override
    {
        item_value = m_the_j[item_name].get_string();
    }
    void String(std::string& in_str) override
    {
        in_str = m_the_j.get_string();
    }
    void Number(int in_num) override
    {
        in_num = m_the_j.get_int<int>();
    }
};

class Member
{
public:
    Member() = default;
    Member(std::string_view in_1st_name, std::string_view in_last_name)
    : m_1st_name(in_1st_name)
    , m_last_name(in_last_name) {}

    std::string m_1st_name;
    std::string m_last_name;

    void ToJson(json_node& out_j)
    {
        out_j["1st name"] = m_1st_name;
        out_j["last name"] = m_last_name;
    }

    void FromJson(json_node& in_j)
    {
        SerializerReader rdr(in_j);
        m_1st_name = in_j["1st name"].get_string();
        m_last_name = in_j["last name"].get_string();
    }

    void Serialize(Serializer& in_seri)
    {
        in_seri.ObjectItem("1st name", m_1st_name);
        in_seri.ObjectItem("last name", m_last_name);
    }
};

std::ostream& operator<<(std::ostream& os, const Member& memb)
{
    os << memb.m_1st_name << " " << memb.m_last_name;
    return os;
}

class Band
{
public:
    Band() = default;
    Band(std::string_view in_name) : m_name(in_name) {}

    std::string m_name;
    std::vector<Member> m_members;

    void ToJson(json_node& out_j)
    {
        out_j["name"] = m_name;
        auto& members_j = out_j.append_array("members");
        for (auto& mem : m_members)
        {
            auto& member_j  = members_j.append_object();
            SerializerWriter wrtr(member_j);
            mem.Serialize(wrtr);
        }
    }

    void FromJson(json_node& in_j)
    {
        m_name = in_j["name"].get_string();
        for (auto& mem_j : in_j["members"])
        {
            Member& mem = m_members.emplace_back();
            SerializerReader rdr(mem_j);
            mem.Serialize(rdr);
        }
    }

    void Serialize(Serializer& in_seri)
    {
        in_seri.ObjectItem("name", m_name);
        auto members_ser = in_seri.Array("members");
    }
};


std::ostream& operator<<(std::ostream& os, const Band& band)
{
    os << band.m_name << ":\n";
    for (auto& memb : band.m_members)
    {
        std::cout << "    " << memb << "\n";
    }
    return os;
}

int main(int argc, char* argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        Band my_band;
        std::filesystem::path a_j_file = std::filesystem::path(argv[i]);

        {
            std::cout << a_j_file << std::endl;
            std::ifstream ifs(a_j_file);
            std::string contents = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

            jsonland::json_doc parse_j;
            parse_j.parse_insitu(contents);
            std::cout << parse_j.dump() << std::endl << std::endl;
            my_band.FromJson(parse_j);
            std::cout << my_band << std::endl << "..." << std::endl;
        }

        {
            jsonland::json_node write_j;
            my_band.ToJson(write_j);

            std::filesystem::path out_j_file = a_j_file;
            auto new_ext = ".out"+out_j_file.extension().string();
            out_j_file.replace_extension(new_ext);
            std::ofstream ofs(out_j_file);
            ofs << write_j;
        }

    }
}
