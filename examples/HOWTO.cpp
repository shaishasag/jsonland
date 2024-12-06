#include "json_node.h"
#include <iostream>
#include "HOWTO.h"


void how_to_check_array_size_empty()
{
    // create null json item
    jsonland::json_node j_array;
    j_array.is_array();                    // => false, it's not an array
    j_array.empty_as(jsonland::array_t);   // => true, it's not an array, so considred empty
    j_array.size_as(jsonland::array_t);    // => 0, it's not an array, so size is 0

    // change type to array_t
    j_array = jsonland::array_t;
    j_array.is_array();                    // => true, it's an array
    j_array.empty_as(jsonland::array_t);   // => true, it's an empty array
    j_array.size_as(jsonland::array_t);    // => 0, it's an empty array, so size is 0

    // add some array items
    j_array.push_back("one");
    j_array.push_back("two");
    j_array.push_back("three");

    // now check again, is it a non-empty array? what is the size?
    j_array.empty_as(jsonland::array_t);   // => false, it's and array that has some values
    j_array.size_as(jsonland::array_t);    // => 3, it's an array with 3 items

    if ( ! j_array.empty_as(jsonland::array_t))
    {
        // do somthing with the array
        std::cout << "j_array size is: " << j_array.size_as(jsonland::array_t) << std::endl;
        // => j_array size is: 3
    }

    if (j_array.size_as(jsonland::array_t) >= 3)
    {
        std::cout << "j_array[2] is: " << j_array[2].get_string() << std::endl;
        // => j_array[2] is: three
    }
}

void how_to_loop_an_array_1()
{
    jsonland::json_node j_array(jsonland::array_t);
    j_array.push_back("one");
    j_array.push_back("two");
    j_array.push_back("three");

    // loop homogeneous array - all items expected to be strings
    for (auto& j_array_item : j_array)
    {
        std::cout << j_array_item.get_string() << ", ";
    } // => one, two, three,
    std::cout << std::endl;
}

void how_to_loop_an_array_2()
{
    jsonland::json_node j_array(jsonland::array_t);
    j_array.push_back(1);
    j_array.push_back("two");
    j_array.push_back(3.0);

    // loop heterogeneous array - sum up all numbers
    // get_double() will return 0 if type is not a number
    double total{0.0};
    for (auto& j_array_item : j_array)
    {
        total += j_array_item.get_double();
    }
    std::cout << "total == " << total << std::endl;
    // => total == 4
}



void how_to_check_object_size_empty()
{
    // create null json item
    jsonland::json_node j_object;
    j_object.is_object();                   // => false, it's not an object
    j_object.empty_as(jsonland::object_t);  // => true, it's not an object, so considred empty
    j_object.size_as(jsonland::object_t);   // => 0, it's not an object, so size is 0

    // change type to object_t
    j_object = jsonland::object_t;
    // is it an object and is it non-empty?
    j_object.is_object();                   // => true, it's an object
    j_object.empty_as(jsonland::object_t);  // => true, it's an empty object
    j_object.size_as(jsonland::object_t);   // => 0, it's an empty object, so size is 0

    // add some items to the object 
    j_object["one"] = 1;
    j_object["two"] = 2;
    j_object["three"] = 3;

    // now check again, is it a non-empty object?
    j_object.empty_as(jsonland::object_t);  // => false, it's object has some values
    j_object.size_as(jsonland::object_t);   // => 3, it's an object with 3 items

    if ( ! j_object.empty_as(jsonland::object_t))
    {
        // do somthing with the object
        std::cout << "j_object size is: " << j_object.size_as(jsonland::object_t) << std::endl;
        // => j_object size is: 3
    }

    if (j_object.size_as(jsonland::object_t) >= 3)
    {
        std::cout << "j_object[\"two\"] is: " << j_object["two"].get_int() << std::endl;
        // => j_object["two"] is: 2
    }
}


void how_to_loop_an_object_1()
{
    jsonland::json_node j_object(jsonland::object_t);
    j_object["one"] = 1;
    j_object["two"] = 2;
    j_object["three"] = 3;

    for (auto& j_object_item : j_object)
    {
        std::cout << j_object_item.key() << "==" << j_object_item.get_int() <<  ", ";
    } // => one==1, two==2, three==3,
    std::cout << std::endl;
}
