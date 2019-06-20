#include "Bond.hpp"

#include <iostream>
#include <vector>

int main(void)
{
	Bond bond(100, 0, 10, 2, std::vector<double>());
	bond.setFlatCoupons(1.00);
	std::cout << bond << '\n';
	for (int y = 0; y <= 10; y += 1)
		{std::cout << y << " " << bond.FairValue(0.0, y) << '\n';}
	double y;
	int a;
	yield(y, a, bond, 85, 0.0);
	std::cout << "yield = " << y << '\n';
	std::cout << "iterations = " << a << '\n';
	return 0;
}
