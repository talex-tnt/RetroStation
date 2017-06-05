#include "guis/GuiRetroArchConfig.h"
#include "Window.h"
#include "views/gamelist/IGameListView.h"
#include "views/ViewController.h"
#include "CfgFile.h"
#include "guis/GuiMsgBox.h"
#include "GuiImportRetroArchConfig.h"
#include "SystemData.h"

GuiRetroArchConfig::GuiRetroArchConfig(
	Window* window,
	std::string title,
	SystemData& system,
	std::unique_ptr<CfgFile> config)
	: GuiOptionWindow(window, title),
	mSystem(system),
	m_config(std::move(config))
{
	ComponentListRow row;
	bool configFileExists = m_config->ConfigFileExists();
	if (configFileExists)
	{
		std::string title = "EDIT CONFIG";
		row.elements.clear();
		row.addElement(std::make_shared<TextComponent>(mWindow, title, Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
		row.input_handler = [ this, title ] (InputConfig* config, Input input)
		{
			if (config->isMappedTo("a", input) && input.value)
			{
				//auto s = new GuiImportRetroArchConfig(mWindow, title);
				//mWindow->pushGui(s);
				return true;
			}
			return false;
		};
		addRow(row);
		{
			std::string title = "DELETE CONFIG";
			row.elements.clear();
			row.addElement(std::make_shared<TextComponent>(mWindow, title, Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
			row.input_handler = [ this, title ] (InputConfig* config, Input input)
			{
				if (config->isMappedTo("a", input) && input.value)
				{
					mWindow->pushGui(new GuiMsgBox(mWindow, "Do you really want to Delete " + m_config->GetConfigFilePath() + "?", "YES",
						[ this ]
					{
						m_config->DeleteConfigFile();
						if (m_config->ConfigFileExists())
						{
							mWindow->pushGui(new GuiMsgBox(mWindow, "Could not Delete " + m_config->GetConfigFilePath() + "?", "Close",
								[ this ]
							{
							}, nullptr));
						}
						else
						{
							delete this;
						}

					}, "NO", nullptr));
					return true;
				}
				return false;
			};
			addRow(row);
		}
	}
	else
	{
		std::string title = "CREATE CONFIG";
		row.elements.clear();
		row.addElement(std::make_shared<TextComponent>(mWindow, title, Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
		row.input_handler = [ this, title ] (InputConfig* config, Input input)
		{
			if (config->isMappedTo("a", input) && input.value)
			{
				bool writeFile = true; \
					// THIS IS WRONG: 
					//if (m_config->ConfigFileExists())
					//{
					//	mWindow->pushGui(new GuiMsgBox(mWindow, "Do you really want to Overwrite " + m_config->GetConfigFilePath() + "?", "YES",
					//		[ this, &writeFile ]
					//	{
					//		writeFile = true;
					//	}, "NO", nullptr));
					//}
					if (writeFile)
					{
						m_config->SaveConfigFile();
						if (!m_config->ConfigFileExists())
						{
							mWindow->pushGui(new GuiMsgBox(mWindow, "Could not Save " + m_config->GetConfigFilePath() + "?", "Close",
								[ this ]
							{
							}, nullptr));
						}
						else
						{
							delete this;
						}
					}
				return true;
			}
			return false;
		};
		addRow(row);
	}

	{
		std::string title = "IMPORT CONFIG";
		row.elements.clear();
		row.addElement(std::make_shared<TextComponent>(mWindow, title, Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
		row.input_handler = [ this, title ] (InputConfig* config, Input input)
		{
			if (config->isMappedTo("a", input) && input.value)
			{
				boost::filesystem::path configFolder =  mSystem.getRetroArchConfigImportFolder();
				if (boost::filesystem::exists(configFolder))
				{
					auto s = new GuiImportRetroArchConfig(mWindow, title, configFolder,
						std::bind(&GuiRetroArchConfig::OnImportConfigSelected, this, std::placeholders::_1)
					);
					mWindow->pushGui(s);
					return true;
				}
			else
			{
				mWindow->pushGui(new GuiMsgBox(mWindow,
					"Configuration folder not found: " + configFolder.generic_string(),
					"Close", [ this ] { delete this; }));
				return true;
				}
			}
			return false;
		};
		addRow(row);
	}
}

GuiRetroArchConfig::~GuiRetroArchConfig()
{

}

IGameListView* GuiRetroArchConfig::getGamelist()
{
	return ViewController::get()->getGameListView(&mSystem).get();
}

void GuiRetroArchConfig::OnImportConfigSelected(boost::filesystem::path configPath)
{
	mWindow->pushGui(new GuiMsgBox(mWindow, "Do you really want to Overwrite " + m_config->GetConfigFilePath() + " with the content of " + configPath.generic_string() + "?", "YES",
		[ this, configPath ]
	{
		std::unique_ptr<CfgFile> new_config(new CfgFile());
		if (LoadConfigFile(new_config, configPath))
		{
			const std::string originalPath = m_config->GetConfigFilePath();
			if (DeleteConfigFile(m_config))
			{
				if (SaveConfigFile(new_config, originalPath))
				{
					m_config = std::move(new_config);
				}
			}
		}
		delete this;
	}, "NO", nullptr));
}

void GuiRetroArchConfig::ShowError(std::string mgs)
{
	mWindow->pushGui(new GuiMsgBox(mWindow, mgs, "Close", [] { }));
}

bool GuiRetroArchConfig::LoadConfigFile(std::unique_ptr<CfgFile>& config, boost::filesystem::path configPath)
{
	const bool result = config->LoadConfigFile(configPath.generic_string());
	if (!result)
	{
		ShowError("Could not load " + configPath.generic_string());
	}
	return result;
}


bool GuiRetroArchConfig::SaveConfigFile(std::unique_ptr<CfgFile>& config, boost::filesystem::path configPath)
{
	const bool result = config->SaveConfigFile(configPath.generic_string());
	if (!result)
	{
		ShowError("Could not save " + configPath.generic_string());
	}
	return result;
}


bool GuiRetroArchConfig::DeleteConfigFile(std::unique_ptr<CfgFile>& config)
{
	if (config->ConfigFileExists())
	{
		const bool result = config->DeleteConfigFile();
		if (!result)
		{
			ShowError("Could not delete " + config->GetConfigFilePath());
		}
		return result;
	}
	else
	{
		return true;
	}
}