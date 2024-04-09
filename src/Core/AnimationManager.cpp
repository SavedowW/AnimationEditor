#include "Application.h"
#include "AnimationManager.h"
#include <cassert>

AnimationManager::AnimationManager(Renderer* renderer_, const std::string &rootPath_) :
	m_renderer(renderer_),
	m_rootPath(rootPath_ + "/")
{
	auto sprPath = rootPath_ + "/Resources/Sprites/";
	std::filesystem::path basePath(sprPath);
	std::cout << "=== LISTING FOUND ANIMATIONS ===\n";
	for (const auto &entry : std::filesystem::recursive_directory_iterator(sprPath))
	{
		std::filesystem::path dirpath = entry.path();
		std::filesystem::path filepath(entry.path().string() + "/animinfo.txt");
		if (entry.is_directory() && std::filesystem::exists(filepath))
		{
			ContainedAnimationData cad;
			cad.m_path = dirpath.string();
			m_textureArrs.push_back(cad);
			auto idstring = utils::getRelativePath(sprPath, dirpath.string());
			m_ids[idstring] = m_textureArrs.size() - 1;
			std::cout << dirpath.filename() << std::endl;
		}
	}
	std::cout << "=== LISTING ENDS HERE ===\n";
}

std::shared_ptr<TextureArr> AnimationManager::getTextureArr(int id_)
{
	if (m_textureArrs[id_].m_preloaded)
	{
		//Logger::print("Texture arr is preloaded\n");
		return m_textureArrs[id_].m_preloaded;
	}
	else if (m_textureArrs[id_].m_texArr.expired())
	{
		//Logger::print("Texture arr does not exist, creating new\n");

		//Parse data.txt
		std::ifstream loadData(m_textureArrs[id_].m_path + "/animinfo.txt");
		std::string prefix;
		int ibegin, iend;
		int w = 0;
		int h = 0;
		int ver = 1;
		std::string temp;
		loadData >> temp;
		if (temp == "V.2.0")
		{
			ver = 2;
			loadData >> w;
			loadData >> h;
			loadData >> prefix;
		}
		else
		{
			prefix = temp;
		}
		loadData >> ibegin >> iend;
		int amount = iend - ibegin + 1;

		//Load all textures
		SDL_Texture** texs = new SDL_Texture * [amount];
		for (int i = ibegin; i <= iend; ++i)
		{
			texs[i - ibegin] = m_renderer->loadTexture((m_textureArrs[id_].m_path + "/" + prefix + std::to_string(i) + ".png").c_str());
		}

		int totalDuration = 0;
		loadData >> totalDuration;
		std::vector<int> framesData;
		for (int i = 0; i < totalDuration; ++i)
		{
			int temp;
			loadData >> temp;
			framesData.push_back(temp);
		}

		//Create TextureArray with loaded textures
		if (ver == 2)
		{
			std::shared_ptr<TextureArr> reqElem(new TextureArr(texs, amount, totalDuration, framesData, w, h));
			m_textureArrs[id_].m_texArr = reqElem;
			return reqElem;
		}
		else
		{
			std::shared_ptr<TextureArr> reqElem(new TextureArr(texs, amount, totalDuration, framesData));
			m_textureArrs[id_].m_texArr = reqElem;
			return reqElem;
		}
	}
	else
	{
		//Logger::print("Texture arr already exist\n");
		return m_textureArrs[id_].m_texArr.lock();
	}
}

void AnimationManager::preload(int toPreload_)
{
	if (m_textureArrs[toPreload_].m_preloaded == nullptr)
	{
		m_textureArrs[toPreload_].m_preloaded = std::shared_ptr<TextureArr>(getTextureArr(toPreload_));
	}
}

void AnimationManager::preload(const std::string &toPreload_)
{
	int id = getAnimID(toPreload_);

	preload(id);
}

int AnimationManager::getAnimID(const std::string &animName_) const
{
	try
	{
    	return m_ids.at(animName_);
	}
	catch (std::out_of_range exc_)
	{
		throw std::string("Failed to find animation ") + animName_ + " : " + exc_.what();
	}
}

//Properly release all textures
TextureArr::~TextureArr()
{
	//Logger::print("Release " + intToString(amount) + " textures\n");
	for (int i = 0; i < amount; ++i)
		SDL_DestroyTexture(tex[i]);
	delete[] tex;
}

Animation::Animation(AnimationManager &animationManager_, int id_, LOOPMETHOD isLoop_, int beginFrame_, int beginDirection_) :
	m_isLoop(isLoop_),
	m_currentFrame(beginFrame_),
	m_direction(beginDirection_)
{
	m_textures = animationManager_.getTextureArr(id_);
}

void Animation::generateWhite(Renderer &renderer_)
{
	if (m_whiteTextures.get() != nullptr)
		return;

	SDL_Texture** texs = new SDL_Texture *[m_textures->amount];

	auto blendmode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ONE, SDL_BLENDOPERATION_MAXIMUM, SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ZERO, SDL_BLENDOPERATION_ADD);
	for (int i = 0; i < m_textures->amount; ++i)
	{
		SDL_Texture *regtex = m_textures->tex[i];

		texs[i] = renderer_.createTexture(m_textures->w, m_textures->h);

		renderer_.setRenderTarget(texs[i]);
        renderer_.fillRenderer({255, 255, 255, 0});

        SDL_SetTextureBlendMode(regtex, blendmode);
        renderer_.renderTexture(regtex, 0, 0, m_textures->w, m_textures->h);
        SDL_SetTextureBlendMode(regtex, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(texs[i], SDL_BLENDMODE_BLEND);
	}

	m_whiteTextures = std::make_unique<TextureArr>(texs, m_textures->amount, m_textures->totalDuration, m_textures->framesData);
	renderer_.setRenderTarget(nullptr);
}

void Animation::update()
{
	if (isFinished())
		animFinished();
	else
		m_currentFrame += m_direction;;
}

SDL_Texture* Animation::getSprite()
{
	if (m_currentFrame == -1)
		m_currentFrame = 0;
		
	return (*m_textures)[m_currentFrame];
}

SDL_Texture* Animation::getWhiteSprite()
{
	if (m_whiteTextures.get() == nullptr)
		return nullptr;

	if (m_currentFrame == -1)
		m_currentFrame = 0;
		
	return (*m_whiteTextures)[m_currentFrame];
}

bool Animation::isFinished()
{
	if (m_direction > 0)
		return m_currentFrame == m_textures->totalDuration - 1;
	else
		return !m_currentFrame;
}

void Animation::switchDir()
{
	m_direction *= -1;
}

void Animation::setDir(int dir_)
{
	assert(dir_ == 1 || dir_ == -1);

	m_direction = dir_;
}

void Animation::reset(int beginFrame_, int beginDirection_)
{
	m_currentFrame = beginFrame_;
	m_direction = beginDirection_;
}

void Animation::animFinished()
{
	switch (m_isLoop)
	{
	case (LOOPMETHOD::NOLOOP):
			m_direction = 0;
		break;

	case (LOOPMETHOD::JUMP_LOOP):
		if (m_currentFrame >= m_textures->totalDuration - 1)
			m_currentFrame = 0;
		else if (m_currentFrame <= 0)
			m_currentFrame = m_textures->totalDuration - 1;
		break;

	case (LOOPMETHOD::SWITCH_DIR_LOOP):
		m_direction *= -1;
		if (m_direction == -1)
			m_currentFrame = m_textures->totalDuration - 1;
		else
			m_currentFrame = 0;
		break;
	}
}

Vector2<float> Animation::getSize()
{
	return {(float)m_textures->w, (float)m_textures->h};
}

int Animation::getDirection() const
{
	return m_direction;
}

Animation::Animation(Animation &&anim_)
{
	m_textures = std::move(anim_.m_textures);
	m_whiteTextures = std::move(anim_.m_whiteTextures);
	m_currentFrame = anim_.m_currentFrame;
	m_direction = anim_.m_direction;
	m_isLoop = anim_.m_isLoop;
}

Animation &Animation::operator=(Animation &&anim_)
{
	m_textures = std::move(anim_.m_textures);
	m_whiteTextures = std::move(anim_.m_whiteTextures);
	m_currentFrame = anim_.m_currentFrame;
	m_direction = anim_.m_direction;
	m_isLoop = anim_.m_isLoop;

	return *this;
}