#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <filesystem>
#include <fstream>
#include <map>

#include "json_node.h"
using namespace jsonland;

class Friend
{
public:
    std::string name;
    std::vector<std::string> hobbies;
};

class User
{
public:
    int id{-1};
    std::string name;
    std::string city;
    int age{0};
    std::map<std::string, int> shopping;
    std::vector<Friend> friends;
};

class serializer_impl
{
    friend class serializer_base;
protected:
    virtual json_node& _object_item(json_node&, std::string_view) = 0;
    virtual void _number(json_node&, int&) = 0;
    virtual void _string(json_node& in_j, std::string& a_str) = 0;
    virtual void _boolean(json_node& in_j, bool& a_bool) = 0;
};

class serializer_read_impl : public serializer_impl
{
public:

    json_node& _object_item(json_node& in_j, std::string_view in_name) override
    {
        return in_j[in_name];
    }

    void _number(json_node& in_j, int& a_number) override
    {
        a_number = in_j.get_int<int>();
    }
    void _string(json_node& in_j, std::string& a_str) override
    {
        a_str = in_j.get_string();
    }

    void _boolean(json_node& in_j, bool& a_bool) override
    {
        a_bool = in_j.get_bool();
    }


};

class serializer_write_impl : public serializer_impl
{
public:

    json_node& _object_item(json_node& in_j, std::string_view in_name) override
    {
        return in_j[in_name];
    }

    void _number(json_node& in_j, int& a_number) override
    {
        in_j = a_number;
    }
    void _string(json_node& in_j, std::string& a_str) override
    {
        in_j = a_str;
    }
    void _boolean(json_node& in_j, bool& a_bool) override
    {
        in_j = a_bool;
    }
};

class serializer_base
{
public:
    virtual ~serializer_base() = default;


    serializer_base object_item(std::string_view item_name)
    {
        json_node& item_node = the_impl->_object_item(j, item_name);
        return serializer_base(item_node, the_impl);
    }
    void number(int& i)
    {
        the_impl->_number(j, i);
    }

    void string(std::string& in_str)
    {
        the_impl->_string(j, in_str);
    }

    void boolean(bool& in_bool)
    {
        the_impl->_boolean(j, in_bool);
    }

protected:
    serializer_base(json_node& in_j, serializer_impl*p)
    : j(in_j)
    , the_impl(p)
    {
    }

private:
    virtual serializer_base _object_item(std::string_view)
    {
        return *this;
    }
    virtual void _number(int& i)
    {

    }

    json_node& j;
    serializer_impl* the_impl{nullptr};

};

class serializer_reader : public serializer_base
{
public:
    serializer_reader(json_node& in_j)
    : serializer_base(in_j, new serializer_read_impl) {}
};

class serializer_writer : public serializer_base
{
public:
    serializer_writer(json_node& in_j)
    : serializer_base(in_j, new serializer_write_impl) {}
};

static void serialize_a_user(serializer_base& rj, User& a_user)
{
    auto id_item = rj.object_item("id");
    id_item.number(a_user.id);

    rj.object_item("name").string(a_user.name);
    rj.object_item("city").string(a_user.city);
    rj.object_item("age").number(a_user.age);

//    auto& friends_j = rj.my_j()["friends"];
//    for (auto& friend_j : friends_j)
//    {
//        auto& a_friend = a_user.friends.emplace_back();
//        a_friend.name = friend_j["name"].get_string();
//        for (auto& hobby_j : friend_j["hobbies"])
//        {
//            a_friend.hobbies.emplace_back(hobby_j.get_string());
//        }
//    }
}

static void write_users(std::vector<User>& users_vec, json_node& wj)
{
    for (auto& a_user : users_vec)
    {
        auto& user_j = wj.append_object();
        serializer_writer uwj(user_j);
        serialize_a_user(uwj, a_user);
     }
}

static void read_users(json_node& rj, std::vector<User>& users_vec)
{
    for (auto& user_j : rj)
    {
        User& a_user = users_vec.emplace_back();
        serializer_reader urj(user_j);
        serialize_a_user(urj, a_user);
    }
}

static void lululin(User& a_user, serializer_writer& wj)
{
//    if (!a_user.friends.empty())
//    {
//        auto& friends_j = wj.my_j().append_array("friends");
//        for (auto& a_friend : a_user.friends)
//        {
//            auto& friend_j = friends_j.append_object();
//            friend_j["name"] = a_friend.name;
//
//            auto& hobbies_j = friend_j.append_array("hobbies");
//            for (auto& a_hobby : a_friend.hobbies)
//            {
//                hobbies_j.push_back(a_hobby);
//            }
//        }
//    }
}

static void statistics_users(std::vector<User>& users_vec)
{
    if (users_vec.empty())
    {
        std::cout << "no users in users_vec" << std::endl;
        return;
    }

    int youngest{users_vec[0].age};
    int oldest{users_vec[0].age};
    size_t min_friends{users_vec[0].friends.size()};
    size_t max_friends{users_vec[0].friends.size()};
    int age_total{0};
    int friends_total{0};
    std::unordered_map<std::string_view, int> cities;
    for (auto& u : users_vec)
    {
        youngest = std::min(youngest, u.age);
        oldest = std::max(oldest, u.age);
        min_friends = std::min(min_friends, u.friends.size());
        max_friends = std::max(max_friends, u.friends.size());
        oldest = std::max(oldest, u.age);
        age_total += u.age;
        friends_total += u.friends.size();
        cities[u.city] += 1;
    }

    float age_average = (float)age_total / (float)users_vec.size();
    float friends_average = (float)friends_total / (float)users_vec.size();

    std::cout << users_vec.size() << " users" << std::endl;
    std::cout << cities.size() << " cities" << std::endl;
    std::cout << age_average << " average age" << std::endl;
    std::cout << youngest << " youngest" << std::endl;
    std::cout << oldest << " oldest" << std::endl;
    std::cout << friends_average << " average num friends" << std::endl;
    std::cout << min_friends << " min num friends" << std::endl;
    std::cout << max_friends << " max num friends" << std::endl;
}



int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "no file given" << std::endl;
        return 0;
    }

    std::filesystem::path p(argv[1]);
    std::ifstream file(p, std::ios::in | std::ios::binary);
    if (!file.is_open())
    {
        std::cout << "failed to open file" << std::endl;
        return 0;
    }

    // Read contents
    std::string content{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

    jsonland::json_doc j;
    j.parse_insitu(content);
    if (j.parse_error() != 0)
    {
        std::cout << "parse error: "<< j.parse_error() << std::endl;
        std::cout << "error message: "<< j.parse_error_message() << std::endl;
        return -1;

    }
    std::cout << "memory_consumption: "<< j.memory_consumption() << std::endl;

    std::vector<User> users_vec;
    read_users(j,  users_vec);

    statistics_users(users_vec);

    json_node out_users_j(jsonland::array_t);
    write_users(users_vec, out_users_j);
    std::cout << out_users_j.dump() << std::endl;
}
