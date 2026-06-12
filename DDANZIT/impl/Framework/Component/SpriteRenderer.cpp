#include "SpriteRenderer.h"

#include "Debug.h"

#include "GameObject.h"


#pragma region Constructor

SpriteRenderer::SpriteRenderer(GameObject* pGameObject) : 
	Component(pGameObject, "SpriteRenderer"), sprite(SpriteIndex::None), color(Color(1.0f, 1.0f, 1.0f, 1.0f)), flipX(false), flipY(false), useAtlas(false), currentAtlas(SpriteAtlasRect{0,0,0,0})
{

}

#pragma endregion



#pragma region Clone

Component* SpriteRenderer::Clone() const
{
	return new SpriteRenderer(*this);
}

#pragma endregion



#pragma region Properties

#pragma endregion



#pragma region Methods

bool SpriteRenderer::TryAddAtlasRect(const std::string& name, SpriteAtlasRect sliceRect)
{
	auto result = atlasRectMap.insert({ name, sliceRect });

	if (result.second /*== true*/)
		return true;

	Debug::Log("TryAddAtlasRect: 이미 존재하는 이름입니다.");
	return false;
}

bool SpriteRenderer::TryAddAtlasRect(const std::string& name, int offsetX, int offsetY, int width, int height)
{
	auto result = atlasRectMap.insert({ name, SpriteAtlasRect{ offsetX, offsetY, width, height } });

	if (result.second /*== true*/)
		return true;

	Debug::Log("TryAddAtlasRect: 이미 존재하는 이름입니다.");
	return false;
}


bool SpriteRenderer::TrySetAtlas(const std::string& name)
{
	auto result = atlasRectMap.find(name);

	if (result == atlasRectMap.end())
	{
		Debug::Log("TrySetAtlas: 존재하지 않는 이름입니다.");
		return false;
	}

	currentAtlas = result->second;

	return true;
}

#pragma endregion

