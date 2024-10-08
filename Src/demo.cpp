#include "ti5mcl.hpp"
using namespace ti5mcl;
using namespace std;
int main()
{
    ti5Motor M1(1,ti5Motor::reductionRatio101);
    while(1)
    {

    M1.home();
    this_thread::sleep_for(std::chrono::seconds(5));
    M1.moveAbsolute(PI/4,PI/6);
    this_thread::sleep_for(std::chrono::seconds(5));
   }
    return 0;
}
