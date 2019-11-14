#include "SanskritSlovary.h"

GUI_APP_MAIN
{
	StdLogSetup( LOG_FILE | LOG_COUT );
	SanskritSlovaryWindow WindowSanskritSlovary;
	WindowSanskritSlovary.Run();
}
