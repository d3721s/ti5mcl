#include "ti5mcl.hpp"
using namespace ti5mcl;
using namespace std;
int main()
{
    ti5Motor M1(1, ti5Motor::reductionRatio101);
    ti5Motor M2(2, ti5Motor::reductionRatio101);
    while (1)
    {

        M1.home();
        M2.home();
        this_thread::sleep_for(std::chrono::seconds(5));

        M1.moveRelative(M_PI / 4);
        M2.moveAbsolute(M_PI / 4);
        this_thread::sleep_for(std::chrono::seconds(5));
        M1.moveRelative(-M_PI / 2);
        M2.moveAbsolute(M_PI / 4);
        this_thread::sleep_for(std::chrono::seconds(5));
        M1.reset();
    }
    return 0;
}
