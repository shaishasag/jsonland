#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <string_view>
#include <filesystem>
#include <fstream>

#include "json_node.h"
using namespace jsonland;
class serializer_base;

template<typename TSerialable> concept IsSerialable =
            IsJsonScalarType<TSerialable>
            || requires (TSerialable& mapped, serializer_base& sb){mapped.jsonland_serialize(sb);}
            || requires (TSerialable& mapped, serializer_base& sb){jsonland_serialize(mapped, sb);};


template <typename TAssociative>
concept IsAssociative = IsString<typename TAssociative::key_type>
                        && IsSerialable<typename TAssociative::mapped_type>
                        && requires(TAssociative container, typename TAssociative::key_type key)
                        {
                            { container.find(key) } -> std::same_as<typename TAssociative::iterator>; // Must have find()
                            { container.count(key) } -> std::convertible_to<std::size_t>;  // Must have count()
                        };

template <typename TIterable>
concept IsIterable =
    requires(TIterable container) {
            typename TIterable::value_type;
            { std::begin(container) } -> std::input_or_output_iterator; // Must have a begin() method
            { std::end(container) } -> std::input_or_output_iterator;   // Must have an end() method
    }
    && IsSerialable<typename TIterable::value_type>;

class serializer_base
{
protected:

    enum role
    {
        ser_base,
        ser_read,
        ser_write,
        ser_null
    };

    serializer_base(jsonland::json_node& in_j, role in_role)
    : my_j(in_j)
    , my_role(in_role)
    {
    }

public:
    template<typename TToSer>
    void serialize(TToSer& to_ser)
    {
        if constexpr (requires { to_ser.jsonland_serialize(*this); })
        {   // member function TToSer::jsonland_serialize exists
            to_ser.jsonland_serialize(*this);
        }
        else if constexpr (requires { jsonland_serialize(to_ser, *this); })
        {   // free function jsonland_serialize(TToSer&,serializer_base&) exists
            jsonland_serialize(to_ser, *this);
        }
        else if constexpr (IsAssociative<TToSer>)
        {
            serialize_map(to_ser);
        }
        else if constexpr (IsIterable<TToSer>)
        {
            serialize_array(to_ser);
        }
        else if constexpr (IsJsonScalarType<TToSer>)
        {
            serialize_scalar(to_ser);
        }
    }

    template<IsJsonScalarType TScalar>
    void serialize_scalar(TScalar& to_ser)
    {
        if (my_role == ser_read)
        {
            to_ser = my_j.get_as<TScalar>();
        }
        else if (my_role == ser_write)
        {
            my_j = to_ser;
        }
    }

    template<IsAssociative TMap>
    void serialize_map(TMap& container)
    {
        if (my_role == ser_read)
        {
            for (auto& key_value : my_j)
            {
                using key_t = typename TMap::key_type;
                using value_t = typename TMap::mapped_type;
                container[key_t{key_value.key()}] = key_value.get<value_t>();
            }
        }
        else if (my_role == ser_write)
        {
            my_j = jsonland::object_t; // make sure it's an object even if container is empty
            for (auto& [key, value] : container)
            {
                my_j[key] = value;
            }
        }
    }

    template<IsIterable TArr>
    void serialize_array(TArr& container)
    {
        if (my_role == ser_read)
        {
            using value_t = typename TArr::value_type;
            for (auto& value_j : my_j)
            {
                value_t& new_value = container.emplace_back();
                serializer_base new_ser(value_j, ser_read);
                new_ser.serialize(new_value);
            }
        }
        else if (my_role == ser_write)
        {
            my_j = jsonland::array_t; // make sure it's an array even if container is empty
            for (auto& value : container)
            {
                jsonland::json_node& new_j = my_j.emplace_back();
                serializer_base new_ser(new_j, ser_write);
                new_ser.serialize(value);
            }
        }
    }

    serializer_base operator[](std::string_view in_key)
    {
        if ((my_role == ser_read && my_j.contains(in_key)) || my_role == ser_write)
        {
            return serializer_base(my_j[in_key], my_role);
        }
        else
        {
            return serializer_base(my_j, ser_null);
        }
    }

    role my_role{ser_base};
    jsonland::json_node& my_j;
};

class serializer_read : public serializer_base
{
public:
    serializer_read(jsonland::json_node& in_j)
    : serializer_base(in_j, ser_read)
    {
    }
};

class serializer_write : public serializer_base
{
public:
    serializer_write(jsonland::json_node& in_j)
    : serializer_base(in_j, ser_write)
    {
    }
};

class Friend
{
public:
    std::string name;
    std::vector<std::string> hobbies;
};

void jsonland_serialize(Friend& in_frnd, serializer_base& rj)
{
    rj["name"].serialize(in_frnd.name);
    rj["hobbies"].serialize(in_frnd.hobbies);
}

class User
{
public:
    int id{-1};
    std::string name;
    std::string city;
    int age{0};
    std::map<std::string, int> shop;
    std::vector<Friend> friends;

    void jsonland_serialize(serializer_base& rj)
    {
        rj["id"].serialize(id);
        rj["name"].serialize(name);
        rj["city"].serialize(city);
        rj["age"].serialize(age);

        auto shopping_ser = rj["shop"];
        shopping_ser.serialize(shop);

        static_assert(IsIterable<decltype(friends)>, "??");
        auto friends_ser = rj["friends"];
        friends_ser.serialize(friends);
    }
};


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

    std::filesystem::path input_path(argv[1]);
    std::ifstream input_file(input_path, std::ios::in | std::ios::binary);
    if (!input_file.is_open())
    {
        std::cout << "failed to open file" << std::endl;
        return 0;
    }

    // Read contents
    std::string content{std::istreambuf_iterator<char>(input_file), std::istreambuf_iterator<char>()};

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
    reader.serialize(users_vec);

    statistics_users(users_vec);

    jsonland::json_node out_users_j(jsonland::array_t);
    serializer_write writer(out_users_j);
    writer.serialize(users_vec);

    std::filesystem::path output_path(input_path);
    output_path.replace_extension("out.json");

    std::ofstream output_file(output_path, std::ios::out | std::ios::binary);

    output_file << out_users_j.dump(jsonland::dump_style::pretty) << std::endl;
    output_file.close();
}
