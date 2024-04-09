#ifndef ANIMATION_MANAGER_H_
#define ANIMATION_MANAGER_H_
#include "TextureManager.h"
#include <fstream>
#include <filesystem>

//Texture array structure
struct TextureArr
{
	//Makes sure textures actually exist
	TextureArr(SDL_Texture** tex_, int amount_, int totalDuration_, const std::vector<int> &framesData_) :
		tex(tex_), amount(amount_), totalDuration(totalDuration_), framesData(framesData_)
	{
		//assert(t_tex);

		//Sizes are based on first sprite
		SDL_QueryTexture(tex[0], NULL, NULL, &w, &h);
	}

	TextureArr(SDL_Texture** tex_, int amount_, int totalDuration_, const std::vector<int> &framesData_, int w_, int h_) :
		tex(tex_), amount(amount_), totalDuration(totalDuration_), framesData(framesData_), w(w_), h(h_)
	{
	}

	SDL_Texture* operator[](const int rhs)
	{
		return tex[framesData[rhs]];
	}

	//Texture array and required info
	SDL_Texture** tex;
	int amount;
	int w, h;
	int totalDuration;
	std::vector<int> framesData;

	//Properly removes texture
	virtual ~TextureArr();
};

struct ContainedAnimationData
{
	std::string m_path;
	std::weak_ptr<TextureArr> m_texArr;
	std::shared_ptr<TextureArr> m_preloaded;
};

/**************************
*
*	Animation manager holds all texture arrays
*	and makes sure there is only one version of each
*	texture array exist and only if required
*
**************************/
class AnimationManager
{
public:
	AnimationManager(Renderer* renderer_, const std::string &rootPath_);
	std::shared_ptr<TextureArr> getTextureArr(int id_);
	void preload(const std::string &toPreload_);
	void preload(int toPreload_);

	int getAnimID(const std::string &animName_) const;

private:
	std::string m_rootPath;
	Renderer* m_renderer;

	std::map<std::string, int> m_ids;
	std::vector<ContainedAnimationData> m_textureArrs;
};

enum class LOOPMETHOD
{
	NOLOOP,
	JUMP_LOOP,
	SWITCH_DIR_LOOP
};

/**************************
*
*	Animation class
*	Call update each frame
*	Direction = 1 - direct order
*	Direction = -1 - reverse order
*
**************************/
class Animation
{
public:
	Animation(AnimationManager &animationManager_, int id_, LOOPMETHOD isLoop_ = LOOPMETHOD::JUMP_LOOP, int beginFrame_ = -1, int beginDirection_ = 1);
	void generateWhite(Renderer &renderer_);
	void update();
	SDL_Texture* getSprite();
	SDL_Texture* getWhiteSprite();
	bool isFinished();
	void switchDir();
	void setDir(int dir_);
	Vector2<float> getSize();
	void reset(int beginFrame_ = -1, int beginDirection_ = 1);
	int getDirection() const;


	Animation(Animation &anim_) = delete;
	Animation &operator=(Animation &anim_) = delete;
	Animation(Animation &&anim_);
	Animation &operator=(Animation &&anim_);

private:
	std::shared_ptr<TextureArr> m_textures;
	std::unique_ptr<TextureArr> m_whiteTextures;
	int m_currentFrame;
	int m_direction;
	LOOPMETHOD m_isLoop;
	void animFinished();

};

using Animation_t = std::shared_ptr<Animation>;

#endif