
#include "json_node.h"
#include "json_creator.h"

#include<array>
#include<vector>
#include<tuple>

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
    std::string str;
    jsonland::array_creator array(str);
    array.append("Dancing Queen");
    array.append("Gimme", "Gimme", "Gimme");
    std::vector<float> f_vec{17.f, 18.f};
    array.extend(f_vec);
    std::vector<std::string_view> sv_vec{"Mama", "Mia"};
    array.extend(sv_vec);
    

    std::cout << str << std::endl;
}
static void create_object()
{
    std::string str;
    jsonland::object_creator object(str);
    object.append("People Need Love", 1972);
    object.append("Nina, Pretty Ballerina", 1973);
    object.append("Waterloo", 1974);

    std::cout << str << std::endl;
}
static void create_object_and_array()
{
    std::string str;
    jsonland::object_creator album(str);
    
    album.append("Artist", "ABBA");
    album.append("Name", "Arrival");
    album.append("Year", 1976);
    album.append("Studio", "Metronome");
    album.append("Time", 33.009876);

    jsonland::array_creator personnel = album.append_array("Personnel");
    personnel.append("👩🏼‍👩🏼‍👦🏼‍👦🏼");
    personnel.append("Agnetha Fältskog");
    personnel.append("Anni-Frid Lyngstad");
    personnel.append("Björn Ulvaeus");
    personnel.append("Benny Andersson");

    jsonland::object_creator tracks = album.append_object("Tracks");
    
    jsonland::array_creator side_1 = tracks.append_array("Side 1");
    side_1.append("When I Kissed the Teacher");
    side_1.append("Dancing Queen", "My Love, My Life");
    side_1.append("Dum Dum Diddle", "Knowing Me, Knowing You");
    
    jsonland::array_creator side_2 = tracks.append_array("Side 2");
    side_2.append("Money, Money, Money");
    std::array<std::string_view, 2> songs_array{"That's Me", "Why Did It Have to Be Me?"};
    side_2.extend(songs_array);
    std::vector<std::string_view> songs_vector{"Tiger", "Arrival"};
    side_2.extend(songs_vector);

    std::cout << str << std::endl;
}

static void create()
{
    create_array();
    create_object();
    create_object_and_array();
}

int main()
{
    create();
    
    return 0;
}
