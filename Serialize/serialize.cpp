#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <filesystem>
#include <fstream>

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
    std::vector<Friend> friends;
};

class serializer_base
{
public:
    serializer_base(jsonland::json_node& in_j, serializer_base*p)
    : j(in_j)
    , the_real_me(p)
    {
    }
    virtual ~serializer_base() = default;

    serializer_base* self() { return the_real_me; }
    jsonland::json_node& my_j() { return self()->j; }

    serializer_base object_item(std::string_view item_name)
    {
        return self()->_object_item(item_name);
    }
    void number(int& i)
    {
        serializer_base* me = self();
        me->_number(i);
    }


private:
    virtual serializer_base _object_item(std::string_view)
    {
        return *this;
    }
    virtual void _number(int& i)
    {

    }

    jsonland::json_node& j;
    serializer_base* the_real_me{nullptr};

};

class serializer_read : public serializer_base
{
public:
    serializer_read(jsonland::json_node& in_j)
    : serializer_base(in_j, this)
    {
    }

    serializer_base _object_item(std::string_view item_name) override
    {
        serializer_read retVal(my_j()[item_name]);
        return retVal;
    }

    void _number(int& a_number) override
    {
        a_number = my_j().get_int<int>();
    }

};

class serializer_write : public serializer_base
{
public:
    serializer_write(jsonland::json_node& in_j)
    : serializer_base(in_j, this)
    {
    }


    serializer_base _object_item(std::string_view item_name) override
    {
        jsonland::json_node& new_obj_item = my_j()[item_name];
        serializer_write retVal(new_obj_item);
        return retVal;
    }

    void _number(int& a_number) override
    {
        jsonland::json_node& my_real_j = my_j();
        my_real_j = a_number;
    }
};

static void read_a_user(serializer_read& rj, User& a_user)
{
    auto id_item = rj.object_item("id");
    id_item.number(a_user.id);

    a_user.name = rj.my_j()["name"].get_string();
    a_user.city = rj.my_j()["city"].get_string();
    a_user.age = rj.my_j()["age"].get_int<int>();

    auto& friends_j = rj.my_j()["friends"];
    for (auto& friend_j : friends_j)
    {
        auto& a_friend = a_user.friends.emplace_back();
        a_friend.name = friend_j["name"].get_string();
        for (auto& hobby_j : friend_j["hobbies"])
        {
            a_friend.hobbies.emplace_back(hobby_j.get_string());
        }
    }
}

static void read_users(serializer_read& rj, std::vector<User>& users_vec)
{
    for (auto& user_j : rj.my_j())
    {
        User& a_user = users_vec.emplace_back();
        serializer_read urj(user_j);
        read_a_user(urj, a_user);
    }
}

static void write_a_user(User& a_user, serializer_write& wj)
{
    auto id_item = wj.object_item("id");
    id_item.number(a_user.id);

    wj.my_j()["name"] = a_user.name;
    wj.my_j()["city"] = a_user.city;
    wj.my_j()["age"] = a_user.age;

    if (!a_user.friends.empty())
    {
        auto& friends_j = wj.my_j().append_array("friends");
        for (auto& a_friend : a_user.friends)
        {
            auto& friend_j = friends_j.append_object();
            friend_j["name"] = a_friend.name;

            auto& hobbies_j = friend_j.append_array("hobbies");
            for (auto& a_hobby : a_friend.hobbies)
            {
                hobbies_j.push_back(a_hobby);
            }
        }
    }
}

static void write_users(std::vector<User>& users_vec, serializer_write& wj)
{
    for (auto& a_user : users_vec)
    {
        auto& user_j = wj.my_j().append_object();
        serializer_write uwj(user_j);
        write_a_user(a_user, uwj);
     }
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
    serializer_read reader(j);
    read_users(reader,  users_vec);

    statistics_users(users_vec);

    jsonland::json_node out_users_j(jsonland::array_t);
    serializer_write writer(out_users_j);
    write_users(users_vec, writer);
    std::cout << out_users_j.dump() << std::endl;
}
