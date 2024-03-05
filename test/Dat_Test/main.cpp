#include <iostream>
#include <memory>
#include <variant>
#include <Rut/RxJson.h>
#include <Rut/RxStr.h>
#include <Seraph/Dat.h>
#include <Seraph/Script_Scenario.h>




int main()
{
	Seraph::Dat data1;
	data1.Unpack(L"ArchPac.Dat",0x05EBB40D);

	Seraph::Dat data2;
	data2.UnpackScnPac(L"ScnPac.Dat");

	int a = 0;
}