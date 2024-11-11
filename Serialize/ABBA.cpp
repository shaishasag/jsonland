
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
