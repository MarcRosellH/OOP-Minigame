#pragma once

#include <iostream>
#include <stdio.h>
#include <fstream>

#include "app.h"
#include "memleak.h"
#include "utils.h"

#include "module.h"

enum Main_State
{
	MAIN_INITIALIZATION,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

int main()
{
	// Application State control flow
	Main_State state = MAIN_INITIALIZATION;
	App* app = nullptr;
	Update_State update_state;

	// Initialize crtdbg to detect memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	std::cout << "OOP simulation of " << MAX_OBJECTS << " objects during " << MAX_TIME << " seconds:" << std::endl;
	std::cout << "Average values above may differ due to calculating them using incremental averaging!" << std::endl;

	// Application loop
	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_INITIALIZATION:
			LOG("Creating application...");
			app = DBG_NEW App();
			ASSERT(app->initialize(), "Error when initializing the application!");
			state = MAIN_UPDATE;
			LOG("Update application loop...");
			break;

		case MAIN_UPDATE:
			update_state = app->update();
			ASSERT(update_state != UPDATE_ERROR, "Error when updating the application!");
			(update_state == UPDATE_STOP) ? state = MAIN_FINISH : 0;
			break;

		case MAIN_FINISH:
			LOG("Cleaning up application...");
			ASSERT(app->clean_up(), "Error when cleaning up the application!");
			state = MAIN_EXIT;
			break;
		}
	}

	std::string name = std::string("output_" + std::to_string(MAX_OBJECTS) + "_objects_in_" + std::to_string((int)MAX_TIME)+".txt");
	std::ofstream outfile(name);
	for (unsigned long long i = 0; i < app->delta_time_list.size(); ++i)
	{
		outfile << app->delta_time_list[i] << std::endl;
	}

	outfile.close();

	std::cout << "\nAverage update time: " << app->average_delta_time << " seconds" << std::endl;
	std::cout << "Average updates per second: " << app->average_fps  << std::endl;
	std::cout << "In this time there were a total of " << app->total_created << " objects created." << std::endl;
	std::cout << "In this time there were a total of " << app->total_destroyed << " objects destroyed." << std::endl;
	std::cout << "Update times stored in file " << name << std::endl;
	std::cout << "\nPress any key to close..." << std::endl;
	char a = getchar();
	// Release app data
	RELEASE(app);

	LOG("Program sucessfully ended!\n");
	return 0;
}