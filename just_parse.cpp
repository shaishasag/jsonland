#include <string>
#include <string_view>

#include "fstring/fstring.h"
#include "fstring/fstringstream.h"

class ObjBass
{
public:
    virtual void add(std::string_view) {}
    virtual operator std::string_view() const { return std::string_view(); }
};

template<typename TStringo>
class Obj : public ObjBass
{
public:
    Obj(TStringo& in_bob) : m_bob(in_bob) {}

    void add(std::string_view in_sv) override
    {
        m_bob += in_sv;
    }

    Obj<TStringo> gime_bob()
    {
        return Obj<TStringo>(m_bob);
    }

    operator std::string_view() const override
    {
        return std::string_view(m_bob);
    }

protected:
    TStringo& m_bob;
};

template<typename TStringo>
class ObjBoss : public Obj<TStringo>
{
public:
    ObjBoss() : Obj<TStringo>(m_uncle) {}

    TStringo m_uncle;
};

template<typename TStringo>
std::ostream& operator<<(std::ostream& os, const ObjBass& obj)
{
    os << std::string_view(obj);
    return os;
}

using ObjStd = Obj<std::string>;
using ObjBossStd = ObjBoss<std::string>;

using ObjFixed = Obj<fstr::fstr127>;
using ObjBossFixed = ObjBoss<fstr::fstr127>;

void SayHillaLululin(ObjBass& ob)
{
    ob.add("Hilla");

    auto bob1 = ob.gime_bob();
    bob1.add(" ");
    auto bob2 = bob1.gime_bob();
    bob2.add("Lululin");

    std::cout << std::string_view(ob) << std::endl;
    std::cout << std::string_view(bob2) << std::endl;
}
;
int main(int argc, char* argv[])
{
    {
        ObjBossStd ob;
        SayHillaLululin(ob);
    }
    {
        ObjBossFixed ob;
        SayHillaLululin(ob);
    }
}
