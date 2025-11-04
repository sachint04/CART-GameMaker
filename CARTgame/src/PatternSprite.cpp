#include "PatternSprite.h"
#include "AssetManager.h"
#include "World.h"
#include "Logger.h"
namespace cart
{
#pragma region Constructor & Init

                                
	PatternSprite::PatternSprite(World* _owningworld, const std::string& _id, int patternIndex, std::vector<Rectangle>& patterndata)
        :Sprite2D{ _owningworld, _id }, m_patternIndex{ patternIndex }, m_recPattern{ patterndata }
	{
	}
	void PatternSprite::Init()
	{
        UIElement::Init();
	}
   
#pragma endregion

#pragma region Loop


	
	void PatternSprite::Draw(float _deltaTime)
	{
        // Draw BG pattern
        if (m_strTexture.size() > 0) {
            Vector2 loc = GetLocation();
            Rectangle screen = {loc.x,loc.y, m_width, m_height};
            m_texture2d = AssetManager::Get().LoadTextureAsset(m_strTexture, m_textureStatus);
            DrawTextureTiled(m_recPattern.at(m_patternIndex), screen, { 0,0 }, 0, 1.f, m_textureColor);
          /*  DrawTextureTiled(texPattern, recPattern[activePattern], { (float)OPT_WIDTH + MARGIN_SIZE, (float)MARGIN_SIZE, GetScreenWidth() - OPT_WIDTH - 2.0f * MARGIN_SIZE, GetScreenHeight() - 2.0f * MARGIN_SIZE },
                (Vector2) {
                0.0f, 0.0f
            }, rotation, scale, colors[activeCol]);*/
        }
        

        //--------------------------------------------
	}
#pragma endregion
    void PatternSprite::DrawTextureTiled(Rectangle source, Rectangle dest, Vector2 origin, float rotation, float scale, Color tint)
    {

       /* void DrawTextureTiled(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, float scale, Color tint)
        {*/
            if ((m_texture2d.get()->id <= 0) || (scale <= 0.0f)) return;  // Wanna see a infinite loop?!...just delete this line!
            if ((source.width == 0) || (source.height == 0)) return;

            int tileWidth = (int)(source.width * scale), tileHeight = (int)(source.height * scale);
            if ((dest.width < tileWidth) && (dest.height < tileHeight))
            {
                // Can fit only one tile
                DrawTexturePro(*m_texture2d,{ source.x, source.y, ((float)dest.width / tileWidth)* source.width, ((float)dest.height / tileHeight)* source.height },
                    {
                    dest.x, dest.y, dest.width, dest.height
                }, origin, rotation, tint);
            }
            else if (dest.width <= tileWidth)
            {
                // Tiled vertically (one column)
                int dy = 0;
                for (; dy + tileHeight < dest.height; dy += tileHeight)
                {
                    DrawTexturePro(*m_texture2d, { source.x, source.y, ((float)dest.width / tileWidth)* source.width, source.height }, { dest.x, dest.y + dy, dest.width, (float)tileHeight }, origin, rotation, tint);
                }

                // Fit last tile
                if (dy < dest.height)
                {
                    DrawTexturePro(*m_texture2d, { source.x, source.y, ((float)dest.width / tileWidth)* source.width, ((float)(dest.height - dy) / tileHeight)* source.height },
                        {
                        dest.x, dest.y + dy, dest.width, dest.height - dy
                    }, origin, rotation, tint);
                }
            }
            else if (dest.height <= tileHeight)
            {
                // Tiled horizontally (one row)
                int dx = 0;
                for (; dx + tileWidth < dest.width; dx += tileWidth)
                {
                    DrawTexturePro(*m_texture2d, { source.x, source.y, source.width, ((float)dest.height / tileHeight)* source.height }, { dest.x + dx, dest.y, (float)tileWidth, dest.height }, origin, rotation, tint);
                }

                // Fit last tile
                if (dx < dest.width)
                {
                    DrawTexturePro(*m_texture2d, { source.x, source.y, ((float)(dest.width - dx) / tileWidth)* source.width, ((float)dest.height / tileHeight)* source.height },
                        {
                        dest.x + dx, dest.y, dest.width - dx, dest.height
                    }, origin, rotation, tint);
                }
            }
            else
            {
                // Tiled both horizontally and vertically (rows and columns)
                int dx = 0;
                for (; dx + tileWidth < dest.width; dx += tileWidth)
                {
                    int dy = 0;
                    for (; dy + tileHeight < dest.height; dy += tileHeight)
                    {
                        DrawTexturePro(*m_texture2d, source, { dest.x + dx, dest.y + dy, (float)tileWidth, (float)tileHeight }, origin, rotation, tint);
                    }

                    if (dy < dest.height)
                    {
                        DrawTexturePro(*m_texture2d, { source.x, source.y, source.width, ((float)(dest.height - dy) / tileHeight)* source.height },
                            {
                            dest.x + dx, dest.y + dy, (float)tileWidth, dest.height - dy
                        }, origin, rotation, tint);
                    }
                }

                // Fit last column of tiles
                if (dx < dest.width)
                {
                    int dy = 0;
                    for (; dy + tileHeight < dest.height; dy += tileHeight)
                    {
                        DrawTexturePro(*m_texture2d, { source.x, source.y, ((float)(dest.width - dx) / tileWidth)* source.width, source.height },
                            {
                            dest.x + dx, dest.y + dy, dest.width - dx, (float)tileHeight
                        }, origin, rotation, tint);
                    }

                    // Draw final tile in the bottom right corner
                    if (dy < dest.height)
                    {
                        DrawTexturePro(*m_texture2d, { source.x, source.y, ((float)(dest.width - dx) / tileWidth)* source.width, ((float)(dest.height - dy) / tileHeight)* source.height },
                            {
                            dest.x + dx, dest.y + dy, dest.width - dx, dest.height - dy
                        }, origin, rotation, tint);
                    }
                }
            }
       // }
    }
#pragma region Helper

#pragma endregion

#pragma region  CleanUp


	PatternSprite::~PatternSprite()
	{
	}

#pragma endregion
}