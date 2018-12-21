#pragma once
#include "../../EffectImpl.hpp"
#include "../../../Expressions/CompiledShader.h"
#include "../PointSpritesRender.hpp"
#include <variant>
using namespace Hlsl::Render::SuperScope;

// ==== Common stuff for all styles ====
enum struct eScopeStyle : uint8_t
{
	Dots,
	Lines
};

struct ScopeBase
{
public:
	struct AvsState
	{
		RString effect_exp[ 4 ];
		int which_ch;
		int num_colors;
		int colors[ 16 ];
		int mode;

		eScopeStyle style() const;

		float sampleV() const;
	};

	struct FixedStateData : public SuperScopeState
	{
		FixedStateData( const AvsState& s )
		{
			update( s );
		}

		HRESULT update( const AvsState& ass );

		static inline UINT stateSize() { return 4; }
	};

	struct DynamicStateData : public Expressions::Compiler
	{
		DynamicStateData();
	};

	struct StateData
	{
	};
};


class SuperScope
{
public:
};