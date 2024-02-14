#include <iostream>
#include <Seraph/Dat.h>


int main()
{
	Seraph::Dat data(L"ScnPac.Dat");
	data.ReadIndex();
	data.Unpack();

	int a = 0;
}