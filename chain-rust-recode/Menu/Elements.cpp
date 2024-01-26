#include <filesystem>
#include "Elements.h"
#include "Framework/GUI.h"
#include "../stdafx.hpp"
#include "../Config.hpp"

namespace Menu
{

	void DrawMenu ( )
	{
		if ( GUI::Form::BeginWindow ( _ ( "phantom.tech" ) ) || GUI::ctx->setup )
		{
			if ( GUI::Form::BeginTab ( 0, _ ( "A" ) ) || GUI::ctx->setup )
			{
				GUI::Group::BeginGroup ( _ ( "Aimbot" ), Vector2 ( 50, 60 ) );
				{
					GUI::Controls::Checkbox ( _ ( "Enabled##rage" ), &g_Vars.aimbot.enabled );
					GUI::Controls::Hotkey ( _ ( "Enabled key##rage" ), &g_Vars.aimbot.aim_key );
					GUI::Controls::Dropdown ( _ ( "Hitbox" ) + std::string ( _ ( "#" ) ), {
						_ ( "Head" ),
						_ ( "Neck" ),
						_ ( "Chest" ),
						_ ( "Closest to crosshair" ) },
						&g_Vars.aimbot.hitbox );

					GUI::Controls::Slider ( _ ( "Field of view" ), &g_Vars.aimbot.aim_fov, 0.f, 200.f, _ ( "%.0f°" ) );
					GUI::Controls::Slider ( _ ( "Smoothing" ), &g_Vars.aimbot.smoothing, 0, 10 );
					GUI::Controls::Checkbox ( _ ( "Terrain check" ), &g_Vars.aimbot.terraincheck );
				}
				GUI::Group::EndGroup ( );

				GUI::Group::BeginGroup ( _ ( "Special" ), Vector2 ( 50, 40 ) );
				{
					GUI::Controls::Checkbox ( _ ( "Silent aim" ), &g_Vars.aimbot.silent );
					GUI::Controls::Checkbox ( _ ( "Team check" ), &g_Vars.aimbot.ignore_friends );
				}
				GUI::Group::EndGroup ( );

				GUI::Group::BeginGroup ( _ ( "Accuracy" ), Vector2 ( 50, 50 ) );
				{
					GUI::Controls::Checkbox ( _ ( "Big bullet" ), &g_Vars.misc.thickness );
					GUI::Controls::Slider ( _ ( "Big bullet scale" ), &g_Vars.misc.flthickness, 0.f, 1.0f );

					GUI::Controls::Checkbox ( _ ( "Remove scope sway" ), &g_Vars.misc.nosway );
					GUI::Controls::Checkbox ( _ ( "Remove spread" ), &g_Vars.misc.nospread );
					GUI::Controls::Checkbox ( _ ( "Remove shotgun spread" ), &g_Vars.misc.noshotgunspread );
					GUI::Controls::Checkbox ( _ ( "Shoot while midair" ), &g_Vars.misc.jumpshoot );

					GUI::Controls::Slider ( _ ( "Recoil percentage x" ), &g_Vars.misc.recoil_pitch, 0.f, 100.f, _ ( "%.0f%%" ) );
					GUI::Controls::Slider ( _ ( "Recoil percentage y" ), &g_Vars.misc.recoil_yaw, 0.f, 100.f, _ ( "%.0f%%" ) );

				}
				GUI::Group::EndGroup ( );

				GUI::Group::BeginGroup ( _ ( "Other" ), Vector2 ( 50, 50 ) );
				{
					GUI::Controls::Checkbox ( _ ( "Rapid fire" ), &g_Vars.misc.rapidfire, true );
					GUI::Controls::Slider ( _ ( "Rapid fire speed" ), &g_Vars.misc.rapidfire_speed, 0.f, 0.1f );
					GUI::Controls::Checkbox ( _ ( "Insta eoka" ), &g_Vars.misc.eoka );
					GUI::Controls::Checkbox ( _ ( "Insta compound bow" ), &g_Vars.misc.compound );
					GUI::Controls::Checkbox ( _ ( "Fast bow" ), &g_Vars.misc.fastbow, true );

					GUI::Controls::Checkbox ( _ ( "Force automatic" ), &g_Vars.misc.force_automatic );
					GUI::Controls::Checkbox ( _ ( "Force semi automatic" ), &g_Vars.misc.force_semiautomatic );
				}
				GUI::Group::EndGroup ( );
			}

			if ( GUI::Form::BeginTab ( 1, _ ( "C" ) ) || GUI::ctx->setup )
			{

				GUI::Group::BeginGroup ( _ ( "General" ), Vector2 ( 50, 60 ) );
				{
					GUI::Controls::Checkbox ( _ ( "Enabled##esp" ), &g_Vars.esp.enabled );

					if ( GUI::Controls::Checkbox ( _ ( "Offscreen" ), &g_Vars.esp.offscren_enabled ) || GUI::ctx->setup )
					{
						GUI::Controls::ColorPicker ( _ ( "Offscreen color" ), &g_Vars.esp.offscreen_color );
						GUI::Controls::Slider ( _ ( "Offscreen distance" ), &g_Vars.esp.offscren_distance, 10, 100.f, _ ( "%.0f%%" ) );
						//GUI::Controls::Slider ( _ ( "Offscreen size" ), &g_Vars.esp.offscren_size, 4, 16.f, _ ( "%.0fpx" ) );
					}

					GUI::Controls::Checkbox ( _ ( "Chams" ), &g_Vars.esp.chams );

					GUI::Controls::Checkbox ( _ ( "Box" ), &g_Vars.esp.box );
					GUI::Controls::ColorPicker ( _ ( "Color##box" ), &g_Vars.esp.box_color );

					GUI::Controls::Checkbox ( _ ( "Name" ), &g_Vars.esp.name );
					GUI::Controls::ColorPicker ( _ ( "Color##name" ), &g_Vars.esp.name_color );

					GUI::Controls::Checkbox ( _ ( "Health" ), &g_Vars.esp.health );
					GUI::Controls::Checkbox ( _ ( "Health color override" ), &g_Vars.esp.health_override );
					GUI::Controls::ColorPicker ( _ ( "Health color override color##Health" ), &g_Vars.esp.health_color );

					GUI::Controls::Checkbox ( _ ( "Skeleton" ), &g_Vars.esp.skeleton );
					GUI::Controls::ColorPicker ( _ ( "Color##Skeleton" ), &g_Vars.esp.skeleton_color );

					//GUI::Controls::Checkbox ( _ ( "Ammo" ), &g_Vars.esp.ammo_bar );
					//GUI::Controls::ColorPicker ( _ ( "Ammo color##color" ), &g_Vars.esp.ammo_bar_color );

					GUI::Controls::Checkbox ( _ ( "Distance" ), &g_Vars.esp.distance );
					GUI::Controls::ColorPicker ( _ ( "Color##Distance" ), &g_Vars.esp.distance_color );

					GUI::Controls::Checkbox ( _ ( "Weapon" ), &g_Vars.esp.weapon );
					GUI::Controls::ColorPicker ( _ ( "Color##Weapon" ), &g_Vars.esp.weapon_color );

					GUI::Controls::Checkbox ( _ ( "Snaplines" ), &g_Vars.esp.snaplines );
					GUI::Controls::ColorPicker ( _ ( "Color##Snaplines" ), &g_Vars.esp.snaplines_color );

					GUI::Controls::Checkbox ( _ ( "Npc" ), &g_Vars.esp.npc );
					GUI::Controls::Checkbox ( _ ( "Tunnel dweller" ), &g_Vars.esp.tunneldweller );

					GUI::Controls::Checkbox ( _ ( "Sleepers" ), &g_Vars.esp.sleeper );
					GUI::Controls::ColorPicker ( _ ( "Color##Sleepers" ), &g_Vars.esp.sleeper_color );
					GUI::Controls::Checkbox ( _ ( "Teammates" ), &g_Vars.esp.teammate );

					GUI::Controls::Checkbox ( _ ( "Highlight aimbot target" ), &g_Vars.esp.highlight_target );
					GUI::Controls::ColorPicker ( _ ( "Color##Highlight" ), &g_Vars.esp.highlight_target_color );

					GUI::Controls::Checkbox ( _ ( "Show corpses" ), &g_Vars.esp.item_corpse );
					GUI::Controls::ColorPicker ( _ ( "Color##Corpses" ), &g_Vars.esp.item_corpse_color );

					GUI::Controls::Checkbox ( _ ( "Show dropped weapons" ), &g_Vars.esp.item_weapon );
					GUI::Controls::ColorPicker ( _ ( "Color##DroppedWeapons" ), &g_Vars.esp.item_weapon_color );

					GUI::Controls::Checkbox ( _ ( "Show dropped items" ), &g_Vars.esp.item_other );
					GUI::Controls::ColorPicker ( _ ( "Color##DroppedItems" ), &g_Vars.esp.item_other_color );

					GUI::Controls::Checkbox ( _ ( "Animal" ), &g_Vars.esp.animal );
					GUI::Controls::ColorPicker ( _ ( "Color##Animal" ), &g_Vars.esp.animals_color );

					GUI::Group::EndGroup ( );
				}

				GUI::Group::BeginGroup ( _ ( "Other ESP" ), Vector2 ( 50, 40 ) );
				{
					GUI::Controls::Slider ( _ ( "Distance limit" ), &g_Vars.esp.distance_limit, 0.f, 4000.f );

					GUI::Controls::Checkbox ( _ ( "Hide esp on inventory" ), &g_Vars.esp.hide );

					GUI::Controls::Checkbox ( _ ( "Hotbar esp" ), &g_Vars.esp.item_info );

					GUI::Controls::Checkbox ( _ ( "Raid" ), &g_Vars.esp.raid );
					GUI::Controls::ColorPicker ( _ ( "Color##Raid" ), &g_Vars.esp.raid_color );

					GUI::Controls::Checkbox ( _ ( "Tool cupboard" ), &g_Vars.esp.toolcupboard );
					GUI::Controls::ColorPicker ( _ ( "Color##Toolcupboard" ), &g_Vars.esp.toolcupboard_color );

					GUI::Group::EndGroup ( );
				}

				GUI::Group::BeginGroup ( _ ( "World environment" ), Vector2 ( 50, 70 ) );
				{
					GUI::Controls::Checkbox ( _ ( "Stash" ), &g_Vars.esp.stash );
					GUI::Controls::ColorPicker ( _ ( "Color##Stash" ), &g_Vars.esp.stash_color );

					if ( GUI::Controls::Checkbox ( _ ( "Ores" ), &g_Vars.esp.ores ) )
					{
						GUI::Controls::Checkbox ( _ ( "Stone ore" ), &g_Vars.esp.stone );
						GUI::Controls::ColorPicker ( _ ( "Color##Stone" ), &g_Vars.esp.stone_color );

						GUI::Controls::Checkbox ( _ ( "Sulfur ore" ), &g_Vars.esp.sulfur );
						GUI::Controls::ColorPicker ( _ ( "Color##Sulfur" ), &g_Vars.esp.sulfur_color );

						GUI::Controls::Checkbox ( _ ( "Metal ore" ), &g_Vars.esp.metal );
						GUI::Controls::ColorPicker ( _ ( "Color##Metal" ), &g_Vars.esp.metal_color );
					}

					if ( GUI::Controls::Checkbox ( _ ( "Vehicles" ), &g_Vars.esp.vehicles ) )
					{
						GUI::Controls::Checkbox ( _ ( "Horse" ), &g_Vars.esp.vehicles_horse );
						GUI::Controls::ColorPicker ( _ ( "Color##Horse" ), &g_Vars.esp.vehicles_horse_color );

						GUI::Controls::Checkbox ( _ ( "Boats" ), &g_Vars.esp.vehicles_boats );
						GUI::Controls::ColorPicker ( _ ( "Color##Boats" ), &g_Vars.esp.vehicles_boats_color );

						GUI::Controls::Checkbox ( _ ( "Helicopters" ), &g_Vars.esp.vehicles_helicopters );
						GUI::Controls::ColorPicker ( _ ( "Color##Helicopters" ), &g_Vars.esp.vehicles_helicopters_color );

						GUI::Controls::Checkbox ( _ ( "Special" ), &g_Vars.esp.special );
						GUI::Controls::ColorPicker ( _ ( "Color##Special" ), &g_Vars.esp.special_color );

					}

					if ( GUI::Controls::Checkbox ( _ ( "Traps" ), &g_Vars.esp.traps ) )
					{
						GUI::Controls::Checkbox ( _ ( "Auto turret" ), &g_Vars.esp.autoturret );
						GUI::Controls::ColorPicker ( _ ( "Color##Autoturret" ), &g_Vars.esp.autoturret_color );

						GUI::Controls::Checkbox ( _ ( "Flame turret" ), &g_Vars.esp.flameturret );
						GUI::Controls::ColorPicker ( _ ( "Color##Flameturret" ), &g_Vars.esp.flameturret_color );

						GUI::Controls::Checkbox ( _ ( "Shotgun trap" ), &g_Vars.esp.shotguntrap );
						GUI::Controls::ColorPicker ( _ ( "Color##Shotguntrap" ), &g_Vars.esp.shotguntrap_color );

						GUI::Controls::Checkbox ( _ ( "Land mine" ), &g_Vars.esp.landmine );
						GUI::Controls::ColorPicker ( _ ( "Color##Landmine" ), &g_Vars.esp.landmine_color );

						GUI::Controls::Checkbox ( _ ( "Bear trap" ), &g_Vars.esp.beartrap );
						GUI::Controls::ColorPicker ( _ ( "Color##Beartrap" ), &g_Vars.esp.beartrap_color );

						GUI::Controls::Checkbox ( _ ( "Sam site" ), &g_Vars.esp.samsite );
						GUI::Controls::ColorPicker ( _ ( "Color##Samsite" ), &g_Vars.esp.samsite_color );
					}

					if ( GUI::Controls::Checkbox ( _ ( "Collectables" ), &g_Vars.esp.collectable ) )
					{
						GUI::Controls::Checkbox ( _ ( "Hemp" ), &g_Vars.esp.hemp );
						GUI::Controls::ColorPicker ( _ ( "Color##Hemp" ), &g_Vars.esp.hemp_color );

						GUI::Controls::Checkbox ( _ ( "Mushroom" ), &g_Vars.esp.mushroom );
						GUI::Controls::ColorPicker ( _ ( "Color##Mushroom" ), &g_Vars.esp.mushroom_color );

						GUI::Controls::Checkbox ( _ ( "Corn" ), &g_Vars.esp.corn );
						GUI::Controls::ColorPicker ( _ ( "Color##Corn" ), &g_Vars.esp.corn_color );

						GUI::Controls::Checkbox ( _ ( "Berry" ), &g_Vars.esp.berry );
						GUI::Controls::ColorPicker ( _ ( "Color##Berry" ), &g_Vars.esp.berry_color );

						GUI::Controls::Checkbox ( _ ( "Potato" ), &g_Vars.esp.potato );
						GUI::Controls::ColorPicker ( _ ( "Color##Potato" ), &g_Vars.esp.potato_color );

						GUI::Controls::Checkbox ( _ ( "Stone" ), &g_Vars.esp.cstone );
						GUI::Controls::ColorPicker ( _ ( "Color##Stonee" ), &g_Vars.esp.cstone_color );

						GUI::Controls::Checkbox ( _ ( "Sulfur" ), &g_Vars.esp.csulfur );
						GUI::Controls::ColorPicker ( _ ( "Color##Sulfurr" ), &g_Vars.esp.csulfur_color );

						GUI::Controls::Checkbox ( _ ( "Metal" ), &g_Vars.esp.cmetal );
						GUI::Controls::ColorPicker ( _ ( "Color##Metall" ), &g_Vars.esp.cmetal_color );

						GUI::Controls::Checkbox ( _ ( "Wood" ), &g_Vars.esp.cwood );
						GUI::Controls::ColorPicker ( _ ( "Color##Wood" ), &g_Vars.esp.cwood_color );
					}

					if ( GUI::Controls::Checkbox ( _ ( "Radtown" ), &g_Vars.esp.radtown ) )
					{
						GUI::Controls::Checkbox ( _ ( "Crate" ), &g_Vars.esp.crate );
						GUI::Controls::ColorPicker ( _ ( "Color##Crate" ), &g_Vars.esp.crate_color );

						GUI::Controls::Checkbox ( _ ( "Elite crate" ), &g_Vars.esp.ecrate );
						GUI::Controls::ColorPicker ( _ ( "Color##Elitecrate" ), &g_Vars.esp.ecrate_color );

						GUI::Controls::Checkbox ( _ ( "Military crate" ), &g_Vars.esp.mcrate );
						GUI::Controls::ColorPicker ( _ ( "Color##Militarycrate" ), &g_Vars.esp.mcrate_color );

						GUI::Controls::Checkbox ( _ ( "Barrel" ), &g_Vars.esp.barrel );
						GUI::Controls::ColorPicker ( _ ( "Color##Barrel" ), &g_Vars.esp.barrel_color );

						GUI::Controls::Checkbox ( _ ( "Toolbox" ), &g_Vars.esp.toolbox );
						GUI::Controls::ColorPicker ( _ ( "Color##Toolbox" ), &g_Vars.esp.toolbox_color );

						GUI::Controls::Checkbox ( _ ( "Oil barrel" ), &g_Vars.esp.oil );
						GUI::Controls::ColorPicker ( _ ( "Color##Oilbarrel" ), &g_Vars.esp.oil_color );

						GUI::Controls::Checkbox ( _ ( "Minecart" ), &g_Vars.esp.minecart );
						GUI::Controls::ColorPicker ( _ ( "Color##Minecart" ), &g_Vars.esp.minecart_color );
					}

					GUI::Group::EndGroup ( );
				}

				GUI::Group::BeginGroup ( _ ( "Effects" ), Vector2 ( 50, 30 ) );
				{
					GUI::Controls::Checkbox ( _ ( "Always day" ), &g_Vars.misc.alwaysday );
					GUI::Controls::Checkbox ( _ ( "Always night" ), &g_Vars.misc.alwaysnight );

					GUI::Controls::Checkbox ( _ ( "Full bright" ), &g_Vars.misc.fullbright );

					GUI::Controls::Checkbox ( _ ( "Zoom on scope" ), &g_Vars.misc.zoom_on_scope );

					GUI::Controls::Slider ( _ ( "Field of view##effects" ), &g_Vars.misc.fov, 10.f, 180.f );

					GUI::Group::EndGroup ( );
				}

			}

			if ( GUI::Form::BeginTab ( 2, _ ( "D" ) ) || GUI::ctx->setup )
			{
				GUI::Group::BeginGroup ( _ ( "General" ), Vector2 ( 50, 70 ) );
				{
					GUI::Controls::Checkbox ( _ ( "Admin flag" ), &g_Vars.misc.admin, true );

					//GUI::Controls::Checkbox ( _ ( "Fast loot" ), &g_Vars.misc.fastloot );
					GUI::Controls::Checkbox ( _ ( "Spiderman" ), &g_Vars.misc.spider );

					GUI::Controls::Checkbox ( _ ( "Climb assist" ), &g_Vars.misc.bclimb );
					GUI::Controls::Checkbox ( _ ( "Infinite jump" ), &g_Vars.misc.infinite_jump, true );

					GUI::Controls::Checkbox ( _ ( "Noclip" ), &g_Vars.misc.bfly );
					GUI::Controls::Hotkey ( _ ( "Noclip key##misc" ), &g_Vars.misc.fly );
					GUI::Controls::Slider ( _ ( "Noclip speed" ), &g_Vars.misc.noclip_speed, 10.f, 100.f );
					GUI::Controls::Checkbox ( _ ( "Noclip indicator" ), &g_Vars.misc.flyhack_indicator );

					GUI::Controls::Checkbox ( _ ( "Kill localplayer" ), &g_Vars.misc.kill_localplayer );
					GUI::Controls::Hotkey ( _ ( "Kill localplayer key##misc" ), &g_Vars.misc.kill_localplayer_key );

					//GUI::Controls::Checkbox ( _ ( "Omni sprint" ), &g_Vars.misc.omnisprint, true );
					GUI::Controls::Checkbox ( _ ( "Hitsound" ), &g_Vars.misc.hitsound );

					if ( g_Vars.misc.hitsound || GUI::ctx->setup )
					{
						const auto GetScripts = [] ( )
						{
							std::string dir = GetDocumentsDirectory ( ).append ( _ ( "\\phantom.tech\\sounds\\" ) );
							for ( auto& file_path : std::filesystem::directory_iterator ( dir ) )
							{
								if ( !file_path.path ( ).string ( ).empty ( ) )
								{
									if ( file_path.path ( ).string ( ).find ( _ ( ".wav" ) ) != std::string::npos )
									{
										g_Vars.global.m_hitsounds.emplace_back ( file_path.path ( ).string ( ).erase ( 0, dir.length ( ) ) );
									}
								}
							}
						};

						if ( g_Vars.global.m_hitsounds.empty ( ) )
						{
							GetScripts ( );
						}

						if ( g_Vars.global.m_hitsounds.empty ( ) )
						{
							GUI::Controls::Dropdown ( _ ( "Custom hitsounds" ), { _ ( "phantom.tech\\sounds\\*.wav" ) }, &g_Vars.misc.hitsound_custom );
						}
						else
						{
							GUI::Controls::Dropdown ( _ ( "Custom hitsounds" ), g_Vars.global.m_hitsounds, &g_Vars.misc.hitsound_custom );
							GUI::Controls::Slider ( _ ( "Sound volume" ), &g_Vars.misc.hitsound_volume, 1.f, 100.f, _ ( "%.0f%%" ) );
						}

						GUI::Controls::Button ( _ ( "Refresh sounds" ), [&] ( ) -> void
							{
								g_Vars.global.m_hitsounds.clear ( );

								GetScripts ( );
							} );
					}

					if ( GUI::Controls::Checkbox ( _ ( "Long neck" ), &g_Vars.misc.bview_offset ) )
					{
						GUI::Controls::Hotkey ( _ ( "Long neck key##misc" ), &g_Vars.misc.view_offset );

						GUI::Controls::Slider ( _ ( "Long neck" ), &g_Vars.misc.y, -1.4f, 1.4f );
					}

					GUI::Controls::Checkbox ( _ ( "Shoot while mounted" ), &g_Vars.misc.shoot_from_minicopter );

					GUI::Group::EndGroup ( );
				}

				GUI::Group::BeginGroup ( _ ( "Cheat settings" ), Vector2 ( 50, 30 ) );
				{
					GUI::Controls::Label ( _ ( "Menu theme##menu" ) );
					GUI::Controls::ColorPicker ( _ ( "Color##menu" ), &g_Vars.menu.menu_color );

					GUI::Controls::Button ( _ ( "Reset menu layout" ), [&] ( )
						{
							GUI::ctx->size = { 648, 548 };
							g_Vars.menu.menu_color = FloatColor ( 0.647058824f, 0.57254902f, 0.952941176f, 1.0f );
						} );

					GUI::Group::EndGroup ( );
				}

				GUI::Group::BeginGroup ( _ ( "Configs" ), Vector2 ( 50, 100 ) );
				{
					static int selected_cfg;
					static std::vector<std::string> cfg_list;
					static bool initialise_configs = true;
					bool reinit = false;
					if ( initialise_configs || ( GetTickCount ( ) % 1000 ) == 0 )
					{
						cfg_list = ConfigManager::GetConfigs ( );
						initialise_configs = false;
						reinit = true;
					}

					static std::string config_name;
					GUI::Controls::Textbox ( _ ( "Config name" ), &config_name, 26 );
					GUI::Controls::Listbox ( _ ( "Config selection" ),
						( cfg_list.empty ( ) ? std::vector<std::string>{_ ( "No configs" )} : cfg_list ), &selected_cfg, false, 6 );

					if ( reinit )
					{
						if ( selected_cfg >= cfg_list.size ( ) )
							selected_cfg = cfg_list.size ( ) - 1;

						if ( selected_cfg < 0 )
							selected_cfg = 0;
					}

					if ( !cfg_list.empty ( ) )
					{
						static bool confirm_save = false;
						GUI::Controls::Button ( !confirm_save ? _ ( "Save config#save" ) : _ ( "Are you sure?#save" ), [&] ( )
							{
								if ( selected_cfg <= cfg_list.size ( ) && selected_cfg >= 0 )
								{
									if ( !confirm_save )
									{
										confirm_save = true;
										return;
									}

									if ( confirm_save )
									{
										ConfigManager::SaveConfig ( cfg_list.at ( selected_cfg ) );

										confirm_save = false;

										GUI::ctx->SliderInfo.LastChangeTime.clear ( );
										GUI::ctx->SliderInfo.PreviewAnimation.clear ( );
										GUI::ctx->SliderInfo.PreviousAmount.clear ( );
										GUI::ctx->SliderInfo.ShouldChangeValue.clear ( );
										GUI::ctx->SliderInfo.ValueTimer.clear ( );
										GUI::ctx->SliderInfo.ValueAnimation.clear ( );
									}
								}
							}, true );

						GUI::Controls::Button ( _ ( "Load config" ), [&] ( )
							{
								if ( selected_cfg <= cfg_list.size ( ) && selected_cfg >= 0 )
								{
									ConfigManager::LoadConfig ( cfg_list.at ( selected_cfg ) );
								}
							} );

						static bool confirm_delete = false;
						GUI::Controls::Button ( !confirm_delete ? _ ( "Delete config#delete" ) : _ ( "Are you sure?#delete" ), [&] ( )
							{
								if ( selected_cfg <= cfg_list.size ( ) && selected_cfg >= 0 )
								{
									if ( !confirm_delete )
									{
										confirm_delete = true;
										return;
									}

									if ( confirm_delete )
									{
										ConfigManager::RemoveConfig ( cfg_list.at ( selected_cfg ) );
										cfg_list = ConfigManager::GetConfigs ( );
										confirm_delete = false;
									}
								}
							}, true );
					}

					GUI::Controls::Button ( _ ( "Create config" ), [&] ( )
						{
							if ( config_name.empty ( ) )
								return;

							ConfigManager::CreateConfig ( config_name );
							ConfigManager::SaveConfig ( config_name );
							cfg_list = ConfigManager::GetConfigs ( );
						} );

					GUI::Controls::Button ( _ ( "Unload cheat" ), [&] ( )
						{
							LI_FN ( exit )( 0 );
						} );

					GUI::Group::EndGroup ( );
				}
			}

			GUI::Form::EndWindow ( );
		}
	}

	void Draw ( )
	{
		DrawMenu ( );
	}
}