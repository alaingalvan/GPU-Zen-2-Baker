#include "../Core/Actor.h"
#include "../Core/Utils.h"
class Baker : public Actor
{
public:

	Baker();

	virtual void update(float deltaTime) override;

protected:
	float mTime;

	unsigned mVertexArray;
	unsigned mProgram;
	unsigned mVertexShader;
	unsigned mFragmentShader;

	std::vector<unsigned> mIndexBufferData = {};
	unsigned mVertexBuffer;
	unsigned mIndexBuffer;
	unsigned mPositionAttrib;
	void recursive_render(const struct aiScene *sc, const struct aiNode* nd);
};

// 🏭 Factory function
ActorPtr baker();