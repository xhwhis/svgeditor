#include "utility.h"

namespace lwscode {

int gcd(int a, int b)
{
	int r;
	while ((r = a % b)) {
		a = b;
		b = r;
	}
	return b;
}

int scm(int a, int b)
{
	int g = gcd(a, b);
	return a / g * b;
}

} // namespace
