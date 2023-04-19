#pragma once

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

	// Release app data
	RELEASE(app);

	LOG("Program sucessfully ended!\n");
	return 0;
}