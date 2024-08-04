#include "json_node.h"
#include "json_creator.h"

#include <numeric>
#include <array>
#include <vector>
#include <tuple>
#include <span>
#include <fstream>
#if JSONLAND_DEBUG==1
#include "fstring.h"
#endif

#include <nlohmann/json.hpp>


static void personhood()
{
    // Create an instance of json_node
    jsonland::json_node person;

    // Add string value
    person["name"] = "Jane Doe";

    // Add integer value
    person["age"] = 30;

    // Create an array of hobbies
    jsonland::json_node hobbies;
    hobbies.push_back("Reading");
    hobbies.push_back("Hiking");
    hobbies.push_back("Coding");
    
    // Add array to the person object
    person["hobbies"] = hobbies;

    // access some values
    std::string some_values_str = "My name is ";
    some_values_str += person["name"].get_string();
    some_values_str += ", my first hobby is ";
    some_values_str += person["hobbies"][0].get_string();
    std::cout << some_values_str << std::endl;

    // Serialize the whole JSON object to string
    std::string json_string = person.dump();

    // Output the JSON string
    std::cout << json_string << std::endl;

}

static void create_array()
{
    jl_fixed::array_json_creator array;
    array.append_value("Dancing Queen");
    array.append_value("Gimme", "Gimme", "Gimme");
    std::vector<float> f_vec{17.f, 18.f};
    array.extend(f_vec);
    std::vector<std::string_view> sv_vec{"Mama", "Mia"};
    array.extend(sv_vec);
    

    std::cout << std::string_view(array) << std::endl;
}
static void create_object()
{
    jl_fixed::object_json_creator object;
    object.append_value("People Need Love", 1972);
    object.append_value("Nina, Pretty Ballerina", 1973);
    object.append_value("Waterloo", 1974);

    std::cout << std::string_view(object) << std::endl;
}

static void create_object_and_array()
{
    jl_fixed::object_json_creator album;
    
    album.append_value("Artist", "ABBA");
    album.append_value("Name", "Arrival");
    album.append_value("Year", 1976);
    album.append_value("Studio", "Metronome");
    album.append_value("Time", 33.009876);

    auto personnel = album.append_array("Personnel");
    personnel.append_value("👩🏼‍👩🏼‍👦🏼‍👦🏼");
    personnel.append_value("Agnetha Fältskog");
    personnel.append_value("Anni-Frid Lyngstad");
    personnel.append_value("Björn Ulvaeus");
    personnel.append_value("Benny Andersson");

    auto tracks = album.append_object("Tracks");
    
    auto side_1 = tracks.append_array("Side 1");
    side_1.append_value("When I Kissed the Teacher");
    side_1.append_value("Dancing Queen", "My Love, My Life");
    side_1.append_value("Dum Dum Diddle", "Knowing Me, Knowing You");
    
    auto side_2 = tracks.append_array("Side 2");
    side_2.append_value("Money, Money, Money");
    std::array<std::string_view, 2> songs_array{"That's Me", "Why Did It Have to Be Me?"};
    side_2.extend(songs_array);
    std::vector<std::string_view> songs_vector{"Tiger", "Arrival"};
    side_2.extend(songs_vector);

    std::cout << std::string_view(album) << std::endl;
}

static void create()
{
    create_array();
    create_object();
    create_object_and_array();
}

static void all_types()
{
    jl_fixed::object_json_creator object;
    
    object.append_value("True", true);
    object.append_value("False", false);
    object.append_value("float", -1.234500f);
    object.append_value("double", -5.432100f);
    object.append_value("int", (int)-17);
    object.append_value("unsigned int", (unsigned int)17);
    object.append_value("short", (short)-17);
    object.append_value("unsigned short", (unsigned short)17);
    object.append_value("long", (long)-17);
    object.append_value("unsigned long", (unsigned long)17);
    object.append_value("long long", (long long)-17);
    object.append_value("unsigned long long", (unsigned long long)17);

    object.append_value("int16_t", (int16_t)-1787);
    object.append_value("uint16_t", (uint16_t)1787);
    object.append_value("int32_t", (int32_t)-17879796);
    object.append_value("uint32_t", (uint32_t)17879796);
    object.append_value("int64_t", (int64_t)-1787979617879796);
    object.append_value("uint64_t", (uint64_t)1787979617879796);

    const char* p_c = "some dummy text";
    std::string dstr{p_c};
    std::string_view dsv{dstr};
    object.append_value("const char*", p_c);
    object.append_value("std::string", dsv);
    object.append_value("std::string_view", dsv);

    std::cout << std::string_view(object) << std::endl;
}

class JassieJ
{
public:
    explicit JassieJ() = default;
    explicit JassieJ(JassieJ&&) = default;
    explicit JassieJ(const JassieJ&) = default;
    JassieJ& operator=(JassieJ&&) = default;
    JassieJ& operator=(const JassieJ&) = default;
    
    JassieJ& operator[](std::string_view)
    {
        return *this;
    }
};

int main()
{
    JassieJ a;
    JassieJ b(a);
    JassieJ c(a["banana"]);
    
    return 0;
}
