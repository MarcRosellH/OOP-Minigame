#pragma once

#include <iostream>
#include <stdio.h>
#include <fstream>

#include "app.h"
#include "memleak.h"
#include "utils.h"

#include "module.h"
#include "scene.h"

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
			app->last_time = Clock::now();
			break;

		case MAIN_UPDATE:
		{
			update_state = app->update();
			auto now = Clock::now();
			ASSERT(update_state != UPDATE_ERROR, "Error when updating the application!");

			app->delta_time = std::chrono::duration<float, std::chrono::seconds::period>(now - app->last_time).count();
			app->total_time += app->delta_time;


			app->average_delta_time = app->average_delta_time + ((app->delta_time - app->average_delta_time) / app->times_count);
			app->fps = 1 / app->delta_time;
			app->average_fps = app->average_fps + ((app->fps - app->average_fps) / app->times_count);
			app->times_count++;

			app->delta_time_list.push_back(app->delta_time);
			app->last_time = now;

			(update_state == UPDATE_STOP) ? state = MAIN_FINISH : 0;
			break;
		}
		case MAIN_FINISH:
			Scene* ref = app->get_scene();
			LOG("Cleaning up application...");
			ASSERT(app->clean_up(), "Error when cleaning up the application!");
			state = MAIN_EXIT;
			break;
		}
	}
#ifdef _WIN64
	std::string platform = "x64";
#elif _WIN32
	std::string platform = "x86";
#endif
	std::string name = std::string(platform + "output_" + std::to_string(MAX_OBJECTS) + "_objects_in_" + std::to_string((int)MAX_TIME)+".txt");

	std::cout << "\nAverage update time: " << app->average_delta_time << " seconds" << std::endl;
	std::cout << "Average updates per second: " << app->average_fps  << std::endl;
	//std::cout << "Total Created: " << app->total_created << std::endl;
	//std::cout << "Total Destroyed: " << app->total_destroyed << std::endl;
	std::cout << std::to_string(app->delta_time_list.size()) << " update times data in total." << std::endl;
	std::cout << "Saving update times in file " << name << "..." << std::endl;

	std::ofstream outfile(name);
	for (unsigned long long i = 0; i < app->delta_time_list.size(); ++i)
	{
		outfile << app->delta_time_list[i] << std::endl;
	}

	outfile.close();
	app->delta_time_list.clear();

	std::cout << "Data saved." << std::endl;
	std::cout << "\nPress any key to close..." << std::endl;
	char a = getchar();
	// Release app data
	RELEASE(app);

	LOG("Program sucessfully ended!\n");
	return 0;
}