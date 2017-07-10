/*
 * http://github.com/dusty-nv/jetson-reinforcement
 */

#ifndef __REINFORCEMENT_LEARNING_AGENT_H_
#define __REINFORCEMENT_LEARNING_AGENT_H_


#include <stdio.h>
#include <stdint.h>
#include <string>

#include "aiAgent.h"
#include "pyTensor.h"


/**
 * Default name of the Python module to load
 */
#define DEFAULT_RL_MODULE "DQN"

/**
 * Default name of the Python function from the user's module
 * which infers the next action from the current state.
 * The expected function is of the form `def next_action(state):`
 * where state is a pyTorch tensor containing the environment,
 * and the function returns the predicted action.
 */
#define DEFAULT_NEXT_ACTION "next_action"

/**
 * Default name of the Python function from the user's module
 * which recieves rewards and performs training.
 * The expected reward function is of the form 
 * `def next_reward(state, reward, new_episode):`, where the
 * function returns the predicted action and accepts the reward.
 */
#define DEFAULT_NEXT_REWARD "next_reward"

/**
 * Default name of the Python function for loading model checkpoints
 */
#define DEFAULT_LOAD_MODEL "load_model"

/**
 * Default name of the Python function for saving model checkpoints
 */
#define DEFAULT_SAVE_MODEL "save_model"


/**
 * Base class for deep reinforcement learning agent,
 * using Python & pyTorch underneath with C FFI.
 */
class rlAgent : public aiAgent
{
public:
	/**
	 * Create a new instance of a module for training an agent.
	 */
	static rlAgent* Create( uint32_t numInputs, uint32_t numActions, 
					    const char* module=DEFAULT_RL_MODULE,
					    const char* nextAction=DEFAULT_NEXT_ACTION, 
					    const char* nextReward=DEFAULT_NEXT_REWARD,
					    const char* loadModel=DEFAULT_LOAD_MODEL,
					    const char* saveModel=DEFAULT_SAVE_MODEL );

	/**
	 * Create a new instance of a module for training an agent.
	 */
	static rlAgent* Create( uint32_t width, uint32_t height, 
					    uint32_t channels, uint32_t numActions, 
					    const char* module=DEFAULT_RL_MODULE,
					    const char* nextAction=DEFAULT_NEXT_ACTION, 
					    const char* nextReward=DEFAULT_NEXT_REWARD,
					    const char* loadModel=DEFAULT_LOAD_MODEL,
					    const char* saveModel=DEFAULT_SAVE_MODEL );

	/**
	 * Destructor
	 */
	virtual ~rlAgent();

	/**
	 * From the input state, predict the next action (inference)
	 * This function isn't used during training, for that see NextReward()
	 */
	virtual bool NextAction( Tensor* state, int* action );

	/**
	 * Next action with reward (training)
	 */
	virtual bool NextReward( float reward, bool end_episode );

	/**
	 * Load model checkpoint
	 */
	virtual bool LoadCheckpoint( const char* filename );

	/**
 	 * Save model checkpoint
	 */
	virtual bool SaveCheckpoint( const char* filename );

	/**
	 * Globally load Python scripting interpreter.
	 * LoadInterpreter is automatically called before tensors or scripts are run.
	 * It can optionally be called by the user at the beginning of their program to
	 * load Python at that time. It has internal protections to only be called once.
	 */
	static bool LoadInterpreter();

	/**
	 * Load Python script module
	 */
	bool LoadModule( const char* module );

	/**
	 * Load Python script module (with arguments)
	 */
	bool LoadModule( const char* module, int argc, char** argv );

	/**
	 * GetType
	 */
	virtual TypeID GetType() const 	{ return TYPE_RL; }

	/**
 	 * TypeID
	 */
	const TypeID TYPE_RL = TYPE_AI | (1 << 1);

protected:
	rlAgent();

	virtual bool Init( uint32_t width, uint32_t height, uint32_t channels, 
				    uint32_t numActions, const char* module, 
				    const char* nextAction, const char* nextReward,
				    const char* loadModel, const char* saveModel );
#ifdef USE_LUA
	lua_State* L;		/**< Lua/Torch7 operating environment */
	THCState*  THC;	/**< cutorch state */
#endif

	//bool mNewEpisode;

	uint32_t mInputWidth;
	uint32_t mInputHeight;
	uint32_t mNumInputs;
	uint32_t mNumActions;
	
	Tensor* mRewardTensor;
	Tensor* mActionTensor;

	enum
	{
		ACTION_FUNCTION = 0,
		REWARD_FUNCTION,
		LOAD_FUNCTION,
		SAVE_FUNCTION,
		NUM_FUNCTIONS
	};

	std::string mModuleName;
	void*	  mModuleObj;
	void* 	  mFunction[NUM_FUNCTIONS];
	void*	  mFunctionArgs[NUM_FUNCTIONS];
	std::string mFunctionName[NUM_FUNCTIONS];

	static bool scriptingLoaded;
};


#endif