#ifndef IMGUIHANDLER_H
#define IMGUIHANDLER_H

class ImGuiHandler
{
public:
	ImGuiHandler();
	~ImGuiHandler();
	void init();
	void newFrame();
	void render();
	void shutdown();
};



#endif
