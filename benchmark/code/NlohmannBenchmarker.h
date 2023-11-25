#ifndef __NlohmannBenchmarker_h__
#define __NlohmannBenchmarker_h__

class NlohmannBenchmarker : public JsonBenchmarker
{
public:
    NlohmannBenchmarker() : JsonBenchmarker("Nlohmann") {}
    size_t parse_and_dump(std::string& in_json_string, std::string& out_json_string) override
    {
        using namespace nlohmann;
        
        try
        {
            json nj = json::parse(in_json_string.c_str());
//            for (json::iterator it = nj.begin(); it != nj.end(); ++it)
//            {
//                std::cout << "Nlohmann: " << it.key() << " => " <<  it.value() << "\n";
//            }
            out_json_string = nj.dump();
            return nj.size();
        }
        catch (json::parse_error& e)
        {
            std::cerr << ">>> Nlohmann parse failed: " << e.id << ", " <<  e.what() << " at offset " << e.byte << "\n";
        }
        return 0;
    }
};

#endif // __NlohmannBenchmarker_h__
