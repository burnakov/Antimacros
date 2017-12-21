#include "Memory.h"
#include <fstream>

using std::cout;
using std::string;

int main(int argc, char* argv[]) {
	SetConsoleTitle("ANTIMACROS");
	char* TARGET_PROCESS_NAME = "client_203.exe";
	LPCWSTR TARGET_WINDOW_NAME = L"Ultima Online - miner (Forgotten World)";

	HANDLE processHandle;
	int baseAddress;
	int processId;

	int BASE_OFFSET = 0x76614C;
	int GUMP_OFFSET = 0xC0;
	int ID_OFFSET = 0x24;
	int X_OFFSET = 0x14;
	int Y_OFFSET = 0x18;
	int FIRST_IMG_OFFSETS[] = { 0x30,  0x30,  0x30, 0x0 };
	int SECOND_IMG_OFFSETS[] = { 0x0 };


	Memory Memory;
	Memory.GetDebugPrivileges();

	//processId = Memory.GetProcessId(TARGET_PROCESS_NAME);
	processId = Memory.GetProcessByName(TARGET_WINDOW_NAME);
	string path = argv[0]; //program full path + name of binary file
	path.erase(path.find_last_of('\\') + 1);//remove name of binary file
	std::ofstream file;
	file.open(path + "antimacros.txt");
	file.close();
	if (processId == 0)
	{
		cout << "Window not found!" << std::endl;
		std::cin.get();
		return 0;
	}
	processHandle = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
	baseAddress = Memory.GetModuleBase(processHandle, (string)TARGET_PROCESS_NAME);
	int oldGumpAdress = 0;
	int newGumpAdress = Memory.ReadInt(processHandle, baseAddress + BASE_OFFSET);
	short gumpID = Memory.ReadShort(processHandle, newGumpAdress);
	newGumpAdress = Memory.ReadInt(processHandle, newGumpAdress + GUMP_OFFSET);
	if (gumpID == 22304 && newGumpAdress != oldGumpAdress)
	{
		int gumpAdress = newGumpAdress + 0x30;
		int firstImgAdress = Memory.GetPointerAddress(processHandle, gumpAdress, FIRST_IMG_OFFSETS, 4);

		int secondBase = firstImgAdress + 0x30;
		int secondImgAdress = Memory.GetPointerAddress(processHandle, secondBase, SECOND_IMG_OFFSETS, 1);

		int firstImgID = Memory.ReadInt(processHandle, firstImgAdress + ID_OFFSET);
		int secondImgID = Memory.ReadInt(processHandle, secondImgAdress + ID_OFFSET);

		int firstNumber = firstImgID - 2224;
		int secondNumber = secondImgID - 2224;

		int coordBase = secondImgAdress + 0x30;
		for (int i = 0; i < 3; i++)
		{
			coordBase = Memory.ReadInt(processHandle, coordBase);
			coordBase += 0x30;
		}
		coordBase = Memory.ReadInt(processHandle, coordBase);
		int x_1, x_2, y_1, y_2, id;
		x_1 = x_2 = y_1 = y_2 = id = 0;
		for (int i = 0; i < 8; i++)
		{
			id = Memory.ReadInt(processHandle, coordBase + ID_OFFSET);
			if (x_1 == 0 || x_2 == 0)
			{
				if (id == firstImgID)
				{
					x_1 = Memory.ReadInt(processHandle, coordBase + X_OFFSET);
					y_1 = Memory.ReadInt(processHandle, coordBase + Y_OFFSET);
				}
				else if (id == secondImgID)
				{
					x_2 = Memory.ReadInt(processHandle, coordBase + X_OFFSET);
					y_2 = Memory.ReadInt(processHandle, coordBase + Y_OFFSET);
				}
			}
			else
				break;
			coordBase += 0x30;
			coordBase = Memory.ReadInt(processHandle, coordBase);
		}
		if (firstNumber == secondNumber)
		{
			x_2 = x_1;
			y_2 = y_1;
		}
		std::ofstream file;
		file.open(path + "antimacros.txt");
		if (argc == 1)
		{
			file << x_1 << std::endl;
			file << y_1 << std::endl;
			file << x_2 << std::endl;
			file << y_2 << std::endl;
		}
		else if (argc == 2)
		{
			string arg = argv[1];
			file << arg << std::endl;
			if (arg == "1")
			{
				file << x_1 << std::endl;
				file << y_1 << std::endl;
			}
			else if (arg == "2")
			{
				file << x_2 << std::endl;
				file << y_2 << std::endl;
			}
		}
		file.close();
		//for (int i = 0; i < argc; i++)
		//{
		//	cout << argv[i] << std::endl;
		//}
		//cout << "First image ID: " << firstImgID << ", Number: " << firstNumber
		//	<< ", X: " << x_1 << ", Y: " << y_1 << std::endl;
		//cout << "Second image ID: " << secondImgID << ", Number: " << secondNumber
		//	<< ", X: " << x_2 << ", Y: " << y_2 << std::endl;
	}
	//oldGumpAdress = newGumpAdress;
	//std::cin.get();
	return 0;
}