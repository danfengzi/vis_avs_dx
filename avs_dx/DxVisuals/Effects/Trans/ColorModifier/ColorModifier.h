#pragma once
#include "../../EffectImpl.hpp"
using namespace Hlsl::Trans::ColorModifier;

struct ColorModifierStructs
{
	struct AvsState
	{
		RString effect_exp[ 4 ];

		int m_recompute;

		int m_tab_valid;
		unsigned char m_tab[ 768 ];
		int AVS_EEL_CONTEXTNAME;
		double *var_r, *var_g, *var_b, *var_beat;
		int inited;
		int codehandle[ 4 ];
		int need_recompile;
		CRITICAL_SECTION rcs;
	};

	static ID3D11VertexShader* vertexShader()
	{
		return StaticResources::fullScreenTriangle;
	}

	struct PsData: public ColorModifierPS
	{
	};

	using StateData = EmptyStateData;
};

class ColorModifier : public EffectBase1<ColorModifierStructs>
{
public:
	inline ColorModifier( AvsState *pState ) : tBase( pState ) { }

	const Metadata& metadata() override;

	HRESULT render( RenderTargets& rt ) override;
};