#include "rfpch.hpp"
#include "TestClass.hpp"
#include "Math\Vector.hpp"

TestClass::TestClass(int num)
{
	m_num = num;
}

void TestClass::print()
{
	Rimfrost::Vector3 v(5, 3, 5);
	std::cout << m_num << std::endl;
}
