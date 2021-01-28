#pragma once

#ifdef SYR_PLATFORM_WINDOWS

extern SYR::Application* SYR::CreateApplication();

int main(int argc, char** argv) {

	SYR::Log::init();

	auto app = SYR::CreateApplication();
	app->run();
	delete app;
}
#endif