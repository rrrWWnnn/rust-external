#include "../gui.h"
#include "../../../stdafx.hpp"
#include <iomanip>

#pragma warning(disable : 4018)

bool TitleBarHovered;
std::string FormName;

void retard ( )
{
	if ( GUI::ctx->FocusedID != 0 )
		GUI::ctx->FocusedID = 0;

	if ( GUI::ctx->DropdownInfo.HashedID != 0 )
		GUI::ctx->DropdownInfo.HashedID = 0;

	if ( GUI::ctx->ColorPickerInfo.HashedID != 0 )
		GUI::ctx->ColorPickerInfo.HashedID = 0;

	if ( GUI::ctx->MultiDropdownInfo.HashedID != 0 )
		GUI::ctx->MultiDropdownInfo.HashedID = 0;

	if ( GUI::ctx->dragging )
		GUI::ctx->dragging = false;

	if ( GUI::ctx->typing )
		GUI::ctx->typing = false;
}

bool GUI::Form::BeginWindow ( std::string name )
{

	static bool bReset = true;
	if ( ctx->animation <= 0.0f )
	{
		if ( bReset )
		{
			retard ( );
			bReset = false;
		}

		return false;
	}

	bReset = true;

	if ( ctx->FocusedID == 0 )
		ctx->typing = false;

	TitleBarHovered = InputHelper::IsHoveringOverPoint ( ctx->pos, Vector2 ( ctx->size.x, 25 ) );
	bool bResizeHovered = InputHelper::IsHoveringOverPoint ( ( ctx->pos + Vector2 ( 0, 20 ) ) + ctx->size - 15, Vector2 ( 15, 15 ) );

	if ( ctx->FocusedID == 0 )
	{
		static Vector2 prev_mouse_pos;
		static Vector2 mouse_delta;

		mouse_delta = prev_mouse_pos - InputHelper::GetMousePos ( );

		if ( !ctx->dragging_scrollbar )
		{
			if ( !ctx->dragging && InputHelper::Down ( VK_LBUTTON ) && TitleBarHovered )
				ctx->dragging = true;
			else if ( ctx->dragging && InputHelper::Down ( VK_LBUTTON ) )
				ctx->pos -= mouse_delta;
			else if ( ctx->dragging && !InputHelper::Down ( VK_LBUTTON ) )
				ctx->dragging = false;

			if ( !ctx->resizing && InputHelper::Down ( VK_LBUTTON ) && bResizeHovered )
				ctx->resizing = true;
			else if ( ctx->resizing && InputHelper::Down ( VK_LBUTTON ) )
			{
				bool check1 = false;
				bool check2 = false;

				if ( ( ctx->size.x - mouse_delta.x ) > 648 )
				{
					ctx->size.x -= mouse_delta.x;
				}
				else { check1 = true; }

				if ( ( ctx->size.y - mouse_delta.y ) > 548 )
				{
					ctx->size.y -= mouse_delta.y;
				}
				else { check2 = true; }

				if ( check1 && check2 )
				{
					if ( !bResizeHovered )
						ctx->resizing = false;
				}
			}
			else if ( ctx->resizing && !InputHelper::Down ( VK_LBUTTON ) )
				ctx->resizing = false;
		}

		prev_mouse_pos = InputHelper::GetMousePos ( );
	}

	// let's grab screensize, this is not calling a game func so this should never cause issues.
	const auto screen = Render::GetScreenSize ( );

	// let's clamp
	ctx->size.x = std::clamp<int> ( ctx->size.x, 648, screen.x );
	ctx->size.y = std::clamp<int> ( ctx->size.y, 538, screen.y );

	ctx->pos.x = std::clamp<int> ( ctx->pos.x, 0, screen.x - ctx->size.x );
	ctx->pos.y = std::clamp<int> ( ctx->pos.y, 0, screen.y - ctx->size.y );

	// swap cursor pos stack
	std::stack< Vector2 > ( ).swap ( ctx->CursorPosStack );

	// start menu alpha animation
	alpha_mod = ctx->animation;

	static Color menu_outline [6] = {
		Color ( 60, 60, 60 ),
		Color ( 40, 40, 40 ),
		Color ( 40, 40, 40 ),
		Color ( 40, 40, 40 ),
		Color ( 60, 60, 60 ),
		Color ( 10, 10, 10 )
	};

	for ( int i = 1; i < 7; ++i )
	{
		Render::DirectX::rect ( ctx->pos - i, ( ctx->size - 1 ) + ( i * 2 ), menu_outline [i - 1].OverrideAlpha ( 255 ) );
	}

	D3DVIEWPORT9 tab_inner = { ctx->pos.x, ctx->pos.y, ctx->size.x, ctx->size.y };
	D3DVIEWPORT9 old_viewport = Render::DirectX::get_viewport ( );

	Render::DirectX::set_viewport ( tab_inner );
	Render::DirectX::rect_fill ( Vector2 ( ctx->pos.x + 75, ctx->pos.y ), ctx->size, Color ( 20, 20, 20 ) );
	Render::DirectX::Textures::background->Draw ( Vector2 ( ctx->pos.x + 74, ctx->pos.y - 4 ), Color ( 255, 255, 255 ) );
	Render::DirectX::set_viewport ( old_viewport );

	auto rainbow = [] ( Vector2 pos, bool dark )
	{
		Render::DirectX::gradient_h ( pos + 1, Vector2 ( ctx->size.x / 2, 1 ), Color ( 55, 177, 218 ), Color ( 202, 70, 205 ) );
		Render::DirectX::gradient_h ( Vector2 ( pos.x + ctx->size.x / 2, pos.y + 1 ), Vector2 ( ( ctx->size.x / 2 ) - 1, 1 ), Color ( 202, 70, 205 ), Color ( 204, 227, 53 ) );

		if ( dark )
		{
			Render::DirectX::rect_fill ( pos + 1, Vector2 ( ctx->size.x - 1, 1 ), Color ( 0, 0, 0, 100 ) );
			Render::DirectX::rect_fill ( pos + Vector2 ( 1, 2 ), Vector2 ( ctx->size.x - 1, 1 ), Color ( 0, 0, 0, 100 ) );
		}
	};

	// top side bar stuff
	Render::DirectX::rect_fill ( ctx->pos, Vector2 ( 75, 4 ), Color ( 20, 20, 20 ) );
	Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 0, 4 ), Vector2 ( 73, ctx->ActiveTab == 0 ? 20 : 23 ), Color ( 12, 12, 12 ) );
	Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 73, 4 ), Vector2 ( 1, 22 ), Color ( 0, 0, 0 ) );
	Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 74, 4 ), Vector2 ( 1, 22 ), Color ( 48, 48, 48 ) );

	rainbow ( ctx->pos, false );
	rainbow ( ctx->pos + Vector2 ( 0, 1 ), true );

	for ( int i = 0; i < ctx->Tabs.size ( ); ++i )
	{
		Vector2 tab_size = Render::DirectX::Fonts::menu_icon.measure_size ( ctx->Tabs [i] );
		Vector2 tab_pos = ctx->pos + Vector2 ( 0, 23 + ( 76 * i ) );

		if ( ctx->Tabs [i].find ( 'C' ) != std::string::npos )
		{
			//tab_pos.x -= 4;
		}

		if ( ctx->ActiveTab != i )
		{
			bool cut_bottom = ctx->ActiveTab == i + 1;

			Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 0, 27 + ( 76 * i ) ), Vector2 ( 73, cut_bottom ? 73 : 76 ), Color ( 12, 12, 12 ) );
			Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 73, 26 + ( 76 * i ) ), Vector2 ( 1, 76 ), Color ( 0, 0, 0 ) );
			Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 74, 26 + ( 76 * i ) ), Vector2 ( 1, 76 ), Color ( 48, 48, 48 ) );

			Render::DirectX::Fonts::menu_icon.draw_text ( ctx->Tabs [i], tab_pos + Vector2 ( 38, 38 ) - Vector2 ( tab_size.x / 2, tab_size.y / 2 ), Color ( 90, 90, 90 ), font_dropshadow );
		}

		if ( ctx->ActiveTab == i )
		{
			D3DVIEWPORT9 tab_inner2 = { ctx->pos.x, ctx->pos.y + 26 + ( 76 * i ), 75.f, 76.f };
			Render::DirectX::set_viewport ( tab_inner2 );
			Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 0, 25 + ( 76 * i ) ), Vector2 ( 75, 76 ), Color ( 20, 20, 20 ) );
			Render::DirectX::Textures::background->Draw ( ctx->pos + Vector2 ( 0, 22 + ( 76 * i ) ), Color ( 255, 255, 255 ) );
			Render::DirectX::set_viewport ( old_viewport );

			Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 0, 24 + ( 76 * i ) ), Vector2 ( 73, 1 ), Color ( 0, 0, 0 ) );
			Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 0, 25 + ( 76 * i ) ), Vector2 ( 75, 1 ), Color ( 48, 48, 48 ) );

			Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 0, 101 + ( 76 * i ) ), Vector2 ( 75, 1 ), Color ( 48, 48, 48 ) );
			Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 0, 102 + ( 76 * i ) ), Vector2 ( 73, 1 ), Color ( 0, 0, 0 ) );

			Render::DirectX::Fonts::menu_icon.draw_text ( ctx->Tabs [i], tab_pos + Vector2 ( 38, 38 ) - Vector2 ( tab_size.x / 2, tab_size.y / 2 ), Color ( 210, 210, 210 ), font_dropshadow );
		}
		else
		{
			if ( InputHelper::IsHoveringOverPoint ( tab_pos, Vector2 ( 75, 77 ) ) )
			{
				Render::DirectX::Fonts::menu_icon.draw_text ( ctx->Tabs [i], tab_pos + Vector2 ( 38, 38 ) - Vector2 ( tab_size.x / 2, tab_size.y / 2 ), Color ( 150, 150, 150 ), font_dropshadow );

				if ( InputHelper::Pressed ( VK_LBUTTON ) )
					ctx->ActiveTab = i;
			}
		}
	}

	// bottom side bar stuff
	float size_delta = fabs ( ( ctx->pos + Vector2 ( 73, 26 + ( 76 * ctx->Tabs.size ( ) ) ) ).y - ( ctx->pos.y + ctx->size.y ) );
	Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 0, 27 + ( 76 * ctx->Tabs.size ( ) ) ), Vector2 ( 73, size_delta - 1 ), Color ( 12, 12, 12 ) );
	Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 73, 26 + ( 76 * ctx->Tabs.size ( ) ) ), Vector2 ( 1, size_delta ), Color ( 0, 0, 0 ) );
	Render::DirectX::rect_fill ( ctx->pos + Vector2 ( 74, 26 + ( 76 * ctx->Tabs.size ( ) ) ), Vector2 ( 1, size_delta ), Color ( 48, 48, 48 ) );

	ctx->Tabs.clear ( );

	// at what (x, y) relative to the menu's initial cursor position (0,0) we should draw the menu elements
	GUI::PushCursorPos ( Vector2 ( 104, 24 ) );

	return true;
}

void GUI::Form::EndWindow ( )
{
	if ( ctx->DropdownInfo.HashedID != 0 )
	{
		Vector2 pos = ctx->DropdownInfo.Pos;
		int width = ctx->DropdownInfo.Size;

		for ( int i = 0; i < ctx->DropdownInfo.Elements.size ( ); i++ )
		{
			Vector2 option_pos = pos + Vector2 ( 0, 20 * i );
			Vector2 option_size = Vector2 ( width, 20 );

			if ( InputHelper::IsHoveringOverPoint ( option_pos, option_size ) )
			{
				Render::DirectX::rect_fill ( option_pos, option_size, { 25, 25, 25 } );
				if ( InputHelper::Pressed ( VK_LBUTTON ) )
				{
					*ctx->DropdownInfo.Option = i;
					ctx->DropdownInfo.HashedID = 0;
					ctx->FocusedID = 0;
				}
			}
			else
			{
				Render::DirectX::rect_fill ( option_pos, option_size, { 35, 35, 35 } );
			}

			if ( *ctx->DropdownInfo.Option != i )
				Render::DirectX::Fonts::menu.draw_text ( ctx->DropdownInfo.Elements [i], option_pos + Vector2 ( 10, 4 ), Color ( 200, 200, 200 ), 0 );
			else
				Render::DirectX::Fonts::menu_bold.draw_text ( ctx->DropdownInfo.Elements [i], option_pos + Vector2 ( 10, 4 ), g_Vars.menu.menu_color.ToRegularColor ( ), 0 );
		}

		Render::DirectX::rect ( ctx->DropdownInfo.Pos, Vector2 ( ctx->DropdownInfo.Size, 20 * ctx->DropdownInfo.Elements.size ( ) ), { 10, 10, 10 } );

		if ( InputHelper::Pressed ( VK_LBUTTON ) )
		{
			ctx->DropdownInfo.HashedID = 0;
			ctx->FocusedID = 0;
		}
	}

	if ( ctx->MultiDropdownInfo.HashedID != 0 )
	{
		Vector2 pos = ctx->MultiDropdownInfo.Pos;
		int width = ctx->MultiDropdownInfo.Size;

		for ( int i = 0; i < ctx->MultiDropdownInfo.Elements.size ( ); i++ )
		{
			Vector2 option_pos = pos + Vector2 ( 0, 20 * i );
			Vector2 option_size = Vector2 ( width, 20 );

			if ( InputHelper::IsHoveringOverPoint ( option_pos, option_size ) )
			{
				Render::DirectX::rect_fill ( option_pos, option_size, { 25, 25, 25 } );
				if ( InputHelper::Pressed ( VK_LBUTTON ) )
				{
					*ctx->MultiDropdownInfo.Elements.at ( i ).value ^= 1;
				}
			}
			else
			{
				Render::DirectX::rect_fill ( option_pos, option_size, { 35, 35, 35 } );
			}

			if ( !*ctx->MultiDropdownInfo.Elements.at ( i ).value )
				Render::DirectX::Fonts::menu.draw_text ( ctx->MultiDropdownInfo.Elements.at ( i ).name, option_pos + Vector2 ( 10, 4 ), Color ( 200, 200, 200 ), 0 );
			else
				Render::DirectX::Fonts::menu_bold.draw_text ( ctx->MultiDropdownInfo.Elements.at ( i ).name, option_pos + Vector2 ( 10, 4 ), g_Vars.menu.menu_color.ToRegularColor ( ), 0 );
		}

		Render::DirectX::rect ( pos, Vector2 ( ctx->MultiDropdownInfo.Size, 20 * ctx->MultiDropdownInfo.Elements.size ( ) ), { 10, 10, 10 } );

		if ( InputHelper::Pressed ( VK_LBUTTON ) && !InputHelper::IsHoveringOverPoint ( pos, Vector2 ( ctx->MultiDropdownInfo.Size, 20 * ctx->MultiDropdownInfo.Elements.size ( ) ) ) )
		{
			ctx->MultiDropdownInfo.HashedID = 0;
			ctx->FocusedID = 0;
		}
	}

	static float ColorPickerHue = -1.f;
	static std::map<size_t, float> ColorPickerAlpha;
	if ( ctx->ColorPickerInfo.HashedID != 0 && ctx->animation >= 1.0f )
	{
		size_t id = ctx->ColorPickerInfo.HashedID;
		// context menu, copy, paste
		if ( ctx->ColorPickerInfo.ActionCringe == 1 )
		{
			Vector2 DrawPos = Vector2 ( ( int ) ctx->ColorPickerInfo.Pos.x, ( int ) ctx->ColorPickerInfo.Pos.y );
			Vector2 PastePos = DrawPos + Vector2 ( 0, 20 );

			// copy
			if ( InputHelper::IsHoveringOverPoint ( DrawPos, Vector2 ( 85, 20 ) ) )
			{
				Render::DirectX::rect_fill ( DrawPos, Vector2 ( 85, 20 ), { 25, 25, 25 } );
				if ( InputHelper::Pressed ( VK_LBUTTON ) )
				{
					std::stringstream color_hex;

					color_hex << "#";
					color_hex << std::hex << std::setw ( 2 ) << std::setfill ( '0' ) << std::uppercase << +( *ctx->ColorPickerInfo.Value ).ToRegularColor ( ).r ( );
					color_hex << std::hex << std::setw ( 2 ) << std::setfill ( '0' ) << std::uppercase << +( *ctx->ColorPickerInfo.Value ).ToRegularColor ( ).g ( );
					color_hex << std::hex << std::setw ( 2 ) << std::setfill ( '0' ) << std::uppercase << +( *ctx->ColorPickerInfo.Value ).ToRegularColor ( ).b ( );
					color_hex << std::hex << std::setw ( 2 ) << std::setfill ( '0' ) << std::uppercase << +( *ctx->ColorPickerInfo.Value ).ToRegularColor ( ).a ( );

					if ( OpenClipboard ( nullptr ) )
					{
						EmptyClipboard ( );
						HGLOBAL clipboard_buffer = GlobalAlloc ( GMEM_DDESHARE, color_hex.str ( ).size ( ) + 1 );
						char* buffer = ( char* ) GlobalLock ( clipboard_buffer );
						strcpy ( buffer, color_hex.str ( ).c_str ( ) );

						GlobalUnlock ( clipboard_buffer );
						SetClipboardData ( CF_TEXT, clipboard_buffer );
						CloseClipboard ( );
					}

					ctx->FocusedID = ctx->ColorPickerInfo.HashedID = 0;
				}
			}
			else
			{
				Render::DirectX::rect_fill ( DrawPos, Vector2 ( 85, 20 ), { 35, 35, 35 } );
			}

			// paste
			if ( InputHelper::IsHoveringOverPoint ( PastePos, Vector2 ( 85, 20 ) ) )
			{
				Render::DirectX::rect_fill ( PastePos, Vector2 ( 85, 20 ), { 25, 25, 25 } );
				if ( InputHelper::Pressed ( VK_LBUTTON ) )
				{
					ColorPickerHue = -1.f;

					// bruh this better be the right format...
					if ( IsClipboardFormatAvailable ( CF_TEXT ) )
					{
						if ( OpenClipboard ( nullptr ) )
						{
							std::string input ( ( char* ) GetClipboardData ( CF_TEXT ) );
							if ( !input.empty ( ) )
							{
								bool bIsValidHexColor = input.at ( 0 ) == '#';

								for ( int i = 0; i < input.length ( ); ++i )
								{
									auto n = input [i];

									if ( n == 'g' || n == 'h' || n == 'i' ||
										n == 'j' || n == 'k' || n == 'l' ||
										n == 'm' || n == 'n' || n == 'o' ||
										n == 'p' || n == 'q' || n == 'r' ||
										n == 's' || n == 't' || n == 'u' ||
										n == 'v' || n == 'w' || n == 'x' ||
										n == 'y' || n == 'z' || n == 'G' ||
										n == 'H' || n == 'I' || n == 'J' ||
										n == 'K' || n == 'L' || n == 'M' ||
										n == 'N' || n == 'O' || n == 'P' ||
										n == 'Q' || n == 'R' || n == 'S' ||
										n == 'T' || n == 'U' || n == 'V' ||
										n == 'W' || n == 'X' || n == 'Y' ||
										n == 'Z' || n == ' ' )
									{
										bIsValidHexColor = false;
									}
								}


								if ( input.length ( ) > 9 )
									bIsValidHexColor = false;

								//	check if we have a hex color copied in our clipboard
								if ( bIsValidHexColor )
								{
									int component_r = std::stoi ( input.substr ( 1, 2 ), 0, 16 );
									int component_g = std::stoi ( input.substr ( 3, 2 ), 0, 16 );
									int component_b = std::stoi ( input.substr ( 5, 2 ), 0, 16 );
									int component_a = input.size ( ) > 7 ? std::stoi ( input.substr ( 7, 2 ), 0, 16 ) : 255;

									Color colPaste = Color ( component_r, component_g, component_b, component_a );

									( *ctx->ColorPickerInfo.Value ).SetColor ( colPaste );
								}
							}
							CloseClipboard ( );
						}
					}

					ctx->FocusedID = ctx->ColorPickerInfo.HashedID = 0;
				}
			}
			else
			{
				Render::DirectX::rect_fill ( PastePos, Vector2 ( 85, 20 ), { 35, 35, 35 } );
			}

			Render::DirectX::Fonts::menu.draw_text ( _ ( "Copy" ), DrawPos + Vector2 ( 10, 4 ), Color ( 200, 200, 200 ), 0 );
			Render::DirectX::Fonts::menu.draw_text ( _ ( "Paste" ), PastePos + Vector2 ( 10, 4 ), Color ( 200, 200, 200 ), 0 );

			Render::DirectX::rect ( DrawPos, Vector2 ( 85, 40 ), { 10, 10, 10 } );

			if ( InputHelper::Pressed ( VK_LBUTTON ) )
			{
				ctx->FocusedID = ctx->ColorPickerInfo.HashedID = 0;
			}

		}
		// regular picker
		else
		{
			if ( ColorPickerAlpha.find ( ctx->ColorPickerInfo.HashedID ) == ColorPickerAlpha.end ( ) )
			{
				ColorPickerAlpha.insert ( { ctx->ColorPickerInfo.HashedID, 0.f } );
			}

			Vector2 DrawPos = Vector2 ( ( int ) ctx->ColorPickerInfo.Pos.x, ( int ) ctx->ColorPickerInfo.Pos.y );

			Color::Hsv_t v1 = Color::RGBtoHSV ( ( *ctx->ColorPickerInfo.Value ).ToRegularColor ( ) );

			float NewHue;

			if ( ColorPickerHue == -1.f )
			{
				NewHue = v1.Hue;
				ColorPickerHue = NewHue;
			}
			else
			{
				NewHue = ColorPickerHue;
			}

			float NewSaturation = v1.Saturation;
			float NewValue = v1.Value;
			ColorPickerAlpha.at ( ctx->ColorPickerInfo.HashedID ) = ( ( *ctx->ColorPickerInfo.Value ).ToRegularColor ( ) ).a ( );

			int MaxHeight = ( ctx->ColorPickerInfo.Size );
			Vector2 DrawSize = Vector2 ( ctx->ColorPickerInfo.Size, MaxHeight );

			Render::DirectX::rect_fill ( DrawPos, DrawSize, Color ( 40, 40, 40, 255 ) );
			Render::DirectX::rect ( DrawPos, DrawSize, Color ( 60, 60, 60, 255 ) );
			Render::DirectX::rect ( DrawPos - 1, DrawSize + 2, Color ( 10, 10, 10, 255 ) );

			Vector2 ColorDrawPos ( DrawPos + 5 );
			Vector2 ColorDrawSize ( Vector2 ( DrawSize.x - 30, DrawSize.y - 25 ) );

			Vector2 AlphaDrawPos ( ColorDrawPos + Vector2 ( 0, ColorDrawSize.y + 4 ) );
			Vector2 AlphaDrawSize = Vector2 ( ColorDrawSize.x, 11 );

			Vector2 ContextDrawPos ( AlphaDrawPos + Vector2 ( 0, AlphaDrawSize.y + 4 ) );
			Vector2 ContextDrawSize ( AlphaDrawSize );

			Vector2 HueDrawPos ( ColorDrawPos + Vector2 ( ColorDrawSize.x + 4, -1 ) );
			Vector2 HueDrawSize = Vector2 ( 16, ColorDrawSize.y );

			static Vector2 ColorPreviewPos;

			bool HoveredMain = InputHelper::IsHoveringOverPoint ( DrawPos, DrawSize );
			bool HoveredPicker = InputHelper::IsHoveringOverPoint ( ColorDrawPos, ColorDrawSize );
			bool HoveredAlpha = InputHelper::IsHoveringOverPoint ( AlphaDrawPos, AlphaDrawSize );
			bool HoveredHue = InputHelper::IsHoveringOverPoint ( HueDrawPos, HueDrawSize );

			// alpha bar
			if ( !ctx->ColorPickerInfo.PickingAlpha && !ctx->ColorPickerInfo.PickingHue && !ctx->ColorPickerInfo.PickingColor && !ctx->ColorPickerInfo.CopyingColor && !ctx->ColorPickerInfo.PastingColor )
			{
				if ( HoveredAlpha && InputHelper::Pressed ( VK_LBUTTON ) )
				{
					ctx->ColorPickerInfo.PickingAlpha = true;
				}
			}
			else if ( ctx->ColorPickerInfo.PickingAlpha )
			{
				if ( InputHelper::Down ( VK_LBUTTON ) )
				{
					float FinalValue = std::clamp<float> ( GUI::MapNumber ( std::clamp<float> (
						Vector2 ( InputHelper::GetMousePos ( ) - AlphaDrawPos ).x, 0, AlphaDrawSize.x ),
						0, AlphaDrawSize.x, 0, 255 ), 0, 255 );

					ColorPickerAlpha.at ( ctx->ColorPickerInfo.HashedID ) = FinalValue;
				}
				else
				{
					ctx->ColorPickerInfo.PickingAlpha = false;
				}
			}

			Render::DirectX::rect_fill ( AlphaDrawPos, AlphaDrawSize, ( ( *ctx->ColorPickerInfo.Value ).ToRegularColor ( ) ).OverrideAlpha ( 255, true ) );
			Render::DirectX::rect ( AlphaDrawPos, AlphaDrawSize, Color ( 10, 10, 10, 255 ) );

			// hue bar
			if ( !ctx->ColorPickerInfo.PickingHue && !ctx->ColorPickerInfo.PickingAlpha && !ctx->ColorPickerInfo.PickingColor && !ctx->ColorPickerInfo.CopyingColor && !ctx->ColorPickerInfo.PastingColor )
			{
				if ( HoveredHue && InputHelper::Pressed ( VK_LBUTTON ) )
				{
					ctx->ColorPickerInfo.PickingHue = true;
				}
			}
			else if ( ctx->ColorPickerInfo.PickingHue )
			{
				if ( InputHelper::Down ( VK_LBUTTON ) )
				{
					NewHue = ( ( InputHelper::GetMousePos ( ).y - ( HueDrawPos.y ) ) / HueDrawSize.y ) * 360.f;

					NewHue = std::clamp<float> ( NewHue, 0.f, 359.f );
					ColorPickerHue = NewHue;
				}
				else
				{
					ctx->ColorPickerInfo.PickingHue = false;
				}
			}

			for ( int i = 0; i < HueDrawSize.y; i++ )
			{
				float Hue = ( ( float ) i / HueDrawSize.y ) * 360.f;
				Color HueColor = Color::HSVtoRGB ( Hue, 1, 1 );

				Render::DirectX::line ( HueDrawPos + Vector2 ( 0, i + 1 ), HueDrawPos + Vector2 ( HueDrawSize.x, i + 1 ), HueColor.OverrideAlpha ( 255 ) );
			}

			Render::DirectX::rect ( HueDrawPos + Vector2 ( 0, 1 ), HueDrawSize, Color ( 10, 10, 10, 255 ) );

			// main picker
			if ( !ctx->ColorPickerInfo.PickingColor && !ctx->ColorPickerInfo.PickingHue && !ctx->ColorPickerInfo.PickingAlpha && !ctx->ColorPickerInfo.CopyingColor && !ctx->ColorPickerInfo.PastingColor )
			{
				if ( HoveredPicker && InputHelper::Pressed ( VK_LBUTTON ) )
				{
					ctx->ColorPickerInfo.PickingColor = true;
				}
			}
			else if ( ctx->ColorPickerInfo.PickingColor )
			{
				if ( InputHelper::Down ( VK_LBUTTON ) )
				{
					Vector2 CursorDelta = InputHelper::GetMousePos ( ) - ColorDrawPos;

					float s = ( float ) CursorDelta.x / ( ColorDrawSize.x );
					float v = 1.f - ( float ) CursorDelta.y / ( ColorDrawSize.y );

					// xd
					ColorPreviewPos = InputHelper::GetMousePos ( );

					NewSaturation = s;
					NewValue = v;
				}
				else
				{
					ctx->ColorPickerInfo.PickingColor = false;
				}
			}

			( *ctx->ColorPickerInfo.Value ).SetColor ( Color::HSVtoRGB ( NewHue, std::clamp<float> ( NewSaturation, 0.0f, 1.0f ), std::clamp<float> ( NewValue, 0.0f, 1.0f ) )
				.OverrideAlpha ( ColorPickerAlpha.at ( ctx->ColorPickerInfo.HashedID ) ) );

			Render::DirectX::gradient_h ( ColorDrawPos, ColorDrawSize + Vector2 ( 0, 1 ), Color ( 255, 255, 255 ),
				Color::HSVtoRGB ( NewHue, 1, 1 ).OverrideAlpha ( 255 ) );

			Render::DirectX::gradient_v ( ColorDrawPos, ColorDrawSize + 1, Color ( 0, 0, 0, 0 ), Color ( 0, 0, 0, 255 ) );
			Render::DirectX::rect ( ColorDrawPos, ColorDrawSize, Color ( 10, 10, 10, 255 ) );

			// the rects that signify what each part of the colorpicker is at

			// alpha
			float AlphaAdditive = ( AlphaDrawSize.x * ( ColorPickerAlpha.at ( ctx->ColorPickerInfo.HashedID ) / 255.f ) );

			Render::DirectX::rect_fill ( AlphaDrawPos + Vector2 ( ( AlphaAdditive - 3 ) < 0 ? 0 : AlphaAdditive - 3, 1 ), Vector2 ( 3, AlphaDrawSize.y - 2 ), Color ( 255, 255, 255, 180 ) );
			Render::DirectX::rect ( AlphaDrawPos + Vector2 ( ( AlphaAdditive - 3 ) < 0 ? 0 : AlphaAdditive - 3, 1 ), Vector2 ( 3, AlphaDrawSize.y - 2 ), Color ( 10, 10, 10, 255 ) );

			// actual color
			auto viewport = Render::DirectX::get_viewport ( );
			D3DVIEWPORT9 color_limit = { ColorDrawPos.x, ColorDrawPos.y, ColorDrawSize.x, ColorDrawSize.y, 0.f, 1.0f };
			Render::DirectX::set_viewport ( color_limit );
			{
				Render::DirectX::rect_fill ( ColorPreviewPos, Vector2 ( 3, 3 ), Color ( 255, 255, 255, 180 ) );
				Render::DirectX::rect ( ColorPreviewPos, Vector2 ( 3, 3 ), Color ( 10, 10, 10, 255 ) );
			}
			Render::DirectX::set_viewport ( viewport );

			// hue
			float HueAdditive = ( HueDrawSize.y * ( NewHue / 360.f ) );

			Render::DirectX::rect_fill ( HueDrawPos + Vector2 ( 1, ( HueAdditive - 3 ) < 0 ? 1 : HueAdditive - 2 ), Vector2 ( HueDrawSize.x - 2, 3 ), Color ( 255, 255, 255, 180 ) );
			Render::DirectX::rect ( HueDrawPos + Vector2 ( 1, ( HueAdditive - 3 ) < 0 ? 1 : HueAdditive - 2 ), Vector2 ( HueDrawSize.x - 2, 3 ), Color ( 10, 10, 10, 255 ) );

			if ( InputHelper::Pressed ( VK_LBUTTON ) && !HoveredMain )
			{
				ctx->ColorPickerInfo.HashedID = ctx->FocusedID = 0;
			}
		}
	}
	else
	{
		ColorPickerHue = -1.f;
	}

	// end menu alpha animation
	alpha_mod = -1.f;
}

bool GUI::Form::BeginTab ( int icon, std::string name )
{
	ctx->Tabs.push_back ( name.data ( ) );

	ctx->CurrentTab = name;
	return ( ctx->ActiveTab == ctx->Tabs.size ( ) - 1 ) || ctx->setup;
}