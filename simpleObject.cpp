#include "simpleObject.h"

SimpleClass::SimpleClass()
{
    std::cout << "SimpleClass constructor" << std::endl;
}

SimpleClass::~SimpleClass()
{
    std::cout << "SimpleClass destructor" << std::endl;
}

void SimpleClass::print()
{
    std::cout << "SimpleClass print" << std::endl;
}