#include "guis/GuiOptionWindow.h"
#include "Window.h"

GuiOptionWindow::GuiOptionWindow(Window* window, const std::string& title) 
	: GuiComponent(window)
	, mMenu(window, title.c_str())
{

	addChild(&mMenu);

	m_backButton = mMenu.addButton("BACK", "go back", [ this ] { delete this; });

	setSize(( float ) Renderer::getScreenWidth(), ( float ) Renderer::getScreenHeight());
	mMenu.setPosition(( mSize.x() - mMenu.getSize().x() ) / 2, Renderer::getScreenHeight() * 0.15f);
}

std::shared_ptr<ButtonComponent> GuiOptionWindow::addButton(
	const std::string& label, 
	const std::string& helpText, 
	const std::function<void()>& callback)
{
	std::shared_ptr<ButtonComponent> button = mMenu.addButton(label, helpText, callback);
	OnButtonAdded(button);
	return button;
}

GuiOptionWindow::~GuiOptionWindow()
{

}

bool GuiOptionWindow::input(InputConfig* config, Input input)
{
	if (config->isMappedTo("b", input) && input.value != 0)
	{
		delete this;
		return true;
	}

	if (config->isMappedTo("select", input) && 
		input.value != 0)
	{
		// close everything
		Window* window = mWindow;
		while (window->peekGui() && window->peekGui()->isPersistent() == false)
			delete window->peekGui();
		return true;
	}

	return GuiComponent::input(config, input);
}

std::vector<HelpPrompt> GuiOptionWindow::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts = mMenu.getHelpPrompts();

	prompts.push_back(HelpPrompt("b", "back"));
	prompts.push_back(HelpPrompt("select", "close"));

	return prompts;
}